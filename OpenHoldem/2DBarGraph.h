// 2DBarGraph.h: interface for the C2DBarGraph class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_2DBARGRAPH_H__62AE2E10_96DE_4410_87A1_2D2CDE38A601__INCLUDED_)
#define AFX_2DBARGRAPH_H__62AE2E10_96DE_4410_87A1_2D2CDE38A601__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define AT_BAR_DRAW_ALL			20
#define AT_BAR_DRAW_SEGMENT		21
#define AT_BAR_DRAW_SERIES		22
#define AT_BAR_BLEND_SEGMENT	25
#define AT_BAR_BLEND_SERIES		26


struct _2DBarGraphValues
{
	int index;
	double value;
	struct _2DBarGraphValues* next;
};

struct _2DBarGraphSeries
{
	int index;
	COLORREF color;
	CString text;
	struct _2DBarGraphValues* values;
	struct _2DBarGraphSeries* next;
};

struct _2DBarGraphSegments
{
	int index;
	CString text;
	struct _2DBarGraphSegments* next;
};

class C2DBarGraph  
{
public:
	COLORREF GetLegendTextColor();
	void SetLegendTextColor( COLORREF t_color );
	COLORREF GetLegendBackgroundColor();
	void SetLegendBackgroundColor( COLORREF l_bgcolor );
	COLORREF GetLabelsColor();
	void SetLabelsColor( COLORREF l_color );
	void SetFullSize( CSize full_size );
	void UpdateAnimation();
	void SetGraphAnimation( BOOL g_animation, int a_type );
	void DeleteSeries( int s_series );
	void AddSeries( CString s_text, COLORREF s_color );
	void SetValue( int s_segment, int s_series, double s_value );
	void DeleteSegment( int s_index );
	void AddSegment( CString s_text );
	void SetGraphSize( CSize g_size );
	void SetGraphPosition( CPoint g_position );
	void SetGraphBkColor( COLORREF g_bkColor );
	int GetGraphAnimationPercent();
	BOOL GetGraphAnimation();
	void BuildGraphLegend( HDC hDC );
	void BuildGraph( HDC hDC );
	void ClearGraph();
	C2DBarGraph( CPoint g_position, CSize g_size );
	virtual ~C2DBarGraph();

private:
	COLORREF m_LegendTextColor;
	COLORREF m_LegendBackgroundColor;
	COLORREF m_LabelsColor;
	int m_BlendSeriesCompleted;
	int m_BlendSegmentsCompleted;
	HBITMAP m_hOldBlendBitmap;
	HBITMAP m_hBlendBitmap;
	HDC m_hBlendDC;
	BOOL m_AnimationPauseStart;
	int m_AnimationPause;
	int m_DrawSeriesCompleted;
	int m_DrawSegmentsCompleted;
	int m_AnimationType;
	int m_AnimationPercent;
	BOOL m_Animation;
	COLORREF m_GraphBkColor;
	CFont* m_ValueFont;
	CFont* m_SegmentFont;
	CFont* m_LegendFont;
	int m_SeriesNumber;
	int m_SegmentsNumber;
	CSize m_FullSize;
	CSize m_Size;
	CPoint m_Position;
	struct _2DBarGraphSeries* m_Series;
	struct _2DBarGraphSegments* m_Segments;
};

#endif // !defined(AFX_2DBARGRAPH_H__62AE2E10_96DE_4410_87A1_2D2CDE38A601__INCLUDED_)
