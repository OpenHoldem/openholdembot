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


#include "StdAfx.h"
#include "StickyButton.h"

const UINT WM_STICKYBUTTONDOWN = RegisterWindowMessage("WM_STICKYBUTTONDOWN");
const UINT WM_STICKYBUTTONUP = RegisterWindowMessage("WM_STICKYBUTTONUP");

CStickyButton::CStickyButton(void) 
{
	m_bState = false;
	m_bIgnore = false;
}

CStickyButton::~CStickyButton(void) 
{
}
BEGIN_MESSAGE_MAP(CStickyButton, CButton)
	ON_CONTROL_REFLECT(BN_CLICKED, &CStickyButton::OnBnClicked)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void CStickyButton::OnBnClicked() 
{
	if (m_bIgnore) {

		m_bIgnore = false;
		SetState(true);
	}
	else 
	{
		m_bState = !m_bState;
		SetState(m_bState); 
		ASSERT(GetParent());
		GetParent()->SendMessage(m_bState ? WM_STICKYBUTTONDOWN : WM_STICKYBUTTONUP, (WPARAM) this->GetSafeHwnd());
	}
}

void CStickyButton::OnKillFocus(CWnd* pNewWnd) 
{
	if (m_bState) 
	{
		m_bIgnore = true;
	} 

	// The following function pops up the button if it's down:
	CButton::OnKillFocus(pNewWnd);
}
