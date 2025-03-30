//****************************************************************************** 
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//****************************************************************************** 
//
// Purpose: keyboarddll.cpp : Defines the entry point for the DLL application.
//
// This version improves human–like typing by:
//   - Using variable delays between keystrokes.
//   - Using variable key press durations.
//   - Adding extra delay based on simulated finger movement between keys (using a QWERTY grid),
//     the delay is computed based on the Euclidean distance.
//   - Simulating occasional typos and corrections:
//       • For digits–only strings, a nearby digit is chosen as the error.
//       • Otherwise, a nearby letter (from the QWERTY layout) is chosen.
//   - A config option "DeletionKey" lets you choose between Backspace (0) or Delete (1)
//     for correcting simulated errors.
//   - All settings are read from "keyboard.cfg" (in the same folder as keyboard.dll), if the "keyboard.cfg"
//     file is not present, the default values from the KeyboardConfig struct will be used.
//   - For characters needing a modifier (e.g. uppercase letters), if the modifier is not
//     already held, a simulated keydown is performed (then later released) so that the effect
//     is as if a human tapped the modifier.
//   - For digits and lowercase letters, no modifiers are forced.
// 
//******************************************************************************

#include <windows.h>
#include <math.h>
#include <atlstr.h>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <vector>
#include "keyboarddll.h"

// Global constant definitions for modifier bit flags.
const int Shift = 1;   // Bit 0x01
const int Control = 2;   // Bit 0x02
const int Alt = 4;   // Bit 0x04

					 //--------------------------------------------------------------------------
					 // Global configuration structure and defaults.
struct KeyboardConfig {
	// Timing parameters
	int interKeyDelayMin;         // Minimum delay between keystrokes (ms)
	int interKeyDelayMax;         // Maximum delay between keystrokes (ms)
	int keyPressDurationMin;      // Minimum duration a key is held (ms)
	int keyPressDurationMax;      // Maximum duration a key is held (ms)
	int fingerMovementDelayFactor;// Extra delay (ms) per grid unit of finger movement

								  // Error simulation parameters
	int errorSimulationProbability;   // Chance (percent) to simulate a typo per key press
	double errorCandidateThreshold;   // Maximum grid distance (in grid units) for error candidates
	int deletionKeyChoice;            // 0 = use Backspace, 1 = use Delete
};

// Recommended default values:
KeyboardConfig g_config = { 90, 450, 80, 150, 30, 6, 1.0, 0 };

//---------------------------------------------------------------------------------------------
// Load configuration from "keyboard.cfg" if present. (located in the same folder as the DLL)
//---------------------------------------------------------------------------------------------
void LoadConfig(HMODULE hModule)
{
	char modulePath[MAX_PATH];
	if (GetModuleFileNameA(hModule, modulePath, MAX_PATH) == 0)
		return;
	// Remove the filename from modulePath.
	for (int i = (int)strlen(modulePath) - 1; i >= 0; i--) {
		if (modulePath[i] == '\\' || modulePath[i] == '/') {
			modulePath[i + 1] = '\0';
			break;
		}
	}
	char cfgPath[MAX_PATH];
	strcpy_s(cfgPath, MAX_PATH, modulePath);
	strcat_s(cfgPath, MAX_PATH, "keyboard.cfg");

	// Read timing parameters.
	g_config.interKeyDelayMin = GetPrivateProfileIntA("Timing", "InterKeyDelayMin", g_config.interKeyDelayMin, cfgPath);
	g_config.interKeyDelayMax = GetPrivateProfileIntA("Timing", "InterKeyDelayMax", g_config.interKeyDelayMax, cfgPath);
	g_config.keyPressDurationMin = GetPrivateProfileIntA("Timing", "KeyPressDurationMin", g_config.keyPressDurationMin, cfgPath);
	g_config.keyPressDurationMax = GetPrivateProfileIntA("Timing", "KeyPressDurationMax", g_config.keyPressDurationMax, cfgPath);
	g_config.fingerMovementDelayFactor = GetPrivateProfileIntA("Timing", "FingerMovementDelayFactor", g_config.fingerMovementDelayFactor, cfgPath);

	// Read error simulation parameters.
	g_config.errorSimulationProbability = GetPrivateProfileIntA("Error", "ErrorSimulationProbability", g_config.errorSimulationProbability, cfgPath);

	char thresholdStr[32];
	GetPrivateProfileStringA("Error", "ErrorCandidateThreshold", "1.0", thresholdStr, 32, cfgPath);
	g_config.errorCandidateThreshold = atof(thresholdStr);

	g_config.deletionKeyChoice = GetPrivateProfileIntA("Error", "DeletionKey", g_config.deletionKeyChoice, cfgPath);
}

//--------------------------------------------------------------------------
// Helper Functions
//--------------------------------------------------------------------------

// Returns a random delay (in ms) between keystrokes.
int RandomInterKeyDelay() {
	return g_config.interKeyDelayMin + (rand() % (g_config.interKeyDelayMax - g_config.interKeyDelayMin + 1));
}

// Returns a random duration (in ms) for which a key is held down.
int RandomKeyPressDuration() {
	return g_config.keyPressDurationMin + (rand() % (g_config.keyPressDurationMax - g_config.keyPressDurationMin + 1));
}

// Returns an approximate QWERTY layout position for alphanumeric keys.
POINT GetKeyPosition(BYTE vkey) {
	POINT pt = { 0, 0 };
	if (vkey >= '0' && vkey <= '9') {
		const char* digits = "1234567890";
		const char* found = strchr(digits, vkey);
		if (found) {
			pt.x = (int)(found - digits);
			pt.y = -1; // assign row for digits
		}
	}
	else {
		char letter = (char)vkey;
		if (letter >= 'a' && letter <= 'z')
			letter -= ('a' - 'A');
		const char* top = "QWERTYUIOP";
		const char* mid = "ASDFGHJKL";
		const char* bot = "ZXCVBNM";
		const char* found = strchr(top, letter);
		if (found) {
			pt.x = (int)(found - top);
			pt.y = 0;
		}
		else {
			found = strchr(mid, letter);
			if (found) {
				pt.x = (int)(found - mid);
				pt.y = 1;
			}
			else {
				found = strchr(bot, letter);
				if (found) {
					pt.x = (int)(found - bot);
					pt.y = 2;
				}
			}
		}
	}
	return pt;
}

// Calculates the Euclidean distance between two points.
double Distance(POINT a, POINT b) {
	int dx = a.x - b.x;
	int dy = a.y - b.y;
	return sqrt(dx*dx + dy*dy);
}

// Given an intended character and a flag (digitsOnly), returns a nearby wrong
// character using the QWERTY layout—only keys within errorCandidateThreshold (in grid units) are considered.
char GetErrorCharacter(char intended, bool digitsOnly) {
	char upIntended = intended;
	if (upIntended >= 'a' && upIntended <= 'z')
		upIntended -= ('a' - 'A');
	POINT intendedPos = GetKeyPosition((BYTE)upIntended);

	const char* allowed = digitsOnly ? "1234567890" : "QWERTYUIOPASDFGHJKLZXCVBNM";
	std::vector<char> candidates;
	double threshold = g_config.errorCandidateThreshold; // grid units
	int len = (int)strlen(allowed);
	for (int i = 0; i < len; i++) {
		char candidate = allowed[i];
		POINT candidatePos = GetKeyPosition((BYTE)candidate);
		double dist = Distance(intendedPos, candidatePos);
		if (dist > 0 && dist <= threshold) {
			candidates.push_back(candidate);
		}
	}
	if (candidates.empty())
		return intended;
	int index = rand() % candidates.size();
	char errorChar = candidates[index];
	if (!digitsOnly && (intended >= 'a' && intended <= 'z'))
		errorChar += ('a' - 'A'); // restore lowercase if needed
	return errorChar;
}

//--------------------------------------------------------------------------
// DllMain
//
// On process attach, seed the random generator and load configuration.
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		srand((unsigned int)time(NULL));
		LoadConfig(hModule);
	}
	return TRUE;
}

//--------------------------------------------------------------------------
// PlayKeyboardEvent
//
// For each modifier indicated by bscan:
//   - If required and not already physically pressed, simulate a keydown (then later release it)
//     so that the main key is sent with the modifier.
//   - If already pressed, leave it as is.
// Then press the main key (held for a random duration) and release it.
// Finally, release any simulated modifiers.
void PlayKeyboardEvent(int vkey, int bscan)
{
	bool ctrlSimulated = false, shiftSimulated = false, altSimulated = false;

	if (bscan & Control) {
		if (!(GetKeyState(VK_CONTROL) & 0x8000)) {
			keybd_event(VK_CONTROL, 0, 0, 0);
			ctrlSimulated = true;
		}
	}
	if (bscan & Shift) {
		if (!(GetKeyState(VK_SHIFT) & 0x8000)) {
			keybd_event(VK_SHIFT, 0, 0, 0);
			shiftSimulated = true;
		}
	}
	if (bscan & Alt) {
		if (!(GetKeyState(VK_MENU) & 0x8000)) {
			keybd_event(VK_MENU, 0, 0, 0);
			altSimulated = true;
		}
	}

	keybd_event(vkey, 0, 0, 0);
	Sleep(RandomKeyPressDuration());
	keybd_event(vkey, 0, KEYEVENTF_KEYUP, 0);

	if (ctrlSimulated)
		keybd_event(VK_CONTROL, 0, KEYEVENTF_KEYUP, 0);
	if (shiftSimulated)
		keybd_event(VK_SHIFT, 0, KEYEVENTF_KEYUP, 0);
	if (altSimulated)
		keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);
}

//--------------------------------------------------------------------------
// SendString
//
// Sends the given CString as keystrokes with human–like timing, error simulation,
// and finger movement delays.
//   - For digits ('0'-'9') and lowercase letters ('a'-'z'), no modifiers are used.
//   - For other characters, modifier bits from VkKeyScan() are used.
//   - With a chance defined by errorSimulationProbability, a typo is simulated then corrected.
//       • If the string is digits–only, the error is a nearby digit.
//   - Extra delay for error correction is computed from the Euclidean distance between the error key
//     and the deletion key (which is determined by deletionKeyChoice).
KEYBOARDDLL_API int SendString(const HWND hwnd, const RECT rect, const CString s, const bool use_comma)
{
	SetFocus(hwnd);
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);

	char ch_str[256];
	sprintf_s(ch_str, 256, "%s", s.GetString());

	int length = (int)strlen(ch_str);
	bool digitsOnly = true;
	for (int j = 0; j < length; j++) {
		if (ch_str[j] < '0' || ch_str[j] > '9') {
			digitsOnly = false;
			break;
		}
	}

	POINT prevPos = { 0, 0 };
	bool firstKey = true;

	for (int i = 0; i < length; i++) {
		if (use_comma && ch_str[i] == '.')
			ch_str[i] = ',';

		short keyscan = VkKeyScan(ch_str[i]);
		BYTE vkey = LOBYTE(keyscan);
		BYTE mods = HIBYTE(keyscan);

		// Force no modifiers for digits and lowercase letters.
		if ((ch_str[i] >= '0' && ch_str[i] <= '9') ||
			(ch_str[i] >= 'a' && ch_str[i] <= 'z'))
		{
			mods = 0;
		}

		// --- Error Simulation ---
		if (rand() % 100 < g_config.errorSimulationProbability) {
			char wrongChar = GetErrorCharacter(ch_str[i], digitsOnly);
			short wrongKeyScan = VkKeyScan(wrongChar);
			BYTE wrongVkey = LOBYTE(wrongKeyScan);
			BYTE wrongMods = HIBYTE(wrongKeyScan);
			if (digitsOnly || (wrongChar >= 'a' && wrongChar <= 'z'))
				wrongMods = 0;
			// Type the wrong key.
			PlayKeyboardEvent(wrongVkey, wrongMods);
			Sleep(RandomInterKeyDelay());

			// Compute extra delay for error correction based on the distance from the error key
			// to the deletion key.
			POINT errorPos = GetKeyPosition(wrongVkey);
			POINT deletionPos;
			if (g_config.deletionKeyChoice == 0) {
				// Backspace: fixed coordinate (10,0)
				deletionPos.x = 10;
				deletionPos.y = 0;
			}
			else {
				// Delete: fixed coordinate (10,1)
				deletionPos.x = 10;
				deletionPos.y = 1;
			}
			double delDist = Distance(errorPos, deletionPos);
			int extraDelay = (int)(g_config.fingerMovementDelayFactor * delDist);
			Sleep(extraDelay);

			// Send the deletion key.
			if (g_config.deletionKeyChoice == 0)
				PlayKeyboardEvent(VK_BACK, 0);
			else
				PlayKeyboardEvent(VK_DELETE, 0);

			Sleep(RandomInterKeyDelay());
		}

		// Send the correct key.
		PlayKeyboardEvent(vkey, mods);

		POINT currPos = GetKeyPosition(vkey);
		if (!firstKey) {
			double dist = Distance(prevPos, currPos);
			int extraDelay = (int)(g_config.fingerMovementDelayFactor * dist);
			Sleep(extraDelay);
		}
		else {
			firstKey = false;
		}
		prevPos = currPos;

		Sleep(RandomInterKeyDelay());
	}

	return TRUE;
}

//--------------------------------------------------------------------------
// SendKey
//
// Sends a single key (or a two–key combo like Ctrl+X) to the specified window,
// using variable key press duration. For a two–key combo, only the main key is sent.
KEYBOARDDLL_API int SendKey(const HWND hwnd, const RECT rect, const char* vkey)
{
	SetFocus(hwnd);
	SetForegroundWindow(hwnd);
	SetActiveWindow(hwnd);

	INPUT input[4];
	ZeroMemory(&input, sizeof(input));
	int input_count = 0;

	if ((vkey[0] == VK_CONTROL || vkey[0] == VK_SHIFT || vkey[0] == VK_MENU) && vkey[1] != '\0') {
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = vkey[1];
		input_count++;

		Sleep(RandomKeyPressDuration());

		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = vkey[1];
		input[input_count].ki.dwFlags = KEYEVENTF_KEYUP;
		input_count++;
	}
	else {
		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = vkey[0];
		input_count++;

		Sleep(RandomKeyPressDuration());

		input[input_count].type = INPUT_KEYBOARD;
		input[input_count].ki.wVk = vkey[0];
		input[input_count].ki.dwFlags = KEYEVENTF_KEYUP;
		input_count++;
	}

	SendInput(input_count, input, sizeof(INPUT));
	Sleep(RandomInterKeyDelay());

	return TRUE;
}

//--------------------------------------------------------------------------
// ProcessMessage
//
// Stub function: does nothing (we keep it in case soemthing else calls this?).
KEYBOARDDLL_API void ProcessMessage(const char *message, const void *param)
{
	if (message == NULL)
		return;
	// No operation.
}
