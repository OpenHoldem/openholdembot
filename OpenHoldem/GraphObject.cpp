
// GraphObject.cpp : implementation file
//

#include "stdafx.h"
#include "GraphObject.h"

#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGraphObject

CGraphObject::CGraphObject()
{
	// Set default graph title and subtitle
	m_GraphTitle = "Add graph title here";
	m_GraphSubtitle = "Add graph subtitle here";
	m_GraphAlertString = "", m_GraphAlertLine = "";
	m_GraphAlert = FALSE;

	// Set default graph type
	m_GraphType = -1;

	// Set default graph DC and bitmap
	m_GraphDC = NULL;
	m_GraphBitmap = NULL;
	// Set default gradient DC and bitmap
	m_GradientDC = NULL;
	m_GradientBitmap = NULL;

	// Set default graph animation
	m_GraphAnimation = FALSE;

	// Set default graph background fill and type
	m_GraphFillType = GB_SOLID;
	m_GraphGradientFillType = GRADIENT_FILL_RECT_V;

	// Set default graph gradient colors
	m_GraphGradientColor1 = RGB(0,0,64);
	m_GraphGradientColor2 = RGB(0,0,216);

	// Set default graph title and subtitle shadow
	m_GraphTitleShadow = FALSE;
	m_GraphSubtitleShadow = FALSE;

	// Set default graph title, subtitle and alert color
	m_GraphTitleColor = RGB(0,0,0);
	m_GraphSubtitleColor = RGB(0,0,0);
	m_GraphAlertColor = RGB(211, 211, 211);

	// Set default graph legend show
	m_ShowLegend = TRUE;
}

CGraphObject::~CGraphObject()
{
	switch ( m_GraphType )
	{
		// Delete 2D bar graph
		case GT_2DBAR:
			{
				delete m_2DBarGraph;
				m_2DBarGraph = NULL;
			}
			break;

		// Delete 2D pie graph
		case GT_2DPIE:
			{
				delete m_2DPieGraph;
				m_2DPieGraph = NULL;
			}
			break;

		// Delete 2D line graph
		case GT_2DLINE:
			{
				delete m_2DLineGraph;
				m_2DLineGraph = NULL;
			}
			break;

		// Delete default graph type (2D bar graph)
		default:
			break;
	}

	// Delete graph title, subtitle and alert font
	m_GraphTitleFont->DeleteObject();
	delete m_GraphTitleFont;
	m_GraphTitleFont = NULL;
	m_GraphSubtitleFont->DeleteObject();
	delete m_GraphSubtitleFont;
	m_GraphSubtitleFont = NULL;
	m_GraphAlertFont->DeleteObject();
	delete m_GraphAlertFont;
	m_GraphAlertFont = NULL;
	m_GraphAlertFontTrue->DeleteObject();
	delete m_GraphAlertFontTrue;
	m_GraphAlertFontTrue = NULL;
	m_GraphAlertLineFont->DeleteObject();
	delete m_GraphAlertLineFont;
	m_GraphAlertLineFont = NULL;

	// Delete graph DC and bitmap
	m_GraphDC->DeleteDC();
	delete m_GraphDC;
	m_GraphDC = NULL;
	m_GraphBitmap->DeleteObject();
	delete m_GraphBitmap;
	m_GraphBitmap = NULL;
	// Delete gradient DC and bitmap
	m_GradientDC->DeleteDC();
	delete m_GradientDC;
	m_GradientDC = NULL;
	m_GradientBitmap->DeleteObject();
	delete m_GradientBitmap;
	m_GradientBitmap = NULL;
}


BEGIN_MESSAGE_MAP(CGraphObject, CWnd)
	//{{AFX_MSG_MAP(CGraphObject)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CGraphObject::DoDataExchange(CDataExchange* pDX)
{
	//CWnd::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CControlContainerDlg)
	DDX_Control(pDX, IDC_LINK, m_Link);
	//}}AFX_DATA_MAP
	return DoDataExchange(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CGraphObject message handlers

BOOL CGraphObject::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	// Minimal graph size is (200x200)
	CRect rectGraph( rect );
	if ( ( rectGraph.right - rectGraph.left ) < 200 )
		rectGraph.right = rectGraph.left + 200;
	if ( ( rectGraph.bottom - rectGraph.top ) < 200 )
		rectGraph.bottom = rectGraph.top + 200;

	// Set graph size and position
	m_Position = CPoint( rectGraph.left, rectGraph.top );
	m_Size = CSize( rectGraph.right - rectGraph.left, rectGraph.bottom - rectGraph.top );

	// Set graph background color
	m_GraphBackgroundColor = RGB(255,255,255);

	m_Update = TRUE;

	// Set graph title font
	m_GraphTitleFont = new CFont();
	m_GraphTitleFont->CreateFont( int(m_Size.cy*0.2), int(m_Size.cx*0.04), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set graph subtitle font
	m_GraphSubtitleFont = new CFont();
	m_GraphSubtitleFont->CreateFont( int(m_Size.cy*0.07), int(m_Size.cx*0.03), 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set graph alert font
	m_GraphAlertFont = new CFont();
	m_GraphAlertFont->CreateFont(int(m_Size.cy*0.08), int(m_Size.cx*0.04), 0, 0, FW_NORMAL, FALSE, FALSE, TRUE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set graph alert font true
	m_GraphAlertFontTrue = new CFont();
	m_GraphAlertFontTrue->CreateFont(int(m_Size.cy*0.08), int(m_Size.cx*0.04), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set graph alert line font
	m_GraphAlertLineFont = new CFont();
	m_GraphAlertLineFont->CreateFont(int(m_Size.cy*0.065), int(m_Size.cx*0.025), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	if (init)
	{
		mLinkRect = CRect(int(m_Size.cx*0.95), int(m_Size.cy*0.75), int(m_Size.cx*0.95), int(m_Size.cy*0.75) + 20);
		m_Link.Create("GO >>", WS_CHILD | WS_OVERLAPPED | WS_CLIPSIBLINGS | WS_VISIBLE,
			mLinkRect, pParentWnd);
		m_Link.SetFire(TRUE); 
		init = FALSE;
	}
  
	return CWnd::Create(NULL, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS, rectGraph, pParentWnd, nID, NULL);
}

void CGraphObject::CreateGraph(int g_type)
{
	// Set graph type
	m_GraphType = g_type;

	switch ( m_GraphType )
	{
		// Create 2D bar graph
		case GT_2DBAR:
			{
				// Set graph size and position
				CPoint g_point = CPoint( int(m_Size.cx*0.05), int(m_Size.cy*0.35) );
				CSize g_size = CSize( int(m_Size.cx*0.7), int(m_Size.cy*0.5) );
				m_2DBarGraph = new C2DBarGraph( g_point, g_size );
				m_2DBarGraph->SetFullSize( m_Size );
			}
			break;

		// Create 2D pie graph
		case GT_2DPIE:
			{
				// Set graph size and position
				CPoint g_point = CPoint( int(m_Size.cx*0.05), int(m_Size.cy*0.35) );
				CSize g_size = CSize( int(m_Size.cx*0.5), int(m_Size.cy*0.5) );
				m_2DPieGraph = new C2DPieGraph( g_point, g_size );
				m_2DPieGraph->SetFullSize( m_Size );
			}
			break;

		// Create 2D line graph
		case GT_2DLINE:
			{
				// Set graph size and position
				CPoint g_point = CPoint( int(m_Size.cx*0.05), int(m_Size.cy*0.35) );
				CSize g_size = CSize( int(m_Size.cx*0.7), int(m_Size.cy*0.5) );
				m_2DLineGraph = new C2DLineGraph( g_point, g_size );
				m_2DLineGraph->SetFullSize( m_Size );
			}
			break;

		// Create default graph type (2D bar graph)
		default:
			break;
	}

	// Get DC
	CDC* pDC = GetDC();

	// Create graph DC and bitmap
	m_GraphDC = new CDC();
	m_GraphDC->CreateCompatibleDC( pDC );
	m_GraphBitmap = new CBitmap();
	m_GraphBitmap->CreateCompatibleBitmap( pDC, m_Size.cx, m_Size.cy );
	m_GraphDC->SelectObject( m_GraphBitmap );
	// Create gradient DC and bitmap
	m_GradientDC = new CDC();
	m_GradientDC->CreateCompatibleDC( pDC );
	m_GradientBitmap = new CBitmap();
	m_GradientBitmap->CreateCompatibleBitmap( pDC, m_Size.cx, m_Size.cy );
	m_GradientDC->SelectObject( m_GradientBitmap );

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1)<<8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1)<<8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1)<<8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2)<<8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2)<<8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2)<<8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill( m_GradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType );

	// Release DC
	CWnd::ReleaseDC( pDC );
}

void CGraphObject::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here

	// Get graph window DC
	CDC* pDC = GetDC();

	CRect gRect;
	// Get graph type
	switch ( m_GraphType )
	{
		// Set 2D bar graph clear rect
		case GT_2DBAR:
				gRect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.6), int(m_Size.cy*0.35) + int(m_Size.cy*0.6) );
			break;

		// Set 2D pie graph clear rect
		case GT_2DPIE:
				gRect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.5), int(m_Size.cy*0.35) + int(m_Size.cy*0.5) );
			break;

		// Set 2D line graph clear rect
		case GT_2DLINE:
			gRect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.6), int(m_Size.cy*0.35) + int(m_Size.cy*0.6) );
			break;

		// Set default graph type (2D bar graph) clear rect
		default:
			break;
	}
	// Clear graph memory DC
	if ( m_GraphFillType == GB_SOLID )
	{
		// Draw graph solid background
		CBrush bgBrush( m_GraphBackgroundColor );
		::FillRect( m_GraphDC->m_hDC, gRect, (HBRUSH)bgBrush.GetSafeHandle() );
		bgBrush.DeleteObject();
	}
	else
	{
		// Draw graph gradient background
		::BitBlt( m_GraphDC->m_hDC, 0, 0, m_Size.cx, m_Size.cy, m_GradientDC->m_hDC, 0, 0, SRCCOPY );
	}

	// Get graph type
	switch ( m_GraphType )
	{
		// Build 2D bar graph
		case GT_2DBAR:
			m_2DBarGraph->BuildGraph( m_GraphDC->m_hDC );
			break;

		// Build 2D pie graph
		case GT_2DPIE:
			m_2DPieGraph->BuildGraph( m_GraphDC->m_hDC );
			break;

		// Build 2D line graph
		case GT_2DLINE:
			m_2DLineGraph->BuildGraph( m_GraphDC->m_hDC );
			break;

		// Build default graph type (2D bar graph)
		default:
			break;
	}

	// Check graph background fill type
	if ( m_GraphFillType == GB_GRADIENT )
	{
		// Draw graph title
		CFont* pOldFont = m_GraphDC->SelectObject( m_GraphTitleFont );
		COLORREF oldTextColor;
		m_GraphDC->SetBkMode( TRANSPARENT );
		if ( m_GraphTitleShadow == TRUE )
		{
			oldTextColor = m_GraphDC->SetTextColor( RGB(128,128,128) );
			m_GraphDC->TextOut( int(m_Size.cx*0.065), int(m_Size.cy*0.037), m_GraphTitle, m_GraphTitle.GetLength() );
			m_GraphDC->SetTextColor( oldTextColor );
		}
		oldTextColor = m_GraphDC->SetTextColor( m_GraphTitleColor );
		m_GraphDC->TextOut( int(m_Size.cx*0.05), int(m_Size.cy*0.025), m_GraphTitle, m_GraphTitle.GetLength() );
		m_GraphDC->SetBkMode( OPAQUE );
		m_GraphDC->SelectObject( pOldFont );
		m_GraphDC->SetTextColor( oldTextColor );

		// Draw graph subtitle
		pOldFont = m_GraphDC->SelectObject( m_GraphSubtitleFont );
		m_GraphDC->SetBkMode( TRANSPARENT );
		if ( m_GraphSubtitleShadow == TRUE )
		{
			oldTextColor = m_GraphDC->SetTextColor( RGB(128,128,128) );
			m_GraphDC->TextOut( int(m_Size.cx*0.06), int(m_Size.cy*0.208), m_GraphSubtitle, m_GraphSubtitle.GetLength() );
			m_GraphDC->SetTextColor( oldTextColor );
		}
		oldTextColor = m_GraphDC->SetTextColor( m_GraphSubtitleColor );
		m_GraphDC->TextOut( int(m_Size.cx*0.05), int(m_Size.cy*0.2), m_GraphSubtitle, m_GraphSubtitle.GetLength() );
		m_GraphDC->SetBkMode( OPAQUE );
		m_GraphDC->SelectObject( pOldFont );
		m_GraphDC->SetTextColor( oldTextColor );

		// Draw graph alert
		COLORREF oldBkColor = m_GraphDC->SetBkColor(m_GraphBackgroundColor);
		oldBkColor = m_GraphDC->SetBkColor(m_GraphBackgroundColor);
		oldTextColor = m_GraphDC->SetTextColor(m_GraphAlertColor);
		pOldFont = m_GraphDC->SelectObject(m_GraphAlertFont);
		m_GraphDC->SetBkMode(TRANSPARENT);
		if (m_GraphAlert == TRUE)
		{
			oldTextColor = m_GraphDC->SetTextColor(RGB(255, 0, 0));
			pOldFont = m_GraphDC->SelectObject(m_GraphAlertFontTrue);
			m_GraphAlert = FALSE;
		}
		if (m_GraphAlertString.GetLength() > 10)
			m_GraphDC->TextOut(int(m_Size.cx*0.42), int(m_Size.cy*0.28), m_GraphAlertString, m_GraphAlertString.GetLength());
		else
			m_GraphDC->TextOut(int(m_Size.cx*0.51), int(m_Size.cy*0.28), m_GraphAlertString, m_GraphAlertString.GetLength());
		m_GraphDC->SetBkMode(OPAQUE);
		m_GraphDC->SelectObject(pOldFont);
		m_GraphDC->SetBkColor(oldBkColor);
		m_GraphDC->SetTextColor(oldTextColor);

		// For 2nd version of SetGraphAlert() otherwise comment it
		oldBkColor = m_GraphDC->SetBkColor(m_GraphBackgroundColor);
		oldTextColor = m_GraphDC->SetTextColor(m_GraphSubtitleColor);
		pOldFont = m_GraphDC->SelectObject(m_GraphAlertLineFont);
		m_GraphDC->SetBkMode(TRANSPARENT);
		m_GraphDC->TextOut(int(m_Size.cx*0.51), int(m_Size.cy*0.38), m_GraphAlertLine, m_GraphAlertLine.GetLength());
		m_GraphDC->SetBkMode(OPAQUE);
		m_GraphDC->SelectObject(pOldFont);
		m_GraphDC->SetBkColor(oldBkColor);
		m_GraphDC->SetTextColor(oldTextColor);

		WINDOWPLACEMENT wndpl;
		m_Link.GetWindowPlacement(&wndpl);
		wndpl.rcNormalPosition.left = mLinkRect.left + m_GraphAlertLine.GetLength() * 3.5;
		wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + 40;
		m_Link.SetWindowPlacement(&wndpl);
		m_Link.SetLinkText(m_GraphAlertLine, FALSE);

		// Check legend flag
		if ( m_ShowLegend == TRUE )
		{
			// Get graph type
			switch ( m_GraphType )
			{
				// Draw 2D bar graph legend
				case GT_2DBAR:
					m_2DBarGraph->BuildGraphLegend( m_GraphDC->m_hDC );
					break;

				// Draw 2D pie graph legend
				case GT_2DPIE:
					m_2DPieGraph->BuildGraphLegend( m_GraphDC->m_hDC );
					break;

				// Draw 2D line graph legend
				case GT_2DLINE:
					m_2DLineGraph->BuildGraphLegend( m_GraphDC->m_hDC );
					break;

				// Build default graph type (2D bar graph) legend
				default:
					break;
			}
		}
	}
	else
	{
		// Check update flag
		if ( m_Update == TRUE )
		{
			// Draw graph title
			COLORREF oldBkColor = m_GraphDC->SetBkColor( m_GraphBackgroundColor );
			CFont* pOldFont = m_GraphDC->SelectObject( m_GraphTitleFont );
			COLORREF oldTextColor;
			m_GraphDC->SetBkMode( TRANSPARENT );
			if ( m_GraphTitleShadow == TRUE )
			{
				oldTextColor = m_GraphDC->SetTextColor( RGB(128,128,128) );
				m_GraphDC->TextOut( int(m_Size.cx*0.065), int(m_Size.cy*0.037), m_GraphTitle, m_GraphTitle.GetLength() );
				m_GraphDC->SetTextColor( oldTextColor );
			}
			oldTextColor = m_GraphDC->SetTextColor( m_GraphTitleColor );
			m_GraphDC->TextOut( int(m_Size.cx*0.05), int(m_Size.cy*0.025), m_GraphTitle, m_GraphTitle.GetLength() );
			m_GraphDC->SetBkMode( OPAQUE );
			m_GraphDC->SelectObject( pOldFont );
			m_GraphDC->SetBkColor( oldBkColor );
			m_GraphDC->SetTextColor( oldTextColor );

			// Draw graph subtitle
			oldBkColor = m_GraphDC->SetBkColor( m_GraphBackgroundColor );
			pOldFont = m_GraphDC->SelectObject( m_GraphSubtitleFont );
			m_GraphDC->SetBkMode( TRANSPARENT );
			if ( m_GraphSubtitleShadow == TRUE )
			{
				oldTextColor = m_GraphDC->SetTextColor( RGB(128,128,128) );
				m_GraphDC->TextOut( int(m_Size.cx*0.06), int(m_Size.cy*0.208), m_GraphSubtitle, m_GraphSubtitle.GetLength() );
				m_GraphDC->SetTextColor( oldTextColor );
			}
			oldTextColor = m_GraphDC->SetTextColor( m_GraphSubtitleColor );
			m_GraphDC->TextOut( int(m_Size.cx*0.05), int(m_Size.cy*0.2), m_GraphSubtitle, m_GraphSubtitle.GetLength() );
			m_GraphDC->SetBkMode( OPAQUE );
			m_GraphDC->SelectObject( pOldFont );
			m_GraphDC->SetBkColor( oldBkColor );
			m_GraphDC->SetTextColor( oldTextColor );

			// Draw graph alert
			oldBkColor = m_GraphDC->SetBkColor(m_GraphBackgroundColor);
			oldTextColor = m_GraphDC->SetTextColor(m_GraphAlertColor);
			pOldFont = m_GraphDC->SelectObject(m_GraphAlertFont);
			m_GraphDC->SetBkMode(TRANSPARENT);
			if (m_GraphAlert == TRUE)
			{
				oldTextColor = m_GraphDC->SetTextColor(RGB(255, 0, 0));
				pOldFont = m_GraphDC->SelectObject(m_GraphAlertFontTrue);
				m_GraphAlert = FALSE;
			}
			if (m_GraphAlertString.GetLength()>10)
				m_GraphDC->TextOut(int(m_Size.cx*0.42), int(m_Size.cy*0.28), m_GraphAlertString, m_GraphAlertString.GetLength());
			else
				m_GraphDC->TextOut(int(m_Size.cx*0.51), int(m_Size.cy*0.28), m_GraphAlertString, m_GraphAlertString.GetLength());
			m_GraphDC->SetBkMode(OPAQUE);
			m_GraphDC->SelectObject(pOldFont);
			m_GraphDC->SetBkColor(oldBkColor);
			m_GraphDC->SetTextColor(oldTextColor);
			
			// For 2nd version of SetGraphAlert() otherwise comment it
			oldBkColor = m_GraphDC->SetBkColor(m_GraphBackgroundColor);
			oldTextColor = m_GraphDC->SetTextColor(m_GraphSubtitleColor);
			pOldFont = m_GraphDC->SelectObject(m_GraphAlertLineFont);
			m_GraphDC->SetBkMode(TRANSPARENT);
			m_GraphDC->TextOut(int(m_Size.cx*0.51), int(m_Size.cy*0.38), m_GraphAlertLine, m_GraphAlertLine.GetLength());
			m_GraphDC->SetBkMode(OPAQUE);
			m_GraphDC->SelectObject(pOldFont);
			m_GraphDC->SetBkColor(oldBkColor);
			m_GraphDC->SetTextColor(oldTextColor);

			WINDOWPLACEMENT wndpl;
			m_Link.GetWindowPlacement(&wndpl);
			wndpl.rcNormalPosition.left = mLinkRect.left + m_GraphAlertLine.GetLength() * 3;
			wndpl.rcNormalPosition.right = wndpl.rcNormalPosition.left + 50;
			m_Link.SetWindowPlacement(&wndpl);
			m_Link.SetLinkText(m_GraphAlertLine, FALSE);

			// Check legend flag
			if ( m_ShowLegend == TRUE )
			{
				// Get graph type
				switch ( m_GraphType )
				{
					// Draw 2D bar graph legend
					case GT_2DBAR:
						m_2DBarGraph->BuildGraphLegend( m_GraphDC->m_hDC );
						break;

					// Draw 2D pie graph legend
					case GT_2DPIE:
						m_2DPieGraph->BuildGraphLegend( m_GraphDC->m_hDC );
						break;

					// Draw 2D line graph legend
					case GT_2DLINE:
						m_2DLineGraph->BuildGraphLegend( m_GraphDC->m_hDC );
						break;

					// Build default graph type (2D bar graph) legend
					default:
						break;
				}
			}

			// Clear update flag
			m_Update = FALSE;
		}
	}

	// Draw graph DC
	CRect rect;
	GetClientRect( &rect );
	pDC->BitBlt( 0, 0, rect.Width(), rect.Height(), m_GraphDC, 0, 0, SRCCOPY );

	// Relese graph window DC
	CWnd::ReleaseDC( pDC );
	
	// Do not call CWnd::OnPaint() for painting messages
}

void CGraphObject::Add2DPieGraphSegment(double s_percent, COLORREF s_color, CString s_text)
{
	// Add new 2D pie graph segment
	m_2DPieGraph->AddSegment( s_percent, s_color, s_text );
}


void CGraphObject::Delete2DPieGraphSegment( int s_index )
{
	// Delete 2D pie graph segment
	m_2DPieGraph->DeleteSegment( s_index );
}

void CGraphObject::SetGraphAnimation(BOOL g_animation, int a_type)
{
	// Set graph animation
	switch ( m_GraphType )
	{
		// Set 2D bar graph animation
		case GT_2DBAR:
			{
				m_2DBarGraph->SetGraphAnimation( g_animation, a_type );
				m_2DBarGraph->UpdateAnimation();
			}
			break;

		// Set 2D pie graph animation
		case GT_2DPIE:
			{
				m_2DPieGraph->SetGraphAnimation( g_animation, a_type );
				m_2DPieGraph->UpdateAnimation();
			}
			break;

		// Set 2D line graph animation
		case GT_2DLINE:
			{
				m_2DLineGraph->SetGraphAnimation( g_animation, a_type );
				m_2DLineGraph->UpdateAnimation();
			}
			break;

		// Set default graph type (2D bar graph) animation
		default:
			break;
	}

	// Set graph animation flag
	m_GraphAnimation = g_animation;

	// Check animation flag
	if ( m_GraphAnimation == TRUE )
	{
		// Set graph animation timer
		SetTimer( IDT_ANIMATION, ANIMATION_TIME, NULL );
	}
	else
	{
		// Kill graph animation timer
		KillTimer( IDT_ANIMATION );
	}
}

BOOL CGraphObject::GetGraphAnimation()
{
	return m_GraphAnimation;
}

void CGraphObject::SetGraphBackgroundColor(COLORREF g_bgcolor)
{
	// Set graph background color
	m_GraphBackgroundColor = g_bgcolor;

	// Get graph type
	switch ( m_GraphType )
	{
		// Set 2D bar graph background color
		case GT_2DBAR:
			m_2DBarGraph->SetGraphBkColor( g_bgcolor );
			break;

		// Set 2D pie graph background color
		case GT_2DPIE:
			m_2DPieGraph->SetGraphBkColor( g_bgcolor );
			break;

		// Set 2D line graph background color
		case GT_2DLINE:
			m_2DLineGraph->SetGraphBkColor( g_bgcolor );
			break;

		// Set default graph type (2D bar graph) background color
		default:
			break;
	}

	Invalidate( FALSE );
}

COLORREF CGraphObject::GetGraphBackgroundColor()
{
	return m_GraphBackgroundColor;
}

BOOL CGraphObject::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default

	m_Update = TRUE;

	CBrush bgBrush( m_GraphBackgroundColor );
	CBrush* pOldBrush = pDC->SelectObject( &bgBrush );
	m_GraphDC->SelectObject( &bgBrush );

	CRect rect;
	pDC->GetClipBox( &rect );
	pDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY );
	m_GraphDC->GetClipBox( &rect );
	m_GraphDC->PatBlt( rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY );
	pDC->SelectObject( pOldBrush );
	m_GraphDC->SelectObject( pOldBrush );
	bgBrush.DeleteObject();
	return TRUE;
}

void CGraphObject::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default

	int result;

	// Get graph type
	switch ( m_GraphType )
	{
		// Refresh 2D bar graph
		case GT_2DBAR:
			{
				m_2DBarGraph->UpdateAnimation();
				result = m_2DBarGraph->GetGraphAnimationPercent();
				if ( result >= 100 )
				{
					CBrush bgBrush( m_GraphBackgroundColor );
					CRect rect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.5), int(m_Size.cy*0.35) + int(m_Size.cy*0.5) );
					::FillRect( m_GraphDC->m_hDC, rect, (HBRUSH)bgBrush.GetSafeHandle() );
					bgBrush.DeleteObject();
				}
			}
			break;

		// Refresh 2D pie graph
		case GT_2DPIE:
			{
				m_2DPieGraph->UpdateAnimation();
				result = m_2DPieGraph->GetGraphAnimationPercent();
				if ( result >= 100 )
				{
					CBrush bgBrush( m_GraphBackgroundColor );
					CRect rect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.5), int(m_Size.cy*0.35) + int(m_Size.cy*0.5) );
					::FillRect( m_GraphDC->m_hDC, rect, (HBRUSH)bgBrush.GetSafeHandle() );
					bgBrush.DeleteObject();
				}
			}
			break;

		// Refresh 2D line graph
		case GT_2DLINE:
			{
				m_2DLineGraph->UpdateAnimation();
				result = m_2DLineGraph->GetGraphAnimationPercent();
				if ( result >= 100 )
				{
					CBrush bgBrush( m_GraphBackgroundColor );
					CRect rect = CRect( int(m_Size.cx*0.05), int(m_Size.cy*0.35), int(m_Size.cx*0.05) + int(m_Size.cx*0.5), int(m_Size.cy*0.35) + int(m_Size.cy*0.5) );
					::FillRect( m_GraphDC->m_hDC, rect, (HBRUSH)bgBrush.GetSafeHandle() );
					bgBrush.DeleteObject();
				}
			}
			break;

		// Refresh default graph type (2D bar graph)
		default:
			break;
	}

	// Clear update flag
	m_Update = FALSE;

	Invalidate( FALSE );
	
	CWnd::OnTimer(nIDEvent);
}

void CGraphObject::SetGraphTitle(CString g_title)
{
	// Set graph title
	m_GraphTitle = g_title;

	// Set update flag
	m_Update = TRUE;

	Invalidate( TRUE );
}

void CGraphObject::SetGraphSubtitle(CString g_subtitle)
{
	// Set graph subtitle
	m_GraphSubtitle = g_subtitle;

	// Set update flag
	m_Update = TRUE;

	Invalidate( TRUE );
}

void CGraphObject::SetGraphAlert(BOOL g_alert)
{
	// Set graph alert
	m_GraphAlert = g_alert;
	//m_GraphAlertString = "MUST PLAY !";
	//m_GraphAlertString = "PUSH !";

	// Set update flag
	m_Update = TRUE;

	Invalidate( TRUE );
}

void CGraphObject::SetGraphAlert(CString g_action_string, CString g_action_line)
{
	// Set graph alert
	m_GraphAlert = TRUE;
	m_GraphAlertString = g_action_string;
	m_GraphAlertLine = g_action_line;

	// Set update flag
	m_Update = TRUE;

	Invalidate(TRUE);
}

void CGraphObject::Update2DPieGraphSegment(int s_index, double s_percent, COLORREF s_color, CString s_text)
{
	// Update 2D pie graph segment
	m_2DPieGraph->UpdateSegment( s_index, s_percent, s_color, s_text );
}

void CGraphObject::SetGraphSize(CSize g_size)
{
	if ( m_GraphAnimation == TRUE )
	{
		// Kill graph animation timer
		KillTimer( IDT_ANIMATION );
	}

	// Set graph size
	m_Size = g_size;

	// Check graph size
	if ( m_Size.cx < 200 )
		m_Size.cx = 200;
	if ( m_Size.cy < 200 )
		m_Size.cy = 200;

	// Set graph position
	CPoint g_position = CPoint( int(m_Size.cx*0.05), int(m_Size.cy*0.35) );

	// Get graph type
	switch ( m_GraphType )
	{
		// Set new 2D bar graph size and position
		case GT_2DBAR:
			{
				m_2DBarGraph->SetGraphSize( CSize(int(m_Size.cx*0.7),int(m_Size.cy*0.5)) );
				m_2DBarGraph->SetGraphPosition( g_position );
				m_2DBarGraph->SetFullSize( m_Size );
			}
			break;

		// Set new 2D pie graph size and position
		case GT_2DPIE:
			{
				m_2DPieGraph->SetGraphSize( CSize(int(m_Size.cx*0.5),int(m_Size.cy*0.5)) );
				m_2DPieGraph->SetGraphPosition( g_position );
				m_2DPieGraph->SetFullSize( m_Size );
			}
			break;

		// Set new 2D line graph size and position
		case GT_2DLINE:
			{
				m_2DLineGraph->SetGraphSize( CSize(int(m_Size.cx*0.7),int(m_Size.cy*0.5)) );
				m_2DLineGraph->SetGraphPosition( g_position );
				m_2DLineGraph->SetFullSize( m_Size );
			}
			break;

		// Set default graph type (2D bar graph) size and position
		default:
			break;
	}

	// Set graph title font
	m_GraphTitleFont->DeleteObject();
	delete m_GraphTitleFont;
	m_GraphTitleFont = new CFont();
	m_GraphTitleFont->CreateFont( int(m_Size.cy*0.2), int(m_Size.cx*0.04), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set graph subtitle font
	m_GraphSubtitleFont->DeleteObject();
	delete m_GraphSubtitleFont;
	m_GraphSubtitleFont = new CFont();
	m_GraphSubtitleFont->CreateFont( int(m_Size.cy*0.07), int(m_Size.cx*0.03), 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set graph alert font
	m_GraphAlertFont->DeleteObject();
	delete m_GraphAlertFont;
	m_GraphAlertFont = new CFont();
	m_GraphAlertFont->CreateFont(int(m_Size.cy*0.08), int(m_Size.cx*0.04), 0, 0, FW_NORMAL, FALSE, FALSE, TRUE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set graph alert font true
	m_GraphAlertFontTrue->DeleteObject();
	delete m_GraphAlertFontTrue;
	m_GraphAlertFontTrue = new CFont();
	// For 1st version of SetGraphAlert() otherwise comment it
	//m_GraphAlertFontTrue->CreateFont(int(m_Size.cy*0.08), int(m_Size.cx*0.04), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
	//	DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");
	// For 2nd version of SetGraphAlert() otherwise comment it
	m_GraphAlertFontTrue->CreateFont(int(m_Size.cy*0.07), int(m_Size.cx*0.03), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set graph alert line font
	m_GraphAlertLineFont->DeleteObject();
	delete m_GraphAlertLineFont;
	m_GraphAlertLineFont = new CFont();
	m_GraphAlertLineFont->CreateFont(int(m_Size.cy*0.065), int(m_Size.cx*0.025), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set new graph window size
	::SetWindowPos( this->m_hWnd, HWND_NOTOPMOST, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, SWP_NOZORDER );
	
	// Set update flag
	m_Update = TRUE;

	if ( m_GraphAnimation == TRUE )
	{
		// Set new graph animation timer
		SetTimer( IDT_ANIMATION, ANIMATION_TIME, NULL );
	}

	// Delete graph DC and bitmap
	m_GraphDC->DeleteDC();
	delete m_GraphDC;
	m_GraphDC = NULL;
	m_GraphBitmap->DeleteObject();
	delete m_GraphBitmap;
	m_GraphBitmap = NULL;
	// Delete gradient DC and bitmap
	m_GradientDC->DeleteDC();
	delete m_GradientDC;
	m_GradientDC = NULL;
	m_GradientBitmap->DeleteObject();
	delete m_GradientBitmap;
	m_GradientBitmap = NULL;

	// Get DC
	CDC* pDC = GetDC();

	// Create graph DC and bitmap
	m_GraphDC = new CDC();
	m_GraphDC->CreateCompatibleDC( pDC );
	m_GraphBitmap = new CBitmap();
	m_GraphBitmap->CreateCompatibleBitmap( pDC, m_Size.cx, m_Size.cy );
	m_GraphDC->SelectObject( m_GraphBitmap );
	// Create gradient DC and bitmap
	m_GradientDC = new CDC();
	m_GradientDC->CreateCompatibleDC( pDC );
	m_GradientBitmap = new CBitmap();
	m_GradientBitmap->CreateCompatibleBitmap( pDC, m_Size.cx, m_Size.cy );
	m_GradientDC->SelectObject( m_GradientBitmap );

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1)<<8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1)<<8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1)<<8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2)<<8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2)<<8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2)<<8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill( m_GradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType );

	// Release DC
	CWnd::ReleaseDC( pDC );

	Invalidate( FALSE );
}

void CGraphObject::SetGraphPosition(CPoint g_position)
{
	if ( m_GraphAnimation == TRUE )
	{
		// Kill graph animation timer
		KillTimer( IDT_ANIMATION );
	}

	// Set new graph position
	m_Position = g_position;

	// Set new graph window position
	::SetWindowPos( this->m_hWnd, HWND_NOTOPMOST, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, SWP_NOZORDER );

	// Set update flag
	m_Update = TRUE;

	if ( m_GraphAnimation == TRUE )
	{
		// Set new graph animation timer
		SetTimer( IDT_ANIMATION, ANIMATION_TIME, NULL );
	}

	Invalidate( TRUE );
}

void CGraphObject::Add2DBarGraphSegment(CString s_text)
{
	// Add new 2D bar graph segment
	m_2DBarGraph->AddSegment( s_text );
}

void CGraphObject::Delete2DBarGraphSegment(int s_index)
{
	// Delete 2D bar graph segment
	m_2DBarGraph->DeleteSegment( s_index );
}

void CGraphObject::Set2DBarGraphValue(int s_segment, int s_series, double s_value)
{
	// Set 2D bar graph value
	m_2DBarGraph->SetValue( s_segment, s_series, s_value );
}

void CGraphObject::Add2DBarGraphSeries(CString s_text, COLORREF s_color)
{
	// Add new 2D bar graph serie
	m_2DBarGraph->AddSeries( s_text, s_color );
}

void CGraphObject::Detele2DBarGraphSeries(int s_index)
{
	// Delete 2D bar graph serie
	m_2DBarGraph->DeleteSeries( s_index );
}

void CGraphObject::Add2DLineGraphSegment(CString s_text)
{
	// Add new 2D line graph segment
	m_2DLineGraph->AddSegment( s_text );
}

void CGraphObject::Delete2DLineGraphSegment(int s_index)
{
	// Delete 2D line graph segment
	m_2DLineGraph->DeleteSegment( s_index );
}

void CGraphObject::Set2DLineGraphValue(int s_segment, int s_series, int s_value)
{
	// Set 2D line graph value
	m_2DLineGraph->SetValue( s_segment, s_series, s_value );
}

void CGraphObject::Add2DLineGraphSeries(CString s_text, COLORREF s_color)
{
	// Add new 2D line graph serie
	m_2DLineGraph->AddSeries( s_text, s_color );
}

void CGraphObject::Delete2DLineGraphSeries(int s_index)
{
	// Delete 2D line graph serie
	m_2DLineGraph->DeleteSeries( s_index );
}

void CGraphObject::SetGraphFillType(int g_fill)
{
	// Set graph background fill type
	switch( g_fill )
	{
		// Set solid graph background fill
		case GB_SOLID:
			m_GraphFillType = GB_SOLID;
			break;

		// Set gradient graph background fill
		case GB_GRADIENT:
			m_GraphFillType = GB_GRADIENT;
			break;

		// Set default graph background fill (GB_SOLID)
		default:
			m_GraphFillType = GB_SOLID;
			break;
	}
}

int CGraphObject::GetGraphFillType()
{
	return m_GraphFillType;
}

void CGraphObject::SetGraphGradientColors(COLORREF g_col1, COLORREF g_col2)
{
	// Set graph background gradient colors
	m_GraphGradientColor1 = g_col1;
	m_GraphGradientColor2 = g_col2;

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1)<<8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1)<<8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1)<<8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2)<<8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2)<<8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2)<<8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill( m_GradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType );
}

void CGraphObject::SetGradientFillType(int f_type)
{
	// Set gradient fill type
	switch( f_type )
	{
		// Set horizontal gradient fill type
		case FT_HORIZONTAL:
			m_GraphGradientFillType = GRADIENT_FILL_RECT_H;
			break;

		// Set vertical gradient fill type
		case FT_VERTICAL:
			m_GraphGradientFillType = GRADIENT_FILL_RECT_V;
			break;

		// Set default gradient fill type (FT_VERTICAL)
		default:
			m_GraphGradientFillType = GRADIENT_FILL_RECT_H;
			break;
	}

	// Draw graph gradient background
	GRADIENT_RECT gRct;
	TRIVERTEX gVert[2];

	gVert[0].x = 0;
	gVert[0].y = 0;
	gVert[0].Red = (GetRValue(m_GraphGradientColor1)<<8);
	gVert[0].Green = (GetGValue(m_GraphGradientColor1)<<8);
	gVert[0].Blue = (GetBValue(m_GraphGradientColor1)<<8);
	gVert[0].Alpha = 0x0000;

	gVert[1].x = m_Size.cx;
	gVert[1].y = m_Size.cy;
	gVert[1].Red = (GetRValue(m_GraphGradientColor2)<<8);
	gVert[1].Green = (GetGValue(m_GraphGradientColor2)<<8);
	gVert[1].Blue = (GetBValue(m_GraphGradientColor2)<<8);
	gVert[1].Alpha = 0x0000;

	gRct.UpperLeft = 0;
	gRct.LowerRight = 1;
	::GradientFill( m_GradientDC->m_hDC, gVert, 2, &gRct, 1, m_GraphGradientFillType );
}

int CGraphObject::GetGradientFillType()
{
	return m_GraphGradientFillType;
}

void CGraphObject::SetGraphTitleShadow(BOOL t_shadow)
{
	// Set graph title shadow
	m_GraphTitleShadow = t_shadow;
}

void CGraphObject::SetGraphSubtitleShadow(BOOL s_shadow)
{
	// Set graph subtitle shadow
	m_GraphSubtitleShadow = s_shadow;
}

BOOL CGraphObject::GetGraphTitleShadow()
{
	return m_GraphTitleShadow;
}

BOOL CGraphObject::GetGraphSubtitleShadow()
{
	return m_GraphSubtitleShadow;
}

void CGraphObject::SetGraphTitleColor(COLORREF t_color)
{
	// Set graph title color
	m_GraphTitleColor = t_color;
}

void CGraphObject::SetGraphSubtitleColor(COLORREF s_color)
{
	// Set graph subtitle color
	m_GraphSubtitleColor = s_color;
}

COLORREF CGraphObject::GetGraphTitleColor()
{
	return m_GraphTitleColor;
}

COLORREF CGraphObject::GetGraphSubtitleColor()
{
	return m_GraphSubtitleColor;
}

void CGraphObject::SetGraphLabelsColor(COLORREF l_color)
{
	// Get graph type
	switch ( m_GraphType )
	{
		// Set 2D bar graph labels color
		case GT_2DBAR:
			m_2DBarGraph->SetLabelsColor( l_color );
			break;

		// Set 2D line graph labels color
		case GT_2DLINE:
			m_2DLineGraph->SetLabelsColor( l_color );
			break;

		// Set default graph type (2D bar graph) labels color
		default:
			break;
	}

	Invalidate( FALSE );
}

COLORREF CGraphObject::GetGraphLabelsColor()
{
	COLORREF result;

	// Get graph type
	switch ( m_GraphType )
	{
		// Get 2D bar graph labels color
		case GT_2DBAR:
			result = m_2DBarGraph->GetLabelsColor();
			break;

		// Set 2D line graph labels color
		case GT_2DLINE:
			result = m_2DLineGraph->GetLabelsColor();
			break;

		// Set default graph type (2D bar graph) labels color
		default:
			break;
	}

	return result;
}

void CGraphObject::SetGraphLegendBackgroundColor(COLORREF l_bgcolor)
{
	// Get graph type
	switch ( m_GraphType )
	{
		// Set 2D bar graph legend background color
		case GT_2DBAR:
			m_2DBarGraph->SetLegendBackgroundColor( l_bgcolor );
			break;

		// Set 2D line graph legend background color
		case GT_2DLINE:
			m_2DLineGraph->SetLegendBackgroundColor( l_bgcolor );
			break;

		// Set 2D pie graph legend background color
		case GT_2DPIE:
			m_2DPieGraph->SetLegendBackgroundColor( l_bgcolor );
			break;

		// Set default graph type (2D bar graph) legend background color
		default:
			break;
	}

	Invalidate( FALSE );
}

COLORREF CGraphObject::GetGraphLegendBackgroundColor()
{
	COLORREF result;

	// Get graph type
	switch ( m_GraphType )
	{
		// Get 2D bar graph legend background color
		case GT_2DBAR:
			result = m_2DBarGraph->GetLegendBackgroundColor();
			break;

		// Get 2D line graph legend background color
		case GT_2DLINE:
			result = m_2DLineGraph->GetLegendBackgroundColor();
			break;

		// Get 2D pie graph legend background color
		case GT_2DPIE:
			m_2DPieGraph->GetLegendBackgroundColor();
			break;

		// Get default graph type (2D bar graph) legend background color
		default:
			break;
	}

	return result;
}

void CGraphObject::SetGraphLegendTextColor(COLORREF t_color)
{
	// Get graph type
	switch ( m_GraphType )
	{
		// Set 2D bar graph legend text color
		case GT_2DBAR:
			m_2DBarGraph->SetLegendTextColor( t_color );
			break;

		// Set 2D line graph legend text color
		case GT_2DLINE:
			m_2DLineGraph->SetLegendTextColor( t_color );
			break;

		// Set 2D pie graph legend text color
		case GT_2DPIE:
			m_2DPieGraph->SetLegendTextColor( t_color );
			break;

		// Set default graph type (2D bar graph) legend text color
		default:
			break;
	}

	Invalidate( FALSE );
}

COLORREF CGraphObject::GetGraphLegendTextColor()
{
	COLORREF result;

	// Get graph type
	switch ( m_GraphType )
	{
		// Get 2D bar graph legend text color
		case GT_2DBAR:
			result = m_2DBarGraph->GetLegendTextColor();
			break;

		// Get 2D line graph legend text color
		case GT_2DLINE:
			result = m_2DLineGraph->GetLegendTextColor();
			break;

		// Get 2D pie graph legend text color
		case GT_2DPIE:
			m_2DPieGraph->GetLegendTextColor();
			break;

		// Get default graph type (2D bar graph) legend text color
		default:
			break;
	}

	return result;
}

void CGraphObject::ShowGraphLegend(BOOL legend)
{
	// Set graph legend show
	m_ShowLegend = legend;
}

void CGraphObject::ClearGraph()
{
	m_2DPieGraph->ClearGraph();
}