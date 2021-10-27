// 2DPieGraph.h: interface for the C2DPieGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DPIEGRAPH_H__59B0EDD5_E8F6_4B44_97CB_6FEAFB11AF9C__INCLUDED_)
#define AFX_2DPIEGRAPH_H__59B0EDD5_E8F6_4B44_97CB_6FEAFB11AF9C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define AT_PIE_DRAW		10
#define AT_PIE_BLEND	11


struct _2DPieGraphSegments
{
	int index;
	double percent;
	COLORREF color;
	CString text;
	struct _2DPieGraphSegments* next;
};


class C2DPieGraph  
{
public:
	COLORREF GetLegendTextColor();
	void SetLegendTextColor( COLORREF t_color );
	COLORREF GetLegendBackgroundColor();
	void SetLegendBackgroundColor( COLORREF l_bgcolor );
	void SetFullSize( CSize full_size );
	void SetGraphPosition( CPoint g_position );
	void SetGraphSize( CSize g_size );
	void UpdateSegment( int s_index, double s_percent, COLORREF s_color, CString s_text );
	void SetGraphBkColor( COLORREF g_bkColor );
	void UpdateAnimation();
	void BuildGraphLegend( HDC hDC );
	int GetGraphAnimationPercent();
	BOOL GetGraphAnimation();
	void SetGraphAnimation( BOOL g_animation, int a_type );
	void ClearGraph();
	void DeleteSegment( int s_index );
	void AddSegment( double s_percent, COLORREF s_color, CString s_text );
	void BuildGraph( HDC hDC );
	C2DPieGraph( CPoint g_position, CSize g_size );
	virtual ~C2DPieGraph();

private:
	COLORREF m_LegendTextColor;
	COLORREF m_LegendBackgroundColor;
	CSize m_FullSize;
	int m_BlendSegments;
	COLORREF m_GraphBkColor;
	HBITMAP m_hOldBlendBitmap;
	HBITMAP m_hBlendBitmap;
	HDC m_hBlendDC;
	int m_AnimationType;
	CFont* m_LegendFont;
	CPoint m_AnimationStartPoint;
	double m_DrawPercentCompleted;
	double m_DrawAnimationAngle;
	double m_AnimationPercent;
	double m_DrawTotalPercent;
	BOOL m_Animation;
	CSize m_Size;
	CPoint m_Position;
	int m_SegmentsNumber;
	struct _2DPieGraphSegments* m_Segments;
};

#endif // !defined(AFX_2DPIEGRAPH_H__59B0EDD5_E8F6_4B44_97CB_6FEAFB11AF9C__INCLUDED_)
