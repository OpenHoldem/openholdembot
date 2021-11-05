//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose:
//
//******************************************************************************

#ifndef INC_CREPLAYFRAME_H
#define INC_CREPLAYFRAME_H

class CReplayFrame {
	// CSymbolEngineReplayFrameController (and nobody else!) 
	// should get access to CreateReplayFrame();
	// to avoid multiple calls during the same heartbeat.
	friend class CSymbolEngineReplayFrameController;
 public:
	// public functions
	CReplayFrame();
	~CReplayFrame();
 private:
	// private functions and variables - not available via accessors or mutators
	void CreateReplayFrame();
	void CreateReplaySessionDirectoryIfNecessary();
	CString GetPlayerInfoAsHTML();
	CString GetButtonStatesAsHTML();
	CString GetBlindInfoAsHTML();
	CString GetCommonCardsAsHTML();
	CString GetPotsAsHTML();
	CString GetLinksToPrevAndNextFile();
	CString GeneralInfo();
	void CreateBitMapFile();
 private:
	static int _next_replay_frame;
};

#endif /* INC_CREPLAYFRAME_H */