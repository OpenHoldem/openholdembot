///////////////////////////////////////////////////////////////////////////////
// Led.h : header file
// Visual Source Safe: $Revision: 1 $
//
// Led static control. Will display a LED in 4 different colors and two shapes.
///////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LEDWND_H__2D837381_FFEC_11D1_A1CE_00A024D311C0__INCLUDED_)
#define AFX_LEDWND_H__2D837381_FFEC_11D1_A1CE_00A024D311C0__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
///////////////////////////////////////////////////////////////////////////////

#define LED_SIZE	16			// Led are 16 X 16 pixels

/////////////////////////////////////////////////////////////////////////////
// CLed window
class CLed : public CStatic
{

protected:
	int m_nLedColor, m_nLedMode, m_nLedShape;
	DWORD m_dwPingTimeout;
	BOOL m_bPingEnabled;
	CBitmap m_LedBitmap;

public:

	enum {
		LED_ROUND = 0,						// Circle starts at row 0
		LED_SQUARE = LED_SIZE * 4,			// squares start at row 4
	};
	enum {
		LED_COLOR_RED = 0 * LED_SIZE,		// Row 0
		LED_COLOR_GREEN = 1 * LED_SIZE,		// Row 1
		LED_COLOR_YELLOW = 2 * LED_SIZE,	// Row 2
		LED_COLOR_BLUE = 3 * LED_SIZE,		// Row 3
	};
	enum {
		LED_ON = 0,							// Column 0
		LED_OFF = 1,						// Column 1
		LED_DISABLED = 2,					// Column 2
	};

// Construction
public:
	CLed();

// Attributes
public:

// Operations
public:
	void SetLed(int nLedColor, int nMode, int nShape);
	int GetLedMode(){return m_nLedMode;}
	void Ping(DWORD dwTimeout = 1000);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLed)
	//}}AFX_VIRTUAL

// Implementation
public:
	void DrawLed( CDC *pDC, int nLEDColor, int nMode, int nShape );
	virtual ~CLed();

	// Generated message map functions
protected:
	//{{AFX_MSG(CLed)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LEDWND_H__2D837381_FFEC_11D1_A1CE_00A024D311C0__INCLUDED_)
