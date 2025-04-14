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

// MainToolBar.cpp : implementation file
//

#include "pch.h"
#include "OpenHoldem.h"
#include "MyCToolBar.h"

// CMyToolBar

IMPLEMENT_DYNAMIC(CMyToolBar, CToolBar)

CMyToolBar::CMyToolBar() 
{
}

CMyToolBar::~CMyToolBar() 
{
}

BEGIN_MESSAGE_MAP(CMyToolBar, CToolBar)
END_MESSAGE_MAP()


void CMyToolBar::OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHandler) 
{
}
