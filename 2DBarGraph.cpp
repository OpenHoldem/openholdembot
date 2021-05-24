// 2DBarGraph.cpp: implementation of the C2DBarGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "2DBarGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DBarGraph::C2DBarGraph(CPoint g_position, CSize g_size)
{
	// Set graph position and size
	m_Position = g_position;
	m_Size = g_size;

	// Set graph segments and series number
	m_SegmentsNumber = 0;
	m_Segments = NULL;
	m_SeriesNumber = 0;
	m_Series = NULL;

	// Set graph legend font
	m_LegendFont = new CFont();
	m_LegendFont->CreateFont( int(m_Size.cy*0.075), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set graph series font
	m_SegmentFont = NULL;

	// Set graph values font
	m_ValueFont = new CFont();
	m_ValueFont->CreateFont( int(m_Size.cy*0.1), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set default graph background color
	m_GraphBkColor = RGB(255,255,2555);

	// Set default graph labels color
	m_LabelsColor = RGB(0,0,0);

	// Set default graph legend background color
	m_LegendBackgroundColor = RGB(255,255,255);
	m_LegendTextColor = RGB(0,0,0);
}

C2DBarGraph::~C2DBarGraph()
{
	// Delete graph segments and series
	ClearGraph();

	// Delete graph legend font
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = NULL;

	// Delete graph segment font
	m_SegmentFont = NULL;

	// Delete graph values font
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = NULL;
}

void C2DBarGraph::ClearGraph()
{
	_2DBarGraphSeries *curr_ser, *prev_ser;
	_2DBarGraphSegments *curr_seg, *prev_seg;
	_2DBarGraphValues *curr_val, *prev_val;

	// Delete all graph series
	curr_ser = m_Series;
	while ( curr_ser != NULL )
	{
		// Delete all graph values
		curr_val = curr_ser->values;
		while ( curr_val != NULL )
		{
			prev_val = curr_val;
			curr_val = curr_val->next;
			delete prev_val;
		}

		prev_ser = curr_ser;
		curr_ser = curr_ser->next;
		delete prev_ser;
	}

	// Delete all graph segments
	curr_seg = m_Segments;
	while ( curr_seg != NULL )
	{
		prev_seg = curr_seg;
		curr_seg = curr_seg->next;
		delete prev_seg;
	}

	// Set graph segment and series parameters
	m_Segments = NULL;
	m_SegmentsNumber = 0;
	m_Series = NULL;
	m_SeriesNumber = 0;
}

void C2DBarGraph::BuildGraph(HDC hDC)
{
	int seg_width, seg_height;
	CPoint ser_start, seg_font_pos;
	int ser_width;
	double max_value;

	// Set graph step parameters
	int seg_area = int(m_Size.cx*0.6);
	int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
	int ser_step = int(double(seg_step)/8 + 0.5);

	// Set segment font height
	int seg_number = m_SegmentsNumber;
	if ( seg_number == 1 )
		seg_number++;
	int font_height = int((double(seg_area)/double(seg_number))*0.38 + 0.5);

	_2DBarGraphSegments *curr_seg;
	_2DBarGraphSeries *c_ser, *curr_ser, *cc_ser;
	_2DBarGraphValues *c_val, *curr_val;
	_2DBarGraphSegments *c_seg;

	HBRUSH hOldSerBrush, hSerBrush;
	HFONT hOldSegFont, hOldValFont;

	// Calculate graph x and y coordinate system begining
	int startX = m_Position.x + int( m_Size.cx*0.15 ) + 20;
	int startY = m_Position.y + int( m_Size.cy*0.95 ) + 25;
	// Draw x coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, m_Size.cx + 70, startY );
	// Draw x coordinate axis arrow
	::MoveToEx( hDC, m_Size.cx + 70, startY, NULL );
	::LineTo( hDC, int((m_Size.cx + 70) *0.98), startY-int(m_Size.cy*0.02) );
	::MoveToEx( hDC, m_Size.cx + 70, startY, NULL );
	::LineTo( hDC, int((m_Size.cx + 70) *0.98), startY+int(m_Size.cy*0.02) );
	// Draw y coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, startX, m_Position.y - 90 );
	// Draw y coordinate axis arrow
	::MoveToEx( hDC, startX, m_Position.y - 90, NULL );
	::LineTo( hDC, startX - int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.065) - 90);
	::MoveToEx( hDC, startX, m_Position.y - 90, NULL );
	::LineTo( hDC, startX + int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.065) - 90);

	// If animation running
	if ( m_Animation == TRUE )
	{
		// If running draw-all animation
		if ( m_AnimationType == AT_BAR_DRAW_ALL )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

				// Get max series values
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment values
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max values
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw values text
				COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				CString str_max;
				str_max.Format( "%d", max_value );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.85), str_max, str_max.GetLength() );
				str_max.Format( "%d", int(double(max_value)/2.0) );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.45), str_max, str_max.GetLength() );
				str_max = "0";
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldValFont );

				// Draw graph segments
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// Set new graph segment font
					m_SegmentFont = new CFont();
					m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
					// Draw segment text
					hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
					::SetBkMode( hDC, TRANSPARENT );
					::SetTextAlign( hDC, TA_CENTER );
					::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
					::SetTextAlign( hDC, TA_LEFT );
					::SetBkMode( hDC, OPAQUE );
					::SelectObject( hDC, hOldSegFont );
					m_SegmentFont->DeleteObject();
					delete m_SegmentFont;

					// Draw graph series
					curr_ser = m_Series;
					curr_val = NULL;
					while ( curr_ser != NULL )
					{
						// Draw graph series values
						curr_val = curr_ser->values;
						while ( curr_val->index != curr_seg->index )
							curr_val = curr_val->next;
						// Calculate values height
						seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
						seg_height = int(double(m_AnimationPercent)/100.0 * seg_height );
						// Draw values rectangle
						hSerBrush = ::CreateSolidBrush( curr_ser->color );
						hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
						::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
						::SelectObject( hDC, hOldSerBrush );
						::DeleteObject( hSerBrush );

						// Get next series
						curr_ser = curr_ser->next;

						// Set next series start point
						if ( curr_ser != NULL )
							ser_start.x += ser_width + ser_step;
						else
							ser_start.x += ser_width;
					}

					// Get next segment
					curr_seg = curr_seg->next;

					// Set next series start point
					ser_start.x += seg_step;
				}
				::SetTextColor( hDC, oldTextColor );
			}
		}
		// If running draw-segment animation
		else if ( m_AnimationType == AT_BAR_DRAW_SEGMENT )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width 
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

				// Get max series values
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment values
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max values
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw values text
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
				CString str_max;
				str_max.Format( "%d", max_value );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.85), str_max, str_max.GetLength() );
				str_max.Format( "%d", int(double(max_value)/2.0) );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.45), str_max, str_max.GetLength() );
				str_max = "0";
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldValFont );

				// Draw graph segments
				BOOL exit_anim = FALSE;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// Set new graph segment font
					m_SegmentFont = new CFont();
					m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
					// Draw segment text
					hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
					::SetBkMode( hDC, TRANSPARENT );
					::SetTextAlign( hDC, TA_CENTER );
					::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
					::SetTextAlign( hDC, TA_LEFT );
					::SetBkMode( hDC, OPAQUE );
					::SelectObject( hDC, hOldSegFont );
					m_SegmentFont->DeleteObject();
					delete m_SegmentFont;

					// If segment animation finished
					if ( m_DrawSegmentsCompleted >= curr_seg->index )
					{
						// Draw graph series
						curr_ser = m_Series;
						curr_val = NULL;
						while ( curr_ser != NULL )
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							while ( curr_val->index != curr_seg->index )
								curr_val = curr_val->next;
							// Calculate values height
							seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
							// Draw values rectangle
							hSerBrush = ::CreateSolidBrush( curr_ser->color );
							hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
							::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
							::SelectObject( hDC, hOldSerBrush );
							::DeleteObject( hSerBrush );

							// Get next series
							curr_ser = curr_ser->next;

							// Set next series start point
							if ( curr_ser != NULL )
								ser_start.x += ser_width + ser_step;
							else
								ser_start.x += ser_width;
						}
					}
					// If animating current segment
					else
					{
						// Draw graph series
						curr_ser = m_Series;
						curr_val = NULL;
						while ( curr_ser != NULL )
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							while ( curr_val->index != curr_seg->index )
								curr_val = curr_val->next;
							// Calculate values height
							seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
							seg_height = int(double(m_AnimationPercent)/100.0 * seg_height );
							// Draw values rectangle
							hSerBrush = ::CreateSolidBrush( curr_ser->color );
							hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
							::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
							::SelectObject( hDC, hOldSerBrush );
							::DeleteObject( hSerBrush );

							// Get next series
							curr_ser = curr_ser->next;

							// Set next series start point
							if ( curr_ser != NULL )
								ser_start.x += ser_width + ser_step;
							else
								ser_start.x += ser_width;
						}

						// Set exit flag
						exit_anim = TRUE;
					}

					// Check exit flag
					if ( exit_anim == TRUE )
					{
						// Exit animation loop
						break;
					}

					// Get next segment
					curr_seg = curr_seg->next;

					// Set next series start point
					ser_start.x += seg_step;
				}
				::SetTextColor( hDC, oldTextColor );
			}
		}
		// If running draw-segment animation
		else if ( m_AnimationType == AT_BAR_DRAW_SERIES )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

				// Get max series values
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment values
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max values
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw values text
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
				CString str_max;
				str_max.Format( "%d", max_value );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.85), str_max, str_max.GetLength() );
				str_max.Format( "%d", int(double(max_value)/2.0) );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.45), str_max, str_max.GetLength() );
				str_max = "0";
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldValFont );

				seg_font_pos = ser_start;
				// Set new graph segment font
				m_SegmentFont = new CFont();
				m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
				hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
				::SetTextAlign( hDC, TA_CENTER );
				::SetBkMode( hDC, TRANSPARENT );
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// Draw segment text
					::TextOut( hDC, seg_font_pos.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );

					seg_font_pos.x += seg_width + seg_step;
					curr_seg = curr_seg->next;
				}
				::SetBkMode( hDC, OPAQUE );
				::SetTextAlign( hDC, TA_LEFT );
				::SelectObject( hDC, hOldSegFont );
				m_SegmentFont->DeleteObject();
				delete m_SegmentFont;

				// Draw graph segments
				BOOL exit_anim = FALSE;
				curr_ser = m_Series;
				while ( curr_ser != NULL )
				{
					// Draw graph segments
					curr_seg = m_Segments;
					while ( curr_seg != NULL )
					{
						// If series animation finished
						if ( m_DrawSeriesCompleted >= curr_ser->index )
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
							while ( curr_val->index != curr_seg->index )
							{
								c_ser = m_Series;
								while ( c_ser != NULL )
								{
									c_ser = c_ser->next;
									if ( c_ser != NULL )
										ser_start.x += ser_width + ser_step;
									else
										ser_start.x += ser_width;
								}
								if ( curr_seg != NULL )
									ser_start.x += seg_step;
								curr_val = curr_val->next;
							}

							c_ser = m_Series;
							while ( c_ser != curr_ser )
							{
								ser_start.x += ser_width + ser_step;
								c_ser = c_ser->next;
							}

							// Check graph values index
							if ( curr_val->index == curr_seg->index )
							{
								// Calculate values height
								seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
								// Draw values rectangle
								hSerBrush = ::CreateSolidBrush( curr_ser->color );
								hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
								::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
								::SelectObject( hDC, hOldSerBrush );
								::DeleteObject( hSerBrush );
							}
						}
						// If animating current series
						else
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
							while ( curr_val->index != curr_seg->index )
							{
								c_ser = m_Series;
								while ( c_ser != NULL )
								{
									c_ser = c_ser->next;
									if ( c_ser != NULL )
										ser_start.x += ser_width + ser_step;
									else
										ser_start.x += ser_width;
								}
								if ( curr_seg != NULL )
									ser_start.x += seg_step;
								curr_val = curr_val->next;
							}

							c_ser = m_Series;
							while ( c_ser != curr_ser )
							{
								ser_start.x += ser_width + ser_step;
								c_ser = c_ser->next;
							}

							// Check graph values index
							if ( curr_val->index == curr_seg->index )
							{
								// Calculate values height
								seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
								seg_height = int(double(m_AnimationPercent)/100.0 * seg_height );
								// Draw values rectangle
								hSerBrush = ::CreateSolidBrush( curr_ser->color );
								hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
								::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
								::SelectObject( hDC, hOldSerBrush );
								::DeleteObject( hSerBrush );
							}

							// Set exit flag
							exit_anim = TRUE;
						}

						// Get next segment
						curr_seg = curr_seg->next;

						// Check exit flag
						if ( ( curr_seg == NULL ) && ( exit_anim == TRUE ) )
						{
							// Exit animation loop
							break;
						}
						else
							exit_anim = FALSE;
					}

					// Check exit flag
					if ( exit_anim == TRUE )
					{
						// Exit animation loop
						break;
					}

					// Get next series
					curr_ser = curr_ser->next;
				}
				::SetTextColor( hDC, oldTextColor );
			}
		}
		// If running blend-segment animation
		else if ( m_AnimationType == AT_BAR_BLEND_SEGMENT )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

				// Get max series values
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment values
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max values
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw values text
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
				CString str_max;
				str_max.Format( "%d", max_value );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.85), str_max, str_max.GetLength() );
				str_max.Format( "%d", int(double(max_value)/2.0) );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.45), str_max, str_max.GetLength() );
				str_max = "0";
				::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldValFont );

				// Draw graph segments
				BOOL exit_anim = FALSE;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// If animating current segment
					if ( m_BlendSegmentsCompleted < curr_seg->index )
					{
						// Draw graph series values
						c_seg = m_Segments;
						ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						while ( c_seg->index < curr_seg->index )
						{
							c_ser = m_Series;
							while ( c_ser != NULL )
							{
								c_ser = c_ser->next;
								if ( c_ser != NULL )
									ser_start.x += ser_width + ser_step;
								else
									ser_start.x += ser_width;
							}
							if ( curr_seg != NULL )
								ser_start.x += seg_step;

							c_seg = c_seg->next;
						}

						// Set new graph segment font
						m_SegmentFont = new CFont();
						m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
							DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
						// Draw segment text
						hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
						::SetBkMode( hDC, TRANSPARENT );
						::SetTextAlign( hDC, TA_CENTER );
						::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
						::SetTextAlign( hDC, TA_LEFT );
						::SetBkMode( hDC, OPAQUE );
						::SelectObject( hDC, hOldSegFont );
						m_SegmentFont->DeleteObject();
						delete m_SegmentFont;

						// Create graph blend DC and bitmap
						m_hBlendDC = ::CreateCompatibleDC( hDC );
						m_hBlendBitmap = ::CreateCompatibleBitmap( hDC, m_FullSize.cx, m_FullSize.cy );
						m_hOldBlendBitmap = (HBITMAP)::SelectObject( m_hBlendDC, m_hBlendBitmap );

						// Create temporary DC and bitmap
						HDC tempDC = ::CreateCompatibleDC( hDC );
						HBITMAP tempBitmap = ::CreateCompatibleBitmap( hDC, m_FullSize.cx, m_FullSize.cy );
						HBITMAP m_tempOldBitmap = (HBITMAP)::SelectObject( tempDC, tempBitmap );

						// Clear graph blend and temporary DC
						::BitBlt( m_hBlendDC, 0, 0, m_FullSize.cx, m_FullSize.cy, hDC, 0, 0, SRCCOPY );
						::BitBlt( tempDC, 0, 0, m_FullSize.cx, m_FullSize.cy, hDC, 0, 0, SRCCOPY );

						// Draw graph series
						curr_ser = m_Series;
						curr_val = NULL;
						while ( curr_ser != NULL )
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							while ( curr_val->index != curr_seg->index )
								curr_val = curr_val->next;
							// Calculate values height
							seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
							// Draw values rectangle
							hSerBrush = ::CreateSolidBrush( curr_ser->color );
							hOldSerBrush = (HBRUSH)::SelectObject( m_hBlendDC, hSerBrush );
							::Rectangle( m_hBlendDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
							::SelectObject( m_hBlendDC, hOldSerBrush );
							::DeleteObject( hSerBrush );

							// Get next series
							curr_ser = curr_ser->next;

							// Set next series start point
							if ( curr_ser != NULL )
								ser_start.x += ser_width + ser_step;
							else
								ser_start.x += ser_width;
						}

						// Do alpha blending on temporary DC
						BLENDFUNCTION bf;
						bf.BlendOp = AC_SRC_OVER;
						bf.BlendFlags = 0;
						bf.AlphaFormat = 0;
						bf.SourceConstantAlpha = BYTE((double(m_AnimationPercent)/100.0)*255);
						::AlphaBlend( tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, m_hBlendDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, bf );

						// Do transparent bliting on destination DC
						::TransparentBlt( hDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, ::GetPixel(tempDC,m_Position.x,m_Position.y) );

						// Set exit flag
						exit_anim = TRUE;

						// Delete graph blend DC and bitmap
						::SelectObject( m_hBlendDC, m_hOldBlendBitmap );
						::DeleteDC( m_hBlendDC );
						::DeleteObject( m_hBlendBitmap );

						// Delete temporary DC and bitmap
						::SelectObject( tempDC, m_tempOldBitmap );
						::DeleteDC( tempDC );
						::DeleteObject( tempBitmap );

						// Draw blended graph segments
						COLORREF oldTxtColor = ::SetTextColor( hDC, m_LabelsColor );
						c_seg = m_Segments;
						ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						while ( c_seg->index < curr_seg->index )
						{
							// Set new graph legend font
							m_SegmentFont = new CFont();
							m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
								DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
							// Draw segment text
							hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
							::SetBkMode( hDC, TRANSPARENT );
							::SetTextAlign( hDC, TA_CENTER );
							::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), c_seg->text, c_seg->text.GetLength() );
							::SetTextAlign( hDC, TA_LEFT );
							::SetBkMode( hDC, OPAQUE );
							::SelectObject( hDC, hOldSegFont );
							m_SegmentFont->DeleteObject();
							delete m_SegmentFont;

							// Draw graph series
							curr_ser = m_Series;
							curr_val = NULL;
							while ( curr_ser != NULL )
							{
								//  graph series values
								curr_val = curr_ser->values;
								while ( curr_val->index != c_seg->index )
									curr_val = curr_val->next;
								// Calculate values height
								seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
								// Draw values rectangle
								hSerBrush = ::CreateSolidBrush( curr_ser->color );
								hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
								::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
								::SelectObject( hDC, hOldSerBrush );
								::DeleteObject( hSerBrush );

								// Get next series
								curr_ser = curr_ser->next;

								// Set next series start point
								if ( curr_ser != NULL )
									ser_start.x += ser_width + ser_step;
								else
									ser_start.x += ser_width;
							}

							// Get next segment
							c_seg = c_seg->next;

							// Set next series start point
							ser_start.x += seg_step;
						}
						::SetTextColor( hDC, oldTxtColor );
					}

					// Check exit flag
					if ( exit_anim == TRUE )
					{
						// Exit animation loop
						break;
					}

					// Get next segment
					curr_seg = curr_seg->next;

					// Set next series start point
					ser_start.x += seg_step;
				}
				::SetTextColor( hDC, oldTextColor );
			}
		}
		// If running blend-series animation
		else if ( m_AnimationType == AT_BAR_BLEND_SERIES )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

				// Get max series values
				max_value = 0.0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment values
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max values
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw values text
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
				CString str_max;
				str_max.Format( "%3.1f", max_value );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), (ser_start.y-int(m_Size.cy*0.85)), str_max, str_max.GetLength() );
				str_max.Format( "%3.1f", max_value/2.0 );
				::TextOut( hDC, startX-int(m_Size.cx*0.02), (ser_start.y-int(m_Size.cy*0.45)), str_max, str_max.GetLength() );
				str_max = "0.0";
				::TextOut( hDC, startX-int(m_Size.cx*0.02), (ser_start.y-int(m_Size.cy*0.05)), str_max, str_max.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldValFont );

				seg_font_pos = ser_start;
				// Set new graph segment font
				m_SegmentFont = new CFont();
				m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
				hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
				::SetTextAlign( hDC, TA_CENTER );
				::SetBkMode( hDC, TRANSPARENT );
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// Draw segment text
					::TextOut( hDC, seg_font_pos.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );

					seg_font_pos.x += seg_width + seg_step;
					curr_seg = curr_seg->next;
				}
				::SetBkMode( hDC, OPAQUE );
				::SetTextAlign( hDC, TA_LEFT );
				::SelectObject( hDC, hOldSegFont );
				m_SegmentFont->DeleteObject();
				delete m_SegmentFont;

				// Draw graph segments
				BOOL exit_anim = FALSE;
				curr_ser = m_Series;
				while ( curr_ser != NULL )
				{
					// If animating current series
					if ( m_BlendSeriesCompleted < curr_ser->index )
					{
						// Create graph blend DC and bitmap
						m_hBlendDC = ::CreateCompatibleDC( hDC );
						m_hBlendBitmap = ::CreateCompatibleBitmap( hDC, m_FullSize.cx, m_FullSize.cy );
						m_hOldBlendBitmap = (HBITMAP)::SelectObject( m_hBlendDC, m_hBlendBitmap );

						// Create temporary DC and bitmap
						HDC tempDC = ::CreateCompatibleDC( hDC );
						HBITMAP tempBitmap = ::CreateCompatibleBitmap( hDC, m_FullSize.cx, m_FullSize.cy );
						HBITMAP m_tempOldBitmap = (HBITMAP)::SelectObject( tempDC, tempBitmap );

						// Clear graph blend and temporary DC
						::BitBlt( m_hBlendDC, 0, 0, m_FullSize.cx, m_FullSize.cy, hDC, 0, 0, SRCCOPY );
						::BitBlt( tempDC, 0, 0, m_FullSize.cx, m_FullSize.cy, hDC, 0, 0, SRCCOPY );

						// Draw graph segments
						curr_seg = m_Segments;
						while ( curr_seg != NULL )
						{
							// Draw graph series values
							curr_val = curr_ser->values;
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
							while ( curr_val->index != curr_seg->index )
							{
								c_ser = m_Series;
								while ( c_ser != NULL )
								{
									c_ser = c_ser->next;
									if ( c_ser != NULL )
										ser_start.x += ser_width + ser_step;
									else
										ser_start.x += ser_width;
								}
								if ( curr_seg != NULL )
									ser_start.x += seg_step;
								curr_val = curr_val->next;
							}

							c_ser = m_Series;
							while ( c_ser != curr_ser )
							{
								ser_start.x += ser_width + ser_step;
								c_ser = c_ser->next;
							}

							// Check graph values index
							if ( curr_val->index == curr_seg->index )
							{
								// Calculate values height
								seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy*0.8));
								// Draw values rectangle
								hSerBrush = ::CreateSolidBrush( curr_ser->color );
								hOldSerBrush = (HBRUSH)::SelectObject( m_hBlendDC, hSerBrush );
								::Rectangle( m_hBlendDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
								::SelectObject( m_hBlendDC, hOldSerBrush );
								::DeleteObject( hSerBrush );
							}

							// Set exit flag
							exit_anim = TRUE;

							// Do alpha blending on temporary DC
							BLENDFUNCTION bf;
							bf.BlendOp = AC_SRC_OVER;
							bf.BlendFlags = 0;
							bf.AlphaFormat = 0;
							bf.SourceConstantAlpha = BYTE((double(m_AnimationPercent)/100.0)*255);
							::AlphaBlend( tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, m_hBlendDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, bf );

							// Do transparent bliting on destination DC
							::TransparentBlt( hDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, ::GetPixel(tempDC,m_Position.x,m_Position.y) );

							// Get next segment
							curr_seg = curr_seg->next;

							// Check exit flag
							if ( ( curr_seg == NULL ) && ( exit_anim == TRUE ) )
							{
								// Exit animation loop
								break;
							}
							else
								exit_anim = FALSE;
						}

						// Set exit flag
						exit_anim = TRUE;

						// Delete graph blend DC and bitmap
						::SelectObject( m_hBlendDC, m_hOldBlendBitmap );
						::DeleteDC( m_hBlendDC );
						::DeleteObject( m_hBlendBitmap );

						// Delete temporary DC and bitmap
						::SelectObject( tempDC, m_tempOldBitmap );
						::DeleteDC( tempDC );
						::DeleteObject( tempBitmap );

						// Draw blended graph series
						COLORREF oldTxtColor = ::SetTextColor( hDC, m_LabelsColor );
						c_ser = m_Series;
						while ( c_ser->index < curr_ser->index )
						{
							// Draw graph segments
							c_seg = m_Segments;
							while ( c_seg != NULL )
							{
								// Draw graph series values
								ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
								c_val = c_ser->values;
								while ( c_val->index != c_seg->index )
								{
									cc_ser = m_Series;
									while ( cc_ser != NULL )
									{
										cc_ser = cc_ser->next;
										if ( cc_ser != NULL )
											ser_start.x += ser_width + ser_step;
										else
											ser_start.x += ser_width;
									}
									if ( c_seg != NULL )
										ser_start.x += seg_step;
									c_val = c_val->next;
								}

								cc_ser = m_Series;
								while ( cc_ser != c_ser )
								{
									ser_start.x += ser_width + ser_step;
									cc_ser = cc_ser->next;
								}

								// Calculate values height
								seg_height = int((double(c_val->value)/double(max_value)) * (m_Size.cy*0.8));
								// Draw values rectangle
								hSerBrush = ::CreateSolidBrush( c_ser->color );
								hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
								::Rectangle( hDC, ser_start.x, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
								::SelectObject( hDC, hOldSerBrush );
								::DeleteObject( hSerBrush );

								// Get next segment
								c_seg = c_seg->next;
							}

							// Get next segment
							c_ser = c_ser->next;
						}

						// Check exit flag
						if ( exit_anim == TRUE )
						{
							// Exit animation loop
							break;
						}
					}

					// Get next series
					curr_ser = curr_ser->next;
				}
				::SetTextColor( hDC, oldTextColor );
			}
		}
	}
	// If no animation running
	else if ( m_Animation == FALSE )
	{
		// Check segments number
		if ( ( m_SegmentsNumber > 0 ) && ( m_SeriesNumber > 0 ) )
		{
			// Calculate segments width
			seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

			// Calculate series start point and width 
			if ( m_SegmentsNumber > 1 )
				ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
			else
				ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
			ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));

			// Get max series values
			max_value = 0.0;
			c_ser = m_Series;
			c_val = NULL;
			curr_seg = m_Segments;
			while ( curr_seg != NULL )
			{
				while ( c_ser != NULL )
				{
					// Find segment values
					c_val = c_ser->values;
					while ( c_val->index != curr_seg->index )
						c_val = c_val->next;
					// Check max values
					if ( c_val->value > max_value )
						max_value = c_val->value;

					// Get next series
					c_ser = c_ser->next;
				}

				// Get next segment
				curr_seg = curr_seg->next;
			}

			// Draw values text
			hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
			::SetBkMode( hDC, TRANSPARENT );
			::SetTextAlign( hDC, TA_RIGHT );
			COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
			CString str_max;
			str_max.Format( "%3.1f", max_value );
			::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*1.05), str_max, str_max.GetLength() );
			str_max.Format( "%3.1f", max_value/2.0 );
			::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.6), str_max, str_max.GetLength() );
			str_max = "0.0";
			::TextOut( hDC, startX-int(m_Size.cx*0.02), ser_start.y-int(m_Size.cy*0.05), str_max, str_max.GetLength() );
			::SetTextAlign( hDC, TA_LEFT );
			::SetBkMode( hDC, OPAQUE );
			::SelectObject( hDC, hOldValFont );

			// Draw graph segments
			curr_seg = m_Segments;
			int loop = 1;
			while ( curr_seg != NULL )
			{
				// Set new graph legend font
				m_SegmentFont = new CFont();
				m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
					DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
				// Draw segment text
				hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_CENTER );
				if (loop == 1)
				::TextOut( hDC, ser_start.x + int((seg_width + m_SeriesNumber*ser_step)/4), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
				else
					::TextOut(hDC, ser_start.x + int((seg_width + m_SeriesNumber * ser_step) / 1.5), startY + int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength());
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldSegFont );
				m_SegmentFont->DeleteObject();
				delete m_SegmentFont;

				// Draw graph series
				curr_ser = m_Series;
				curr_val = NULL;
				while ( curr_ser != NULL )
				{
					// Draw graph series values
					curr_val = curr_ser->values;
					while ( curr_val->index != curr_seg->index )
						curr_val = curr_val->next;
					// Calculate values height
					seg_height = int((double(curr_val->value)/double(max_value)) * (m_Size.cy));
					// Draw values rectangle
					hSerBrush = ::CreateSolidBrush( curr_ser->color );
					hOldSerBrush = (HBRUSH)::SelectObject( hDC, hSerBrush );
					::Rectangle( hDC, ser_start.x+2, ser_start.y - seg_height, ser_start.x+ser_width, ser_start.y );
					::SelectObject( hDC, hOldSerBrush );
					::DeleteObject( hSerBrush );

					// Get next series
					curr_ser = curr_ser->next;

					// Set next series start point
					if ( curr_ser != NULL )
						ser_start.x += ser_width + ser_step/6;
					else
						ser_start.x += ser_width;
				}

				// Get next segment
				curr_seg = curr_seg->next;

				// Set next series start point
				//ser_start.x += seg_step;
				loop++;
			}
			::SetTextColor( hDC, oldTextColor );
		}
	}
}

void C2DBarGraph::BuildGraphLegend(HDC hDC)
{
	_2DBarGraphSeries *curr_ser = m_Series;

	// Select legend font
	HGDIOBJ hOldFont = ::SelectObject( hDC, m_LegendFont->GetSafeHandle() );

	// Get text size
	CSize ts;
	::GetTextExtentPoint32( hDC, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26, &ts );

	int offset = (m_Size.cy - int(m_SeriesNumber*ts.cy))/2;

	CPoint start = CPoint( m_Position.x + int(m_Size.cx*0.9), m_Position.y - int(m_Size.cy*0.014) - offset);
	CSize sz;
	CString str = "";
	CRect rect;
	CBrush* rBrush = NULL;

	// Draw graph legend
	CBrush bgBrush( m_LegendBackgroundColor );
	HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, bgBrush.GetSafeHandle() );
	::Rectangle( hDC, m_Position.x + int(m_Size.cx*0.87), m_Position.y - offset-8, m_Position.x + int(m_Size.cx*1.33), m_Position.y + int((m_SeriesNumber+0.5)*ts.cy - offset*1.1) );
	::SelectObject( hDC, hOldBrush );
	bgBrush.DeleteObject();

	COLORREF oldTextColor = ::SetTextColor( hDC, m_LegendTextColor );
	while ( curr_ser != NULL )
	{
		// Draw segment color field
		rect = CRect( start.x, start.y + int(m_Size.cy*0.03)/2, start.x + int(m_Size.cx*0.05), start.y + int(m_Size.cy*0.075) );
		rBrush = new CBrush();
		rBrush->CreateSolidBrush( curr_ser->color );
		::FillRect( hDC, rect, (HBRUSH)rBrush->GetSafeHandle() );
		rBrush->DeleteObject();
		delete rBrush;

		// Draw segment text
		str.Format( "%s", curr_ser->text );
		::SetBkMode( hDC, TRANSPARENT );
		::TextOut( hDC, start.x + rect.Width() + int(m_Size.cx*0.01), start.y, str, str.GetLength() );
		::SetBkMode( hDC, OPAQUE );

		// Recalculate next text position
		::GetTextExtentPoint32( hDC, curr_ser->text, curr_ser->text.GetLength(), &sz );
		start.y += sz.cy;

		// Get next graph segment
		curr_ser = curr_ser->next;
	}
	::SetTextColor( hDC, oldTextColor );

	// Select old font
	::SelectObject( hDC, hOldFont );
}

BOOL C2DBarGraph::GetGraphAnimation()
{
	return m_Animation;
}

int C2DBarGraph::GetGraphAnimationPercent()
{
	return m_AnimationPercent;
}

void C2DBarGraph::SetGraphBkColor(COLORREF g_bkColor)
{
	// Set graph background color
	m_GraphBkColor = g_bkColor;
}

void C2DBarGraph::SetGraphPosition(CPoint g_position)
{
	// Set new graph position
	m_Position = g_position;
}

void C2DBarGraph::SetGraphSize(CSize g_size)
{
	// Set new graph size
	m_Size = g_size;

	// Set new graph legend font size
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = new CFont();
	m_LegendFont->CreateFont(int(m_Size.cy*0.095), int(m_Size.cx*0.028), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial");

	// Set new graph values font size
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = new CFont();
	m_ValueFont->CreateFont( int(m_Size.cy*0.1), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
}

void C2DBarGraph::AddSegment(CString s_text)
{
	// Increment segments number
	m_SegmentsNumber++;

	// Create new graph segment
	_2DBarGraphSegments* newSegment = new _2DBarGraphSegments;
	newSegment->index = m_SegmentsNumber;
	newSegment->text = s_text;
	newSegment->next = NULL;

	// Add new segment to the graph
	_2DBarGraphSegments* curr_seg = m_Segments;
	if ( curr_seg == NULL )
		m_Segments = newSegment;
	else
	{
		while ( curr_seg->next != NULL )
			curr_seg = curr_seg->next;
		curr_seg->next = newSegment;
	}

	// Check graph series number
	if ( m_SeriesNumber > 0 )
	{
		// Create new graph values
		_2DBarGraphValues* newValue = new _2DBarGraphValues;
		newValue->index = m_SegmentsNumber;
		newValue->value = 0;
		newValue->next = NULL;

		// Add new values to the graph segments
		_2DBarGraphSeries* curr_ser = m_Series;
		_2DBarGraphValues* curr_val = NULL;
		while ( curr_ser != NULL )
		{
			// Add new graph values to the graph
			curr_val = curr_ser->values;
			if ( curr_val == NULL )
				curr_ser->values = newValue;
			else
			{
				while ( curr_val->next != NULL )
					curr_val = curr_val->next;
				curr_val->next = newValue;
			}

			// Get next series
			curr_ser = curr_ser->next;
		}
	}
}

void C2DBarGraph::DeleteSegment(int s_index)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SegmentsNumber ) )
		AfxMessageBox( "Index is out of bounds...", MB_OK, NULL );
	else
	{
		// Decrement segments number
		m_SegmentsNumber--;

		// Delete segment from the graph
		_2DBarGraphSegments* curr_seg = m_Segments;
		_2DBarGraphSegments* prev_seg = NULL;
		if ( curr_seg != NULL )
		{
			// Find graph segment
			while ( curr_seg->index != s_index )
			{
				prev_seg = curr_seg;
				curr_seg = curr_seg->next;
			}

			// Delete first graph segment
			if ( curr_seg == m_Segments )
				m_Segments = m_Segments->next;
			// Delete other graph segment
			else
				prev_seg->next = curr_seg->next;

			// Delete graph segment
			delete curr_seg;

			// Delete values from the graph series
			_2DBarGraphSeries* curr_ser = m_Series;
			_2DBarGraphValues* curr_val = NULL;
			_2DBarGraphValues* prev_val = NULL;
			while ( curr_ser != NULL )
			{
				// Delete graph values from the graph
				curr_val = curr_ser->values;
				prev_val = NULL;
				if ( curr_val == NULL )
				{
					// Find graph series values
					while ( curr_val->index != s_index )
					{
						prev_val = curr_val;
						curr_val = curr_val->next;
					}

					// Delete first graph series values
					if ( curr_val == curr_ser->values )
						curr_ser->values = curr_ser->values->next;
					// Delete other graph series values
					else
						prev_val->next = curr_val->next;

					// Delete graph values
					delete curr_val;
				}

				// Get net series
				curr_ser = curr_ser->next;
			}
		}
	}
}

void C2DBarGraph::SetValue(int s_segment, int s_series, double s_value)
{
	// Check segment bounds
	if ( ( s_segment < 1 ) || ( s_segment > m_SegmentsNumber ) )
		AfxMessageBox( "Segments index is out of bounds...", MB_OK, NULL );
	else if ( ( s_series < 1 ) || ( s_series > m_SeriesNumber ) )
		AfxMessageBox( "Series index is out of bounds...", MB_OK, NULL );
	else
	{
		// Find graph series
		_2DBarGraphSeries* curr_ser = m_Series;
		while ( curr_ser->index != s_series )
			curr_ser = curr_ser->next;

		// Find graph series value
		_2DBarGraphValues* curr_val = curr_ser->values;
		while ( curr_val->index != s_segment )
			curr_val = curr_val->next;

		// Set graph series values
		curr_val->value = s_value;
	}
}

void C2DBarGraph::AddSeries(CString s_text, COLORREF s_color)
{
	// Check segments number
	if ( m_SegmentsNumber > 0 )
	{
		// Increment series number
		m_SeriesNumber++;

		// Create new graph series
		_2DBarGraphSeries* newSeries = new _2DBarGraphSeries;
		newSeries->index = m_SeriesNumber;
		newSeries->text = s_text;
		newSeries->color = s_color;
		newSeries->values = NULL;
		newSeries->next = NULL;

		// Add values to new graph series
		_2DBarGraphValues* curr_val = newSeries->values;
		_2DBarGraphValues* newValue = NULL;
		for ( int index=1; index<=m_SegmentsNumber; index++ )
		{
			// Create new graph series values
			newValue = new _2DBarGraphValues;
			newValue->index = index;
			newValue->value = 0;
			newValue->next = NULL;

			// Add new graph values to the graph
			if ( curr_val == NULL )
				newSeries->values = newValue;
			else
				curr_val->next = newValue;
			curr_val = newValue;
		}

		// Add new graph series to the graph
		_2DBarGraphSeries* curr_ser = m_Series;
		if ( curr_ser == NULL )
			m_Series = newSeries;
		else
		{
			while ( curr_ser->next != NULL )
				curr_ser = curr_ser->next;
			curr_ser->next = newSeries;
		}
	}
	else
		AfxMessageBox( "No graph segments...", MB_OK, NULL );
}

void C2DBarGraph::DeleteSeries(int s_index)
{
	// Check series bounds
	if ( ( s_index < 1 ) || ( s_index > m_SeriesNumber ) )
		AfxMessageBox( "Series index is out of bounds...", MB_OK, NULL );
	else
	{
		// Decrement series number
		m_SeriesNumber--;

		// Find graph series
		_2DBarGraphSeries* curr_ser = m_Series;
		_2DBarGraphSeries* prev_ser = NULL;
		while ( curr_ser->index != s_index )
		{
			prev_ser = curr_ser;
			curr_ser = curr_ser->next;
		}

		// Delete graph series values
		_2DBarGraphValues* curr_val = curr_ser->values;
		_2DBarGraphValues* prev_val = NULL;
		while ( curr_val != NULL )
		{
			prev_val = curr_val;
			curr_val = curr_val->next;
			delete prev_val;
		}

		// Delete graph series
		if ( curr_ser == m_Series )
			m_Series = m_Series->next;
		else
			prev_ser->next = curr_ser->next;
		delete curr_ser;
	}
}

void C2DBarGraph::SetGraphAnimation(BOOL g_animation, int a_type)
{
	// Set graph animation parameters
	m_Animation = g_animation;
	m_AnimationType = a_type;
	m_AnimationPercent = 0;
	m_AnimationPause = 0;
	m_AnimationPauseStart = FALSE;

	// Check graph animation flag
	if ( m_Animation == TRUE )
	{
		// If setting draw-all graph animation type
		if ( m_AnimationType == AT_BAR_DRAW_ALL )
		{
			// Set animation series completed
			m_DrawSegmentsCompleted = 0;
		}
		// If setting draw-segment graph animation type
		else if ( m_AnimationType == AT_BAR_DRAW_SEGMENT )
		{
			// Set animation segments completed
			m_DrawSegmentsCompleted = 0;
		}
		// If setting draw-series graph animation type
		else if ( m_AnimationType == AT_BAR_DRAW_SERIES )
		{
			// Set animation series completed
			m_DrawSeriesCompleted = 0;
		}
		// If setting blend-segments graph animation type
		else if ( m_AnimationType == AT_BAR_BLEND_SEGMENT )
		{
			// Set animation segments completed
			m_BlendSegmentsCompleted = 0;
		}
		// If setting blend-series graph animation type
		else if ( m_AnimationType == AT_BAR_BLEND_SERIES )
		{
			// Set animation series completed
			m_BlendSeriesCompleted = 0;
		}
	}
}

void C2DBarGraph::UpdateAnimation()
{
	if ( m_Animation == TRUE )
	{
		// If running draw-all animation
		if ( m_AnimationType == AT_BAR_DRAW_ALL )
		{
			// Check animation percent
			if ( m_AnimationPercent >= 100 )
			{
				if ( m_AnimationPause > 20 )
				{
					// Reset animation percent
					m_AnimationPercent = 0;

					// Reset animation pause
					m_AnimationPause = 0;
					m_AnimationPauseStart = FALSE;
				}
				else
				{
					//  Set animation pause flag
					m_AnimationPauseStart = TRUE;

					// Increment animation pause
					m_AnimationPause++;
				}
			}

			if ( m_AnimationPauseStart == FALSE )
			{
				// Increment animation percent
				m_AnimationPercent++;
			}
		}
		// If running draw-segment animation
		else if ( m_AnimationType == AT_BAR_DRAW_SEGMENT )
		{
			// Check animation percent
			if ( m_AnimationPercent >= 100 )
			{
				// Increment graph segments animated
				m_DrawSegmentsCompleted++;

				// Check segments animated
				if ( m_DrawSegmentsCompleted >= m_SegmentsNumber )
				{
					//  Set animation pause flag
					m_AnimationPauseStart = TRUE;

					// Increment animation pause
					m_AnimationPause++;
				}

				if ( m_AnimationPauseStart == FALSE )
				{
					// Reset animation percent
					m_AnimationPercent = 0;
				}
				else
				{
					if ( m_AnimationPause > 20 )
					{
						// Set animation pause
						m_AnimationPause = 0;
						m_AnimationPauseStart = FALSE;

						// Reset segments animated
						m_DrawSegmentsCompleted = 0;

						// Reset animation percent
						m_AnimationPercent = 0;
					}
				}
			}

			if ( m_AnimationPauseStart == FALSE )
			{
				// Increment animation percent
				m_AnimationPercent+=2;
			}
		}
		// If running draw-series animation
		else if ( m_AnimationType == AT_BAR_DRAW_SERIES )
		{
			// Check animation percent
			if ( m_AnimationPercent >= 100 )
			{
				// Increment graph segments animated
				m_DrawSeriesCompleted++;

				// Check segments animated
				if ( m_DrawSeriesCompleted >= m_SeriesNumber )
				{
					//  Set animation pause flag
					m_AnimationPauseStart = TRUE;

					// Increment animation pause
					m_AnimationPause++;
				}

				if ( m_AnimationPauseStart == FALSE )
				{
					// Reset animation percent
					m_AnimationPercent = 0;
				}
				else
				{
					if ( m_AnimationPause > 20 )
					{
						// Set animation pause
						m_AnimationPause = 0;
						m_AnimationPauseStart = FALSE;

						// Reset segments animated
						m_DrawSeriesCompleted = 0;

						// Reset animation percent
						m_AnimationPercent = 0;
					}
				}
			}

			if ( m_AnimationPauseStart == FALSE )
			{
				// Increment animation percent
				m_AnimationPercent+=2;
			}
		}
		// If running blend-segments animation
		else if ( m_AnimationType == AT_BAR_BLEND_SEGMENT )
		{
			// Check animation percent
			if ( m_AnimationPercent >= 100 )
			{
				// Increment graph segments animated
				m_BlendSegmentsCompleted++;

				// Check segments animated
				if ( m_BlendSegmentsCompleted >= m_SegmentsNumber )
				{
					// Reset segments animated
					m_BlendSegmentsCompleted = 0;
				}

				// Reset animation percent
				m_AnimationPercent = 0;
			}

			// Increment animation percent
			m_AnimationPercent+=2;
		}
		// If running blend-series animation
		else if ( m_AnimationType == AT_BAR_BLEND_SERIES )
		{
			// Check animation percent
			if ( m_AnimationPercent >= 100 )
			{
				// Increment graph segments animated
				m_BlendSeriesCompleted++;

				// Check segments animated
				if ( m_BlendSeriesCompleted >= m_SeriesNumber )
				{
					// Reset segments animated
					m_BlendSeriesCompleted = 0;
				}

				// Reset animation percent
				m_AnimationPercent = 0;
			}

			// Increment animation percent
			m_AnimationPercent+=2;
		}
	}
}

void C2DBarGraph::SetFullSize(CSize full_size)
{
	// Set full size
	m_FullSize = full_size;
}

void C2DBarGraph::SetLabelsColor(COLORREF l_color)
{
	// Set graph labels color
	m_LabelsColor = l_color;
}

COLORREF C2DBarGraph::GetLabelsColor()
{
	return m_LabelsColor;
}

void C2DBarGraph::SetLegendBackgroundColor(COLORREF l_bgcolor)
{
	// Set legend background color
	m_LegendBackgroundColor = l_bgcolor;
}

COLORREF C2DBarGraph::GetLegendBackgroundColor()
{
	return m_LegendBackgroundColor;
}

void C2DBarGraph::SetLegendTextColor(COLORREF t_color)
{
	// Set legend text color
	m_LegendTextColor = t_color;
}

COLORREF C2DBarGraph::GetLegendTextColor()
{
	return m_LegendTextColor;
}
