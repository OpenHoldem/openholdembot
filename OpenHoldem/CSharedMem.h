//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Synchronization, mainly info about connected tables
//   for the auto-connector
//
//******************************************************************************

#ifndef INC_CSHAREDMEM_H
#define INC_CSHAREDMEM_H

// Shared memory to coordinate the auto-connectors
// of multiple instances of OpenHoldem.
//
// For an introduction to shared memory see
//   * http://www.programmersheaven.com/mb/Win32API/156366/156366/shared-memory-and-dll/
//   * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2044.html
//   * http://www.codeproject.com/KB/threads/SharedMemory_IPC_Threads.aspx
//   * http://www.fstsoft.de/shared.htm (german)
//
// We chose the first solution for its simplicity

// names are limited to 8 chars, including the dot.
// If you changes this name you also have to change the linker pragmas
#define kOpenHoldemSharedmemorySegment ".ohshmem"

class CSharedMem {
 public:
	CSharedMem();
	~CSharedMem();
 public:
	// public accessors and mutators
	bool PokerWindowAttached(HWND Window);
	bool AnyWindowAttached();
	void MarkPokerWindowAsAttached(HWND Window);
	void RememberTimeOfLastFailedAttemptToConnect();
	time_t GetTimeOfLastFailedAttemptToConnect();
	// Used for the table positioner
	HWND *GetDenseListOfConnectedPokerWindows();
	int  SizeOfDenseListOfAttachedPokerWindows();
	bool IsAnyOpenHoldemProcess(int PID);
  bool IsDeadOpenHoldemProcess(int open_holdem_iD);
  void CleanUpProcessMemory(int open_holdem_iD);
 public:
  // For the auto-starter, as only one instance should start new bots
  // and a mutex is not enough, starting usually needs some seconds.
  // We can#t wait that long in the heartbeat and we can#t continue either
  // as then another instance might start more nots.
  int LowestConnectedSessionID();
  // Same but for termination
  int LowestUnconnectedSessionID();
  int NTablesConnected();
  int NUnoccupiedBots();
 public:
  // Public, as this might fail on some systems, on startup or in geberal,
  // so the watchdog might try to re-aquire while heartbeating.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20520
  void AquireOwnProcessID();
 public:
  int OpenHoldemProcessID();
  int OpenHoldemProcessID(int session_ID);
 private:
	void CreateDenseListOfConnectedPokerWindows();
	void VerifyMainMutexName();
  int NBotsPresent();
  void Dump(); 
 private:
	CCritSec	m_critsec;
};

extern CSharedMem *p_sharedmem;

#endif INC_CSHAREDMEM_H
