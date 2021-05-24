#if !defined(AFX_GRAPHOBJECT_H__45515655_438B_444F_A8F0_77DAC449F81B__INCLUDED_)
#define AFX_GRAPHOBJECT_H__45515655_438B_444F_A8F0_77DAC449F81B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GraphObject.h : header file

#include "2DPieGraph.h"
#include "2DBarGraph.h"
#include "2DLineGraph.h"
//

#include "CHyperLink.h"

#define GT_2DBAR		0
#define	GT_2DPIE		1
#define GT_2DLINE		2

#define IDT_ANIMATION	50000
#define ANIMATION_TIME	50

#define GB_SOLID		10
#define GB_GRADIENT		11
#define FT_HORIZONTAL	20
#define FT_VERTICAL		21

/////////////////////////////////////////////////////////////////////////////
// CGraphObject window

class CGraphObject : public CWnd
{
// Construction
public:
	CGraphObject();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGraphObject)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	void ShowGraphLegend( BOOL legend );
	COLORREF GetGraphLegendTextColor();
	void SetGraphLegendTextColor( COLORREF t_color );
	COLORREF GetGraphLegendBackgroundColor();
	void SetGraphLegendBackgroundColor( COLORREF l_bgcolor );
	COLORREF GetGraphLabelsColor();
	void SetGraphLabelsColor( COLORREF l_color );
	COLORREF GetGraphSubtitleColor();
	COLORREF GetGraphTitleColor();
	void SetGraphSubtitleColor( COLORREF s_color );
	void SetGraphTitleColor( COLORREF t_color );
	BOOL GetGraphSubtitleShadow();
	BOOL GetGraphTitleShadow();
	void SetGraphSubtitleShadow( BOOL s_shadow );
	void SetGraphTitleShadow( BOOL t_shadow );
	int GetGradientFillType();
	void SetGradientFillType( int f_type );
	void SetGraphGradientColors( COLORREF g_col1, COLORREF g_col2 );
	int GetGraphFillType();
	void SetGraphFillType( int g_fill );
	void Delete2DLineGraphSeries( int s_index );
	void Add2DLineGraphSeries( CString s_text, COLORREF s_color );
	void Set2DLineGraphValue( int s_segment, int s_series, int s_value );
	void Delete2DLineGraphSegment( int s_index );
	void Add2DLineGraphSegment( CString s_text );
	void Detele2DBarGraphSeries( int s_index );
	void Add2DBarGraphSeries( CString s_text, COLORREF s_color );
	void Set2DBarGraphValue( int s_segment, int s_series, double s_value );
	void Delete2DBarGraphSegment( int s_index );
	void Add2DBarGraphSegment( CString s_text );
	void SetGraphPosition( CPoint g_position );
	void SetGraphSize( CSize g_size );
	void Update2DPieGraphSegment( int s_index, double s_percent, COLORREF s_color, CString s_text );
	void SetGraphSubtitle( CString g_subtitle );
	void SetGraphTitle( CString g_title );
	void SetGraphAlert(BOOL g_alert);
	void SetGraphAlert(CString g_action_string, CString g_action_line);
	COLORREF GetGraphBackgroundColor();
	void SetGraphBackgroundColor(COLORREF g_bgcolor);
	BOOL GetGraphAnimation();
	void SetGraphAnimation(BOOL g_animation, int a_type);
	void Delete2DPieGraphSegment( int s_index );
	void Add2DPieGraphSegment( double s_percent, COLORREF s_color, CString s_text );
	void CreateGraph( int g_type );
	void ClearGraph();
	virtual ~CGraphObject();

	// Generated message map functions
protected:
	//{{AFX_MSG(CGraphObject)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	DECLARE_MESSAGE_MAP()
private:
	BOOL m_ShowLegend;
	BOOL init = TRUE;
	COLORREF m_GraphSubtitleColor;
	COLORREF m_GraphTitleColor;
	COLORREF m_GraphAlertColor;
	BOOL m_GraphSubtitleShadow;
	BOOL m_GraphTitleShadow;
	DWORD m_GraphGradientFillType;
	COLORREF m_GraphGradientColor2;
	COLORREF m_GraphGradientColor1;
	int m_GraphFillType;
	BOOL m_GraphAnimation;
	CFont* m_GraphSubtitleFont;
	CFont* m_GraphAlertFont;
	CFont* m_GraphAlertFontTrue;
	CFont* m_GraphAlertLineFont;
	CString m_GraphSubtitle;
	BOOL m_GraphAlert;
	CString m_GraphAlertString, m_GraphAlertLine;
	CBitmap* m_GradientBitmap;
	CDC* m_GradientDC;
	CBitmap* m_GraphBitmap;
	CDC* m_GraphDC;
	BOOL m_Update;
	CFont* m_GraphTitleFont;
	CString m_GraphTitle;
	COLORREF m_GraphBackgroundColor;
	int m_GraphType;
	CSize m_Size;
	CPoint m_Position;
	C2DLineGraph* m_2DLineGraph;
	C2DBarGraph* m_2DBarGraph;
	C2DPieGraph* m_2DPieGraph;
	CHyperLink	m_Link;
	CRect			mLinkRect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GRAPHOBJECT_H__45515655_438B_444F_A8F0_77DAC449F81B__INCLUDED_)
