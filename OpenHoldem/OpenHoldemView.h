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

#ifndef INC_OPENHOLDEMVIEW_H
#define INC_OPENHOLDEMVIEW_H

#include "OpenHoldemDoc.h"
#include "Card.h"

#define DISPLAY_UPDATE_TIMER	111

// OpenHoldemView.h : interface of the COpenHoldemView class

class COpenHoldemView : public CView {
 protected: // create from serialization only
	COpenHoldemView();
	DECLARE_DYNCREATE(COpenHoldemView)
	DECLARE_MESSAGE_MAP()
  afx_msg void OnTimer(UINT_PTR nIDEvent);
 public:
	COpenHoldemDoc* GetDocument() const;
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual ~COpenHoldemView();
	virtual void OnInitialUpdate();
 private:
	// private functions and variables - not available via accessors or mutators
	void UpdateDisplay(const bool update_all);
	void DrawButtonIndicators();
	void DrawSpecificButtonIndicator(const char ch, const bool on_off, const int left, const int top, const int right, const int bottom);
	void DrawSeatedActiveCircle(const int chair);
	void DrawCard(Card *card, const int left, const int top, const int right, const int bottom, const bool pl_card);
	void DrawDealerButton(const int chair); 
	void DrawNameBox(const int chair); 
	void DrawBalanceBox(const int chair); 
	void DrawPlayerBet(const int chair);
	void DrawPlayerCards(const int chair);
  void DrawColourCodes(const int chair);
 private:
	CString	_handnumber_last;
	double	_sblind_last, _bblind_last, _lim_last, _ante_last, _pot_last;
	int	    _iterator_thread_progress_last;
	int	    _card_common_last[kNumberOfCommunityCards];
	int	    _card_player_last[kMaxNumberOfPlayers][kMaxNumberOfCardsPerPlayer];
	bool		_seated_last[kMaxNumberOfPlayers];
	bool	  _active_last[kMaxNumberOfPlayers];
	CString	_playername_last[kMaxNumberOfPlayers];
	bool		_dealer_last[kMaxNumberOfPlayers]; 
	bool    _istournament_last; 
	double  _playerbalance_last[kMaxNumberOfPlayers]; 
	double  _playerbet_last[kMaxNumberOfPlayers];
	LOGFONT	_logfont;
	CPen		_black_pen, _green_pen, _red_pen, _blue_pen, _white_dot_pen, _null_pen;
	CBrush	_white_brush, _gray_brush, _red_brush, _yellow_brush;
	RECT		_client_rect;
};

inline COpenHoldemDoc* COpenHoldemView::GetDocument() const
   { return reinterpret_cast<COpenHoldemDoc*>(m_pDocument); }


#endif //INC_OPENHOLDEMVIEW_H