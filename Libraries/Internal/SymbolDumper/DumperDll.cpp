#include "DumperDll.h"

#if WIN32
#include <windows.h>
#include <atlstr.h>
#include <conio.h>
#else //is LINUX

App* appForDumpDll = 0;

#endif

#include "AllSymbols.h"

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <streambuf>
//for atoi
#include <cstdlib>
#include <algorithm>



#include "minIni.h"
//list dir, needs wrapper header on windows
#include "dirent.h"


////////////////////////////////////
//consecutive states
holdem_state	m_holdem_state[256];
unsigned char	m_ndx;
////////////////////////////////////
const char* cfgInFileName = "DumperDll.cfg";
//default values, are overridden form config file
#define MAX_CFG_VALUE_LEN 512
std::string dumpOutFileName("DumperDll.log");
//fixme: change
std::string testfilesDir("TestFileDirNotConfigured");

std::ofstream dumpOutFile;
unsigned int count;

int handsplayed = -1;
//int numActionsInHand = 0;
int prevHandsplayed = -2;

int inHandMsgNum = 0;
bool isAllInMsg = false;

int nextTestFilenameIdx = 0;
size_t currentOffset = 0;
std::vector<std::string> testFilenameList;
//std::ifstream curTestFile;
std::string curTestFileContent("");

//for logging only
std::string curTestFilename("");

#define LOG_DUMPER_DEBUG
#ifdef LOG_DUMPER_DEBUG
void debugDumper(std::string m) {
  dumpOutFile << m << std::endl;
}
#endif

typedef struct TestSuiteAction_ {
  TestSuiteAction_(): raise(false), raiseTo(false), fold(false), call(false), allIn(false), raiseToVal(0.0) {}
  bool raise;
  bool raiseTo;
  bool fold;
  bool call;
  //not working because we abuse the button? or does it?
  bool allIn;
  double raiseToVal;
} TestSuiteAction;

TestSuiteAction curTestSuiteAction;

void printCurAction() {
	//std::cout
	dumpOutFile << "curTestSuiteAction{fold=" << curTestSuiteAction.fold << ", call=" << curTestSuiteAction.call <<", allIn=" << curTestSuiteAction.allIn <<
    ", raise=" << curTestSuiteAction.raise << ", raiseTo=" << curTestSuiteAction.raiseTo << ", raiseToVal=" << curTestSuiteAction.raiseToVal << "}" << std::endl;
}



p_getsym_t		p_get_symbol = NULL;
p_send_chat_t		p_send_chat  = NULL;

double getsym( int chair, const char* name, bool& iserr ) 
{
#ifdef WIN32
#ifndef OH_2
   //FIXME: user 0 for chair all the time
   return GetSymbol(name);
#else //is OH_2
   return (*p_get_symbol)(chair,name,iserr);
#endif //OH_2

#else //not WIN32
   return (*p_get_symbol)(appForDumpDll, chair,name,iserr);
#endif //WIN32
}

double getsym( const char* name ) 
{
   bool iserr;
   //chair seems not to be used at all, but pass it for now
   int mychair = (int) getsym(0, "userchair", iserr);
   return getsym(mychair, name, iserr);
}

void send_chat(char* message)
{
	(*p_send_chat)(message);
}



char rankMap[] = { 'X', 'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A' };

#ifndef OH_2
   char suitMap[] = { 'h', 'd', 'c', 's' };
#else //is OH_2
   //OH old/OH2: value-1 to suit: 0-c, 1-d, 2-h, 3-s
   char suitMap[] = { 'Y', 'c', 'd', 'h', 's' };
   //for trunk (compared to old/OH2) we get 0 for h(3), 3 for s(4), 1 for d(2), 2 for c(1)
#endif //OH_2

/**
 * res buffer must be at least 5 long
 **/
void convertCardToString(int c, char* res) {
	//dw DEBUG:
	//sprintf(res, "%i", c);

	//from OH 5.0.1 on: We use now only CARD_NOCARD (0xfe = 254).
	if (c == 0 || c == 254 || c == 253) {
	    strcpy(res, "NONE");
	    return;
	}
	else if (c == 255) {
	    strcpy(res, "BACK");
	    return;
	}
	
	int rank = c & 0xf0;
	rank = rank >> 4;
	res[0] = rankMap[rank];
	int suit = c & 0x0f;
	res[1] = suitMap[suit];
	res[2] = '\0';
}



void getConfigValue(const char* name, std::string& value) {
  char ini_buffer[MAX_CFG_VALUE_LEN];
  ini_gets("DumperDllCfg", name, value.c_str(), ini_buffer, MAX_CFG_VALUE_LEN, cfgInFileName);
  value = ini_buffer;
}

void initTestFilenameList() {
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir (testfilesDir.c_str())) != NULL) {
    // print all the files and directories within directory
    while ((ent = readdir (dir)) != NULL) {
      //printf ("%s\n", ent->d_name);
      if (ent->d_name[0] != '.') {
	testFilenameList.push_back(testfilesDir + "/" + std::string(ent->d_name));
      }
    }
    closedir(dir);
    
    std::sort(testFilenameList.begin(), testFilenameList.end());
    
  } else {
    // could not open directory, we fail later when not finding any test files
  }
}

/**
 * parse FCKRA, just single char and for R amount if given
 * returns position after ',' or position of: ')' or '\n' or '\r'
 * start postion should not be ',', will just return position after ','
 */
size_t parseCurTestSuiteAction(size_t startOff) {
  while(startOff < curTestFileContent.size() && curTestFileContent.at(startOff) != '\r'
    && curTestFileContent.at(startOff) != '\n' && curTestFileContent.at(startOff) != ',' && curTestFileContent.at(startOff) != ')') {
    if (curTestFileContent.at(startOff) == 'F') {
      curTestSuiteAction.fold = true;
      break;
    }
    else if (curTestFileContent.at(startOff) == 'C') {
      curTestSuiteAction.call = true;
      break;
    }
    else if (curTestFileContent.at(startOff) == 'K') {
      curTestSuiteAction.call = true;
      break;
    }
    //
    else if (curTestFileContent.at(startOff) == 'A') {
      curTestSuiteAction.allIn = true;
      break;
    }
    else if (curTestFileContent.at(startOff) == 'R') {
      startOff++;
      if (startOff + 1 >= curTestFileContent.size() || curTestFileContent.at(startOff) != ' ') {
	curTestSuiteAction.raise = true;
      }
      else {
	startOff++;
	curTestSuiteAction.raiseTo = true;
	std::string numberChars("0123456789.");
	size_t endOff = curTestFileContent.find_first_not_of(numberChars, startOff);
	if (endOff != std::string::npos && endOff > startOff) {
	  std::string numStr = curTestFileContent.substr(startOff, endOff);
	  curTestSuiteAction.raiseToVal = atof(numStr.c_str());
	}
	else {
	  //FIXME: should better treat that as error
	  curTestSuiteAction.raise = true;
	  curTestSuiteAction.raiseTo = false;
	}
      }
      break;
    }
    else {
      startOff++;
    }
  }
  
  //set result to first after ',' or to ')' exactly
  while(startOff < curTestFileContent.size() && curTestFileContent.at(startOff) != '\r'
	&& curTestFileContent.at(startOff) != '\n') {
    if (curTestFileContent.at(startOff) == ')') {
      break;
    }
    else if (startOff+1 < curTestFileContent.size() && curTestFileContent.at(startOff) == ',') {
      startOff = startOff + 1;
      break;
    }
    startOff++;
  }
  
  return startOff;
}

bool setCurAction() {
  if (handsplayed != prevHandsplayed && nextTestFilenameIdx < testFilenameList.size()) {
    currentOffset = 0;
    
    curTestFilename = testFilenameList[nextTestFilenameIdx];
    
    std::ifstream curTestFile(curTestFilename.c_str());
    
    curTestFileContent = std::string((std::istreambuf_iterator<char>(curTestFile)),
		    std::istreambuf_iterator<char>());
    
    curTestFile.close();
    nextTestFilenameIdx++;
    inHandMsgNum = 0;
  }
  //FCKRA
  currentOffset = curTestFileContent.find(" can ", currentOffset);
  if (currentOffset != std::string::npos) {
    currentOffset = curTestFileContent.find(" do ", currentOffset);
  }
  
  if (currentOffset != std::string::npos) {
    currentOffset += 4;
    //parse it, e.g. '(R 20.2, C),' or 'F,' or 'F'
    
    curTestSuiteAction = TestSuiteAction();
    if (curTestFileContent.at(currentOffset) == '(') {
      size_t curStart = ++currentOffset;
      while (curTestFileContent.at(curStart) != ')') {
	curStart = parseCurTestSuiteAction(curStart);
      }
    }
    else {
      parseCurTestSuiteAction(currentOffset);
    }
    return true;
  }
  else {
    curTestFilename = "";
    curTestSuiteAction = TestSuiteAction();
    curTestSuiteAction.fold = true;
    return false;
  }
}


void dumpSymbols() {
	dumpOutFile << "====" << " BEGIN SYMBOLS DUMP inHandMsgNum=" << inHandMsgNum << " isAllInMsg=" << isAllInMsg << " count=" << count  << " (" << curTestFilename << ") ====" << std::endl;
	
	bool err_bool = false;
	
	int mychair = (int) getsym(0, "userchair", err_bool);
	
	int idx = 0;
	const char* curSymName = "";
	while (*(curSymName = all_symbol_names[idx++]) != '\0') {
	  double val = getsym(mychair, curSymName, err_bool);
	  dumpOutFile << curSymName << "=" << val << std::endl;
	}
	
	dumpOutFile << "====" << " END SYMBOLS DUMP inHandMsgNum=" << inHandMsgNum << " isAllInMsg=" << isAllInMsg << " count=" << count++ << " (" << curTestFilename << ") ====" << std::endl;
}

void dumpState(const holdem_state* s) {
  dumpOutFile << "====" << " BEGIN STATE DUMP inHandMsgNum=" << inHandMsgNum << " isAllInMsg=" << isAllInMsg << " count=" << count << " ====" << std::endl;
  dumpOutFile << "dealer=" << (int) s->m_dealer_chair << std::endl;
  dumpOutFile << "playing=" << (int) s->m_is_playing << std::endl;
  dumpOutFile << "posting=" << (int) s->m_is_posting << std::endl;
  dumpOutFile << "title=" << s->m_title << std::endl;

  char cardBuf[5];
  
  for (int i = 0; i < 5; i++) {
    convertCardToString(s->m_cards[i], cardBuf);
    dumpOutFile << "ccard[" << i << "]=" << cardBuf << std::endl;
  }

  for (int i = 0; i < 10; i++) {
    const holdem_player& p = s->m_player[i];
    dumpOutFile << "player[" << i << "].name_known=" << (int) p.m_name_known << std::endl;
    if (p.m_name_known) {
      dumpOutFile << "player[" << i << "].name=" << p.m_name << std::endl;
      dumpOutFile << "player[" << i << "].balance_known=" << (int) p.m_balance_known << std::endl;
      if (p.m_balance_known) {
	dumpOutFile << "player[" << i << "].balance=" << p.m_balance << std::endl;
      }
      dumpOutFile << "player[" << i << "].currentbet=" << p.m_currentbet << std::endl;
      convertCardToString(p.m_cards[0], cardBuf);
      dumpOutFile << "player[" << i << "].card0=" << cardBuf << std::endl;
      convertCardToString(p.m_cards[1], cardBuf);
      dumpOutFile << "player[" << i << "].card1=" << cardBuf << std::endl;
    }
  }
  for (int i = 0; i < 10; i++) {
    dumpOutFile << "pot[" << i << "]=" << s->m_pot[i] << std::endl;
  }
  
  dumpOutFile << "====" << " END STATE DUMP inHandMsgNum=" << inHandMsgNum << " isAllInMsg=" << isAllInMsg << " count=" << count++ << " (" << curTestFilename << ") ====" << std::endl;
}









double process_query( const char* pquery ) 
{
	
	if (pquery==NULL)
		return 0;

	//DEBUG only
	if (dumpOutFile.is_open()) {
	  debugDumper(std::string("call to process_query: ") + pquery);
	}
	
	//dump stuff on alli call, or is this a bad idea if we can only call? don't think so
	if (strncmp(pquery,"dll$alli",8)==0) {
	  
	  isAllInMsg = true;
	  
	  handsplayed = (int) getsym("handsplayed");
	  
	  //DEBUG only
	  char handStr[10];
	  sprintf(handStr, "%d", handsplayed);
	  debugDumper(std::string("cur handsplayed: ") + handStr);
	  
	  if (!setCurAction()) {
	    //something went wrong
	    dumpOutFile << "ERROR: did not find next TestSuite2 action for test file " << curTestFilename << std::endl;
	  }
	  else if (!curTestSuiteAction.fold && !curTestSuiteAction.call && !curTestSuiteAction.raise && !curTestSuiteAction.raiseTo && !curTestSuiteAction.allIn) {
	    dumpOutFile << "ERROR: no result on parsing FCKRA for TestSuite2 action in test file " << curTestFilename << std::endl;
	  }

	  
	  //DEBUG only
	  debugDumper(std::string("cur test file: ") + curTestFilename);
	  char cardBuf0[5], cardBuf1[5];
	  convertCardToString((int) getsym("$$pc0"), cardBuf0);
	  convertCardToString((int) getsym("$$pc1"), cardBuf1);
	  debugDumper(std::string("cards: ") + cardBuf0 + " " + cardBuf1);
	  
	  prevHandsplayed = handsplayed;
	  
	  printCurAction();
	  
	  dumpSymbols();
	  //dump previous state
	  holdem_state* prev_state = &m_holdem_state[ (m_ndx) & 0xff ];
	  dumpState(prev_state);
	  
	  if (curTestSuiteAction.allIn && !(curTestSuiteAction.raise || curTestSuiteAction.raiseTo || curTestSuiteAction.call)) {
		  debugDumper(std::string("process_query: alli=1"));
		  return 1.0;
		  //maybe don't do it if other is set too
		  //return 0;
	  }
	}
	else if (strncmp(pquery,"dll$call",8)==0) {
	  if (curTestSuiteAction.call) {
	    debugDumper(std::string("process_query: call=1"));
	    return 1.0;
	    
	  }
	}
	else if (strncmp(pquery,"dll$fold",8)==0) {
	  if (curTestSuiteAction.fold) {
	    debugDumper(std::string("process_query: fold=1"));
	    //does it even exist:
		return 1.0;
		//return 0;
	  }
	}
	else if (strncmp(pquery,"dll$rais",8)==0) {
	  if (curTestSuiteAction.raise || curTestSuiteAction.raiseTo) {
	    debugDumper(std::string("process_query: rais=1"));
	    return 1.0;
	  }
	  //debug:
	  //return 1.0;
	}
	else if (strncmp(pquery,"dll$swag",8)==0 || strncmp(pquery,"dll$betsize",11)==0) {
	  if (curTestSuiteAction.raiseTo) {
	    
	    //DEBUG only
	    char valueStr[10];
	    sprintf(valueStr, "%f", curTestSuiteAction.raiseToVal);
	    debugDumper(std::string("process_query: ") + pquery + "=" + valueStr);
	    
	    return curTestSuiteAction.raiseToVal;
	  }
	}

	return 0;
}

double process_state( holdem_state* pstate ) 
{
	if (pstate!=NULL) {	m_holdem_state[ (++m_ndx)&0xff ] = *pstate; }

	//_cprintf("state: %d\n", m_ndx&0xff);

	return 0;
}

/////////////////////////////////////////////////////
// user.dll entry point
/////////////////////////////////////////////////////
USERDLL_API double process_message (const char* pmessage, const void* param) 
{ 
	if (pmessage==NULL) { return 0; }
	
	inHandMsgNum++;
	isAllInMsg = false;
	
	//ensure files open
	if (!dumpOutFile.is_open()) {
	  //read config
	  getConfigValue("dumpOutFileName", dumpOutFileName);
	  getConfigValue("testfilesDir", testfilesDir);
	  dumpOutFile.open(dumpOutFileName.c_str());
	  initTestFilenameList();
	}
	

	if (strcmp(pmessage,"state")==0) 
	{ 
		std::cout << "process_message called with state" << std::endl;
		holdem_state *state = (holdem_state*) param;
//		CString s; 
//		s.Format("<%s>\nPOTS: %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f %.2f\nCOMMON: %d %d %d %d %d\n%d %d %d\n<%s> %d %.2f %d", 
//			state->m_title,
//			state->m_pot[0], state->m_pot[1], state->m_pot[2], state->m_pot[3], state->m_pot[4], 
//			state->m_pot[5], state->m_pot[6], state->m_pot[7], state->m_pot[8], state->m_pot[9],
//			state->m_cards[0], state->m_cards[1], state->m_cards[2], state->m_cards[3], state->m_cards[4],
//			state->m_is_playing, state->m_is_posting, state->m_dealer_chair,
//			state->m_player[4].m_name, state->m_player[4].m_name_known, state->m_player[4].m_balance, state->m_player[4].m_balance_known);
//		MessageBox(NULL, s, "state", MB_OK);

		return process_state( (holdem_state*)param ); 
	}

	if (strcmp(pmessage,"query")==0) 
	{ 
		std::cout << "process_message called with query " << (const char*) param << std::endl;
//		MessageBox(NULL, (LPCSTR) param, "query", MB_OK);
		return process_query( (const char*)param ); 
	}

	if (strcmp(pmessage,"pfgws")==0) 
	{	
//		MessageBox(NULL, "pfgws", "MESSAGE", MB_OK);
		p_get_symbol = (p_getsym_t)param;
		return 0;
	}

	if (strcmp(pmessage,"p_send_chat_message")==0) 
	{	
//		MessageBox(NULL, "p_send_chat_message", "MESSAGE", MB_OK);
		p_send_chat = (p_send_chat_t)param;
		return 0;
	}

	if (strcmp(pmessage,"event")==0 && strcmp((const char *) param, "load")==0) 
	{ 
//		MessageBox(NULL, "event-load", "MESSAGE", MB_OK);
	}

	if (strcmp(pmessage,"event")==0 && strcmp((const char *) param, "unload")==0) 
	{ 
//		MessageBox(NULL, "event-unload", "MESSAGE", MB_OK);
	}

	return 0;
}
/////////////////////////////////////////////////////

#ifdef WIN32
BOOL APIENTRY DllMain(HANDLE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
    switch (ul_reason_for_call)	
	{
		case DLL_PROCESS_ATTACH:
			//AllocConsole();
			break;
		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			//FreeConsole();
			break;
    }
    return TRUE;
}
#endif

