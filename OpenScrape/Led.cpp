///////////////////////////////////////////////////////////////////////////////
// Led.cpp : implementation file
// Visual Source Safe: $Revision: 1 $
//
// Led static control. Will display a LED in 4 different colors and two shapes.
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"
#include "Led.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLed
#define TIMER_ID_PING		1		// Timer Ping ID

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
CLed::CLed()
{
	m_LedBitmap.LoadBitmap(IDB_LEDS);
	m_nLedColor = LED_COLOR_RED;
	m_nLedMode = LED_OFF;
	m_nLedShape = LED_ROUND;
}

CLed::~CLed()
{
	VERIFY(m_LedBitmap.DeleteObject());
}


BEGIN_MESSAGE_MAP(CLed, CStatic)
	//{{AFX_MSG_MAP(CLed)
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLed message handlers

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CLed::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	DrawLed(&dc,m_nLedColor,m_nLedMode,m_nLedShape);
	
	// Do not call CStatic::OnPaint() for painting messages
}


///////////////////////////////////////////////////////////////////////////////
// Name:		SetLed
// Description:	This method will draw the LED to the specified DC.
//
// Entry:
//				CDC *pDC - DC to draw to
//
//				int iLedColor - Where color is defined by:
//			 		LED_COLOR_RED
//					LED_COLOR_GREEN
//					LED_COLOR_YELLOW
//					LED_COLOR_BLUE
//
//				int iMode - where mode is defined by:
//					LED_ON
//					LED_OFF
//					LED_DISABLED
//
//				int iShape - where shape is defined by:
//					LED_ROUND
//					LED_SQUARE
///////////////////////////////////////////////////////////////////////////////
void CLed::DrawLed(CDC *pDC,int nLEDColor, int nMode, int nShape)
{
	CRect rect;
	GetClientRect(&rect);

	//
	// Center led within an oversized window
	//
	if(rect.Width() >= LED_SIZE && rect.Height() >= LED_SIZE)
	{
		int nWidth = rect.Width();
		int nHeight = rect.Height();
		rect.left += (nWidth - LED_SIZE)/2;
		rect.right -= (nWidth - LED_SIZE)/2;
		rect.top += (nHeight - LED_SIZE)/2;
		rect.bottom -= (nHeight - LED_SIZE)/2;
	}

	//
	// Prepare temporary DCs and bitmaps
	//
	CBitmap TransBitmap;
	TransBitmap.CreateBitmap(LED_SIZE,LED_SIZE,1,1,NULL);
	CBitmap bitmapTemp;
	CBitmap* pBitmap = &m_LedBitmap;
	CDC srcDC;
	CDC dcMask;
	CDC TempDC;
	TempDC.CreateCompatibleDC(pDC);
	srcDC.CreateCompatibleDC(pDC);
	dcMask.CreateCompatibleDC(pDC);

	CBitmap* pOldBitmap = srcDC.SelectObject(pBitmap);
	CBitmap* pOldMaskbitmap = dcMask.SelectObject(&TransBitmap);
	bitmapTemp.CreateCompatibleBitmap(pDC,LED_SIZE,LED_SIZE);

	//
	// Work with tempDC and bitmapTemp to reduce flickering
	//
	CBitmap *pOldBitmapTemp = TempDC.SelectObject(&bitmapTemp);
	TempDC.BitBlt(0, 0, LED_SIZE, LED_SIZE, pDC, rect.left, rect.top, SRCCOPY); 

	//
	// Create mask
	//
	COLORREF OldBkColor = srcDC.SetBkColor(RGB(255,0,255));
	dcMask.BitBlt(0, 0, LED_SIZE, LED_SIZE,&srcDC, nMode*LED_SIZE, nLEDColor+nShape, SRCCOPY); 
	TempDC.SetBkColor(OldBkColor);

	//
	// Using the IDB_LEDS bitmap, index into the bitmap for the appropriate
	// LED. By using the mask color (RGB(255,0,255)) a mask has been created
	// so the bitmap will appear transparent.
	//
	TempDC.BitBlt(0, 0, LED_SIZE, LED_SIZE, &srcDC, nMode*LED_SIZE, nLEDColor+nShape, SRCINVERT); 
	TempDC.BitBlt(0, 0, LED_SIZE, LED_SIZE,&dcMask, 0, 0, SRCAND); 
	TempDC.BitBlt(0, 0, LED_SIZE, LED_SIZE, &srcDC, nMode*LED_SIZE, nLEDColor+nShape, SRCINVERT); 

	//
	// Since the actual minipulation is done to tempDC so there is minimal
	// flicker, it is now time to draw the result to the screen.
	//
	pDC->BitBlt(rect.left, rect.top, LED_SIZE, LED_SIZE, &TempDC, 0, 0, SRCCOPY); 
	
	//
	// House cleaning
	//
	srcDC.SelectObject(pOldBitmap);
	dcMask.SelectObject(pOldMaskbitmap);
	TempDC.SelectObject(pOldBitmapTemp);
	VERIFY(srcDC.DeleteDC());
	VERIFY(dcMask.DeleteDC());
	VERIFY(TempDC.DeleteDC());
	VERIFY(TransBitmap.DeleteObject());
	VERIFY(bitmapTemp.DeleteObject());
}

///////////////////////////////////////////////////////////////////////////////
// Name:		SetLed
// Description:	This method will draw and set led parameters.
//
// Entry:		int iLedColor - Where color is defined by:
//			 		LED_COLOR_RED
//					LED_COLOR_GREEN
//					LED_COLOR_YELLOW
//					LED_COLOR_BLUE
//
//				int iMode - where mode is defined by:
//					LED_ON
//					LED_OFF
//					LED_DISABLED
//
//				int iShape - where shape is defined by:
//					LED_ROUND
//					LED_SQUARE
///////////////////////////////////////////////////////////////////////////////
void CLed::SetLed(int nLedColor, int nMode, int nShape)
{
	m_nLedColor = nLedColor;
	m_nLedMode = nMode;
	m_nLedShape = nShape;

	CDC *pDC;
	pDC = GetDC();
	DrawLed(pDC,m_nLedColor,m_nLedMode,m_nLedShape);
	ReleaseDC(pDC);
}

///////////////////////////////////////////////////////////////////////////////
// Name:		Ping
// Description:	This method will turn the led on for dwTimeout milliseconds and
//				then turn it off.
//
// Entry:		DWORD dwTimeout - Time out in  milliseconds
///////////////////////////////////////////////////////////////////////////////
void CLed::Ping(DWORD dwTimeout)
{
	// Return if pinging
	if(m_bPingEnabled == TRUE)
	{
		KillTimer(TIMER_ID_PING);
	}

	m_bPingEnabled = TRUE;
	SetLed(m_nLedColor,CLed::LED_ON,m_nLedShape);
	SetTimer(TIMER_ID_PING,dwTimeout,NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void CLed::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == TIMER_ID_PING)
	{
		SetLed(m_nLedColor,CLed::LED_OFF,m_nLedShape);
		KillTimer(nIDEvent);
		m_bPingEnabled = FALSE;
	}
	
	CStatic::OnTimer(nIDEvent);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
BOOL CLed::OnEraseBkgnd(CDC* pDC) 
{
	// No background rendering
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
