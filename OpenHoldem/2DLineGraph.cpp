// 2DLineGraph.cpp: implementation of the C2DLineGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "2DLineGraph.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DLineGraph::C2DLineGraph(CPoint g_position, CSize g_size)
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
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman" );

	// Set graph series font
	m_SegmentFont = NULL;

	// Set graph value font
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

	// Set default graph draw series points
	m_DrawSeriesPoints = NULL;
}

C2DLineGraph::~C2DLineGraph()
{
	// Delete graph segments and series
	ClearGraph();

	// Delete graph legend font
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = NULL;

	// Delete graph segment font
	m_SegmentFont = NULL;

	// Delete graph value font
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = NULL;

	// Delete series points
	delete []m_DrawSeriesPoints;
}

void C2DLineGraph::ClearGraph()
{
	_2DLineGraphSeries *curr_ser, *prev_ser;
	_2DLineGraphSegments *curr_seg, *prev_seg;
	_2DLineGraphValues *curr_val, *prev_val;

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

void C2DLineGraph::BuildGraph(HDC hDC)
{
	int seg_width, seg_height;
	CPoint ser_start, ser_end, val_start, val_end, seg_font_pos;
	int ser_width, ser_region_width, ser_region_start;
	int max_value;
	double dist, angle;
	int seg_perc;
	bool exit, flag;

	// Set graph step parameters
	int seg_area = int(m_Size.cx*0.65);
	int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
	int ser_step = int(double(seg_step)/8);

	// Set segment font height
	int seg_number = m_SegmentsNumber;
	if ( seg_number == 1 )
		seg_number++;
	int font_height = int((double(seg_area)/double(seg_number))*0.38 + 0.5);

	_2DLineGraphSegments *curr_seg, *next_seg;
	_2DLineGraphSeries *c_ser, *curr_ser;
	_2DLineGraphValues *c_val, *curr_val, *next_val;

	HFONT hOldSegFont, hOldValFont;

	// Calculate graph x and y coordinate system begining
	int startX = m_Position.x + int( m_Size.cx*0.15 );
	int startY = m_Position.y + int( m_Size.cy*0.95 );
	// Draw x coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, m_Size.cx, startY );
	// Draw x coordinate axis arrow
	::MoveToEx( hDC, m_Size.cx, startY, NULL );
	::LineTo( hDC, int(m_Size.cx*0.98), startY-int(m_Size.cy*0.02) );
	::MoveToEx( hDC, m_Size.cx, startY, NULL );
	::LineTo( hDC, int(m_Size.cx*0.98), startY+int(m_Size.cy*0.02) );
	// Draw y coordinate axis
	::MoveToEx( hDC, startX, startY, NULL );
	::LineTo( hDC, startX, m_Position.y + int(m_Size.cy*0.05) );
	// Draw y coordinate axis arrow
	::MoveToEx( hDC, startX, m_Position.y + int(m_Size.cy*0.05), NULL );
	::LineTo( hDC, startX - int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.085) );
	::MoveToEx( hDC, startX, m_Position.y + int(m_Size.cy*0.05), NULL );
	::LineTo( hDC, startX + int(m_Size.cx*0.015), m_Position.y + int(m_Size.cy*0.085) );

	// If animation running
	if ( m_Animation == TRUE )
	{
		// If draw-all animation running
		if ( m_AnimationType == AT_LINE_DRAW_ALL )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 1 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				seg_font_pos = ser_start;
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));
				ser_region_width = int(m_Size.cy*0.8);
				ser_region_start = m_Position.y + int(m_Size.cy*0.93);

				// Get max series value
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment value
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max value
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw graph values text
				hOldValFont = (HFONT)::SelectObject( hDC, m_ValueFont->GetSafeHandle() );
				::SetBkMode( hDC, TRANSPARENT );
				::SetTextAlign( hDC, TA_RIGHT );
				COLORREF oldTxtColor = ::SetTextColor( hDC, m_LabelsColor );
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
				::SetTextColor( hDC, oldTxtColor );

				// Draw grap segments
				exit = false;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					// Draw graph segments text
					COLORREF oldTextColor = ::SetTextColor( hDC, m_LabelsColor );
					// Set new graph legend font
					m_SegmentFont = new CFont();
					m_SegmentFont->CreateFont( font_height, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
						DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
					// Draw segment text
					hOldSegFont = (HFONT)::SelectObject( hDC, m_SegmentFont->GetSafeHandle() );
					::SetBkMode( hDC, TRANSPARENT );
					::SetTextAlign( hDC, TA_CENTER );
					::TextOut( hDC, seg_font_pos.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
					::SetTextAlign( hDC, TA_LEFT );
					::SetBkMode( hDC, OPAQUE );
					::SelectObject( hDC, hOldSegFont );
					seg_font_pos.x += seg_width + seg_step;
					::SetTextColor( hDC, oldTextColor );
					m_SegmentFont->DeleteObject();
					delete m_SegmentFont;

					// Draw graph series
					curr_ser = m_Series;
					curr_val = next_val = NULL;
					while ( curr_ser != NULL )
					{
						// Draw graph series values
						curr_val = curr_ser->values;
						next_val = curr_val->next;

						// Calculate series start point and width
						if ( m_SegmentsNumber > 1 )
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						else
							ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );

						// If animating current segment
						if ( curr_seg->index > m_DrawSegmentsCompleted )
						{
							while ( curr_val->index < curr_seg->index )
								curr_val = curr_val->next;
							next_val = curr_val->next;

							// Check next series value
							while ( next_val != NULL )
							{
								// Calculate value start and end point
								ser_end.x = m_DrawSeriesPoints[curr_ser->index-1].x + seg_width + seg_step;
								if ( curr_val->index == 1 )
								{
									val_start.x = m_DrawSeriesPoints[curr_ser->index-1].x + int(seg_width/2);
									seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
									val_start.y = ser_region_start - seg_height;
								}
								else
									val_start = m_DrawSeriesPoints[curr_ser->index-1];

								if ( curr_val->index == 1 )
									val_end.x = ser_end.x + int(seg_width/2);
								else
									val_end.x = ser_end.x;
								seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
								val_end.y = ser_region_start - seg_height;

								seg_perc = m_AnimationPercent - m_DrawSegmentsCompleted*m_SegmentsPercent;
								dist = (double(seg_perc)/double(m_SegmentsPercent))*sqrt(pow((val_start.x-val_end.x),2)+pow((val_start.y-val_end.y),2));
								angle = atan(double(val_end.y-val_start.y)/double(val_end.x-val_start.x));
								val_end.x = val_start.x + int(dist*cos(angle));
								val_end.y = val_start.y + int(dist*sin(angle));

								// Draw value line
								LOGBRUSH lb;
								lb.lbStyle = BS_SOLID;
								lb.lbColor = curr_ser->color;
								HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
								HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
								::MoveToEx( hDC, val_start.x, val_start.y, NULL );
								::LineTo( hDC, val_end.x, val_end.y );
								::SelectObject( hDC, hOldPen );
								::DeleteObject( hPen );

								// Exit animation loop
								exit = true;
								break;

								// Get next value
								curr_val = next_val;
								next_val = next_val->next;
							}
						}
						// If current segment animation finished
						else
						{
							// Check next series value
							while ( ( next_val != NULL ) && ( (next_val->index-1) <= m_DrawSegmentsCompleted ) )
							{
								// Calculate value start and end point
								ser_end.x = ser_start.x + seg_width + seg_step;
								if ( curr_val->index == 1 )
								{
									val_start.x = ser_start.x + int(seg_width/2);
									seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
									val_start.y = ser_region_start - seg_height;
								}
								else
									val_start = val_end;
								val_end.x = ser_end.x + int(seg_width/2);
								seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
								val_end.y = ser_region_start - seg_height;

								// Draw value line
								LOGBRUSH lb;
								lb.lbStyle = BS_SOLID;
								lb.lbColor = curr_ser->color;
								HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
								HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
								::MoveToEx( hDC, val_start.x, val_start.y, NULL );
								::LineTo( hDC, val_end.x, val_end.y );
								::SelectObject( hDC, hOldPen );
								::DeleteObject( hPen );

								// Set next series start point
								ser_start.x += seg_width + seg_step;

								// Get next value
								curr_val = next_val;
								next_val = next_val->next;
							}

							// Set new series start point
							m_DrawSeriesPoints[curr_ser->index-1] = val_end;
						}

						// Get next series
						curr_ser = curr_ser->next;
					}

					// Exit animation loop
					if ( exit == true )
						break;

					// Get next segment
					curr_seg = curr_seg->next;
				}
			}
		}
		// If draw-series animation running
		else if ( m_AnimationType == AT_LINE_DRAW_SERIES )
		{
			// Check segments number
			if ( ( m_SegmentsNumber > 1 ) && ( m_SeriesNumber > 0 ) )
			{
				// Calculate segments width
				seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

				// Calculate series start point and width
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
				seg_font_pos = ser_start;
				ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));
				ser_region_width = int(m_Size.cy*0.8);
				ser_region_start = m_Position.y + int(m_Size.cy*0.93);

				// Get max series value
				max_value = 0;
				c_ser = m_Series;
				c_val = NULL;
				curr_seg = m_Segments;
				while ( curr_seg != NULL )
				{
					while ( c_ser != NULL )
					{
						// Find segment value
						c_val = c_ser->values;
						while ( c_val->index != curr_seg->index )
							c_val = c_val->next;
						// Check max value
						if ( c_val->value > max_value )
							max_value = c_val->value;

						// Get next series
						c_ser = c_ser->next;
					}

					// Get next segment
					curr_seg = curr_seg->next;
				}

				// Draw graph values text
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

				// Draw graph segments text
				curr_seg = m_Segments;
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
					::TextOut( hDC, seg_font_pos.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
					::SetTextAlign( hDC, TA_LEFT );
					::SetBkMode( hDC, OPAQUE );
					::SelectObject( hDC, hOldSegFont );
					m_SegmentFont->DeleteObject();
					delete m_SegmentFont;

					curr_seg = curr_seg->next;
					seg_font_pos.x += seg_width + seg_step;
				}
				::SetTextColor( hDC, oldTextColor );

				// Draw grap segments
				exit = false;
				flag = false;
				curr_seg = m_Segments;
				// Draw graph series
				curr_ser = m_Series;
				curr_val = next_val = NULL;
				while ( curr_seg != NULL )
				{
					// If animating current series
					if ( curr_ser->index > m_DrawSeriesCompleted )
					{
						// Draw graph series values
						curr_val = curr_ser->values;
						next_val = curr_val->next;

						// Calculate series start point and width
						if ( m_SegmentsNumber > 1 )
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						else
							ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );

						// If animating current segment
						if ( curr_seg->index > m_DrawSegmentsCompleted )
						{
							while ( curr_val->index < curr_seg->index )
								curr_val = curr_val->next;
							next_val = curr_val->next;

							// Check next series value
							while ( next_val != NULL )
							{
								// Calculate value start and end point
								ser_end.x = m_DrawSeriesPoints[curr_ser->index-1].x + seg_width + seg_step;
								if ( curr_val->index == 1 )
								{
									val_start.x = m_DrawSeriesPoints[curr_ser->index-1].x + int(seg_width/2);
									seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
									val_start.y = ser_region_start - seg_height;
								}
								else
									val_start = m_DrawSeriesPoints[curr_ser->index-1];

								if ( curr_val->index == 1 )
									val_end.x = ser_end.x + int(seg_width/2);
								else
									val_end.x = ser_end.x;
								seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
								val_end.y = ser_region_start - seg_height;

								seg_perc = m_AnimationPercent - m_DrawSegmentsCompleted*m_SegmentsPercent;
								dist = (double(seg_perc)/double(m_SegmentsPercent))*sqrt(pow((val_start.x-val_end.x),2)+pow((val_start.y-val_end.y),2));
								angle = atan(double(val_end.y-val_start.y)/double(val_end.x-val_start.x));
								val_end.x = val_start.x + int(dist*cos(angle));
								val_end.y = val_start.y + int(dist*sin(angle));

								// Draw value line
								LOGBRUSH lb;
								lb.lbStyle = BS_SOLID;
								lb.lbColor = curr_ser->color;
								HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
								HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
								::MoveToEx( hDC, val_start.x, val_start.y, NULL );
								::LineTo( hDC, val_end.x, val_end.y );
								::SelectObject( hDC, hOldPen );
								::DeleteObject( hPen );

								// Exit animation loop
								exit = true;
								break;

								// Get next value
								curr_val = next_val;
								next_val = next_val->next;
							}

							flag = true;
						}
						// If current segment animation finished
						else
						{
							// Check next series value
							while ( ( next_val != NULL ) && ( (next_val->index-1) <= m_DrawSegmentsCompleted ) )
							{
								// Calculate value start and end point
								ser_end.x = ser_start.x + seg_width + seg_step;
								if ( curr_val->index == 1 )
								{
									val_start.x = ser_start.x + int(seg_width/2);
									seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
									val_start.y = ser_region_start - seg_height;
								}
								else
									val_start = val_end;
								val_end.x = ser_end.x + int(seg_width/2);
								seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
								val_end.y = ser_region_start - seg_height;

								// Draw value line
								LOGBRUSH lb;
								lb.lbStyle = BS_SOLID;
								lb.lbColor = curr_ser->color;
								HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
								HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
								::MoveToEx( hDC, val_start.x, val_start.y, NULL );
								::LineTo( hDC, val_end.x, val_end.y );
								::SelectObject( hDC, hOldPen );
								::DeleteObject( hPen );

								// Set next series start point
								ser_start.x += seg_width + seg_step;

								// Get next value
								curr_val = next_val;
								next_val = next_val->next;
							}

							// Set new series start point
							m_DrawSeriesPoints[curr_ser->index-1] = val_end;

							flag = false;
						}
					}
					// If current series animation finished
					else
					{
						// Draw graph series values
						curr_val = curr_ser->values;
						next_val = curr_val->next;

						// Calculate series start point and width
						if ( m_SegmentsNumber > 1 )
							ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						else
							ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );

						// Check next series value
						while ( next_val != NULL )
						{
							// Calculate value start and end point
							ser_end.x = ser_start.x + seg_width + seg_step;
							if ( curr_val->index == 1 )
							{
								val_start.x = ser_start.x + int(seg_width/2);
								seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
								val_start.y = ser_region_start - seg_height;
							}
							else
								val_start = val_end;
							val_end.x = ser_end.x + int(seg_width/2);
							seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
							val_end.y = ser_region_start - seg_height;

							// Draw value line
							LOGBRUSH lb;
							lb.lbStyle = BS_SOLID;
							lb.lbColor = curr_ser->color;
							HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
							HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
							::MoveToEx( hDC, val_start.x, val_start.y, NULL );
							::LineTo( hDC, val_end.x, val_end.y );
							::SelectObject( hDC, hOldPen );
							::DeleteObject( hPen );

							// Set next series start point
							ser_start.x += seg_width + seg_step;

							// Get next value
							curr_val = next_val;
							next_val = next_val->next;
						}

						// Get next series
						if ( curr_ser->next != NULL )
							curr_ser = curr_ser->next;
						else
							exit = true;

						flag = true;
					}

					// Exit animation loop
					if ( exit == true )
						break;

					if ( flag == false )
					{
						// Get next segment
						curr_seg = curr_seg->next;
					}
				}
			}
		}
	}
	// If no animation running
	else if ( m_Animation == FALSE )
	{
		// Check segments number
		if ( ( m_SegmentsNumber > 1 ) && ( m_SeriesNumber > 0 ) )
		{
			// Calculate segments width
			seg_width = int((double(seg_area) - (m_SegmentsNumber-1)*seg_step + m_SegmentsNumber*(m_SeriesNumber-1)*ser_step)/double(m_SegmentsNumber));

			// Calculate series start point and width
			if ( m_SegmentsNumber > 1 )
				ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
			else
				ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
			seg_font_pos = ser_start;
			ser_width = int((double(seg_width) - (m_SeriesNumber-1)*ser_step)/double(m_SeriesNumber));
			ser_region_width = int(m_Size.cy*0.8);
			ser_region_start = m_Position.y + int(m_Size.cy*0.93);

			// Get max series value
			max_value = 0;
			c_ser = m_Series;
			c_val = NULL;
			curr_seg = m_Segments;
			while ( curr_seg != NULL )
			{
				while ( c_ser != NULL )
				{
					// Find segment value
					c_val = c_ser->values;
					while ( c_val->index != curr_seg->index )
						c_val = c_val->next;
					// Check max value
					if ( c_val->value > max_value )
						max_value = c_val->value;

					// Get next series
					c_ser = c_ser->next;
				}

				// Get next segment
				curr_seg = curr_seg->next;
			}

			// Draw graph values text
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

			// Draw graph segments text
			curr_seg = m_Segments;
			next_seg = curr_seg->next;
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
				::TextOut( hDC, seg_font_pos.x + int((seg_width + m_SeriesNumber*ser_step)/2), startY+int(m_Size.cy*0.01), curr_seg->text, curr_seg->text.GetLength() );
				::SetTextAlign( hDC, TA_LEFT );
				::SetBkMode( hDC, OPAQUE );
				::SelectObject( hDC, hOldSegFont );
				m_SegmentFont->DeleteObject();
				delete m_SegmentFont;

				seg_font_pos.x += seg_width + seg_step;

				// Get next segment
				curr_seg = curr_seg->next;
			}
			::SetTextColor( hDC, oldTextColor );

			// Draw graph series
			curr_ser = m_Series;
			curr_val = next_val = NULL;
			while ( curr_ser != NULL )
			{
				// Draw graph series values
				curr_val = curr_ser->values;
				next_val = curr_val->next;

				// Set series start point
				if ( m_SegmentsNumber > 1 )
					ser_start = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					ser_start = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );

				// Check next series value
				while ( next_val != NULL )
				{
					// Calculate value start and end point
					ser_end.x = ser_start.x + seg_width + seg_step;
					if ( curr_val->index == 1 )
					{
						val_start.x = ser_start.x + int(seg_width/2);
						seg_height = int((double(curr_val->value)/double(max_value)) * ser_region_width);
						val_start.y = ser_region_start - seg_height;
					}
					else
						val_start = val_end;
					val_end.x = ser_end.x + int(seg_width/2);
					seg_height = int((double(next_val->value)/double(max_value)) * ser_region_width);
					val_end.y = ser_region_start - seg_height;

					// Draw value line
					LOGBRUSH lb;
					lb.lbStyle = BS_SOLID;
					lb.lbColor = curr_ser->color;
					HPEN hPen = ::ExtCreatePen( PS_GEOMETRIC | PS_ENDCAP_ROUND | PS_JOIN_ROUND, 3, &lb, 0, NULL );
					HPEN hOldPen = (HPEN)::SelectObject( hDC, hPen );
					::MoveToEx( hDC, val_start.x, val_start.y, NULL );
					::LineTo( hDC, val_end.x, val_end.y );
					::SelectObject( hDC, hOldPen );
					::DeleteObject( hPen );

					// Set next series start point
					ser_start.x += seg_width + seg_step;

					// Get next value
					curr_val = next_val;
					next_val = next_val->next;
				}

				// Get next series
				curr_ser = curr_ser->next;
			}
		}
	}
}

void C2DLineGraph::BuildGraphLegend(HDC hDC)
{
	_2DLineGraphSeries *curr_ser = m_Series;

	// Select legend font
	HGDIOBJ hOldFont = ::SelectObject( hDC, m_LegendFont->GetSafeHandle() );

	// Get text size
	CSize ts;
	::GetTextExtentPoint32( hDC, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26, &ts );

	int offset = (m_Size.cy - int(m_SeriesNumber*ts.cy))/2;

	CPoint start = CPoint( m_Position.x + int(m_Size.cx*0.97), m_Position.y + int(m_Size.cy*0.014) + offset );
	CSize sz;
	CString str = "";
	CRect rect;
	CBrush* rBrush = NULL;

	// Draw graph legend
	CBrush bgBrush( m_LegendBackgroundColor );
	HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, bgBrush.GetSafeHandle() );
	::Rectangle( hDC, m_Position.x + int(m_Size.cx*0.95), m_Position.y + offset, m_Position.x + int(m_Size.cx*1.25), m_Position.y + int((m_SeriesNumber+0.5)*ts.cy + offset) );
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

BOOL C2DLineGraph::GetGraphAnimation()
{
	return m_Animation;
}

int C2DLineGraph::GetGraphAnimationPercent()
{
	return m_AnimationPercent;
}

void C2DLineGraph::SetGraphBkColor(COLORREF g_bkColor)
{
	// Set graph background color
	m_GraphBkColor = g_bkColor;
}

void C2DLineGraph::SetGraphPosition(CPoint g_position)
{
	// Set new graph position
	m_Position = g_position;
}

void C2DLineGraph::SetGraphSize(CSize g_size)
{
	// Set new graph size
	m_Size = g_size;

	// Set new graph legend font size
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = new CFont();
	m_LegendFont->CreateFont( int(m_Size.cy*0.08), int(m_Size.cx*0.035), 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Times New Roman" );

	// Set new graph value font size
	m_ValueFont->DeleteObject();
	delete m_ValueFont;
	m_ValueFont = new CFont();
	m_ValueFont->CreateFont( int(m_Size.cy*0.1), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
}

void C2DLineGraph::SetFullSize(CSize full_size)
{
	// Set full size
	m_FullSize = full_size;
}

void C2DLineGraph::AddSegment(CString s_text)
{
	// Increment segments number
	m_SegmentsNumber++;

	// Create new graph segment
	_2DLineGraphSegments* newSegment = new _2DLineGraphSegments;
	newSegment->index = m_SegmentsNumber;
	newSegment->text = s_text;
	newSegment->next = NULL;

	// Add new segment to the graph
	_2DLineGraphSegments* curr_seg = m_Segments;
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
		// Create new graph value
		_2DLineGraphValues* newValue = new _2DLineGraphValues;
		newValue->index = m_SegmentsNumber;
		newValue->value = 0;
		newValue->next = NULL;

		// Add new value to the graph segments
		_2DLineGraphSeries* curr_ser = m_Series;
		_2DLineGraphValues* curr_val = NULL;
		while ( curr_ser != NULL )
		{
			// Add new graph value to the graph
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

void C2DLineGraph::DeleteSegment(int s_index)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SegmentsNumber ) )
		AfxMessageBox( "Index is out of bounds...", MB_OK, NULL );
	else
	{
		// Decrement segments number
		m_SegmentsNumber--;

		// Delete segment from the graph
		_2DLineGraphSegments* curr_seg = m_Segments;
		_2DLineGraphSegments* prev_seg = NULL;
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

			// Delete value from the graph series
			_2DLineGraphSeries* curr_ser = m_Series;
			_2DLineGraphValues* curr_val = NULL;
			_2DLineGraphValues* prev_val = NULL;
			while ( curr_ser != NULL )
			{
				// Delete graph value from the graph
				curr_val = curr_ser->values;
				prev_val = NULL;
				if ( curr_val == NULL )
				{
					// Find graph series value
					while ( curr_val->index != s_index )
					{
						prev_val = curr_val;
						curr_val = curr_val->next;
					}

					// Delete first graph series value
					if ( curr_val == curr_ser->values )
						curr_ser->values = curr_ser->values->next;
					// Delete other graph series value
					else
						prev_val->next = curr_val->next;

					// Delete graph value
					delete curr_val;
				}

				// Get net series
				curr_ser = curr_ser->next;
			}
		}
	}
}

void C2DLineGraph::AddSeries(CString s_text, COLORREF s_color)
{
	// Check segments number
	if ( m_SegmentsNumber > 0 )
	{
		// Increment series number
		m_SeriesNumber++;

		// Create new graph series
		_2DLineGraphSeries* newSeries = new _2DLineGraphSeries;
		newSeries->index = m_SeriesNumber;
		newSeries->text = s_text;
		newSeries->color = s_color;
		newSeries->values = NULL;
		newSeries->next = NULL;

		// Add values to new graph series
		_2DLineGraphValues* curr_val = newSeries->values;
		_2DLineGraphValues* newValue = NULL;
		for ( int index=1; index<=m_SegmentsNumber; index++ )
		{
			// Create new graph series value
			newValue = new _2DLineGraphValues;
			newValue->index = index;
			newValue->value = 0;
			newValue->next = NULL;

			// Add new graph value to the graph
			if ( curr_val == NULL )
				newSeries->values = newValue;
			else
				curr_val->next = newValue;
			curr_val = newValue;
		}

		// Add new graph series to the graph
		_2DLineGraphSeries* curr_ser = m_Series;
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

void C2DLineGraph::DeleteSeries(int s_index)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SeriesNumber ) )
		AfxMessageBox( "Series index is out of bounds...", MB_OK, NULL );
	else
	{
		// Decrement series number
		m_SeriesNumber--;

		// Find graph series
		_2DLineGraphSeries* curr_ser = m_Series;
		_2DLineGraphSeries* prev_ser = NULL;
		while ( curr_ser->index != s_index )
		{
			prev_ser = curr_ser;
			curr_ser = curr_ser->next;
		}

		// Delete graph series values
		_2DLineGraphValues* curr_val = curr_ser->values;
		_2DLineGraphValues* prev_val = NULL;
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

void C2DLineGraph::SetValue(int s_segment, int s_series, int s_value)
{
	// Check segment bounds
	if ( ( s_segment < 1 ) || ( s_segment > m_SegmentsNumber ) )
		AfxMessageBox( "Segments index is out of bounds...", MB_OK, NULL );
	else if ( ( s_series < 1 ) || ( s_series > m_SeriesNumber ) )
		AfxMessageBox( "Series index is out of bounds...", MB_OK, NULL );
	else
	{
		// Find graph series
		_2DLineGraphSeries* curr_ser = m_Series;
		while ( curr_ser->index != s_series )
			curr_ser = curr_ser->next;

		// Find graph series value
		_2DLineGraphValues* curr_val = curr_ser->values;
		while ( curr_val->index != s_segment )
			curr_val = curr_val->next;

		// Set graph series value
		curr_val->value = s_value;
	}
}

void C2DLineGraph::SetGraphAnimation(BOOL g_animation, int a_type)
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
		if ( m_AnimationType == AT_LINE_DRAW_ALL )
		{
			// Set animation segments completed
			m_DrawSegmentsCompleted = 0;

			// Set segments percent
			m_SegmentsPercent = int(100.0/double(m_SegmentsNumber));

			// Set graph step parameters
			int seg_area = int(m_Size.cx*0.7);
			int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
			int ser_step = int(double(seg_step)/8);
			// Calculate graph x and y coordinate system begining
			int startX = m_Position.x + int( m_Size.cx*0.15 );
			int startY = m_Position.y + int( m_Size.cy*0.95 );
			// Set draw segments points
			if ( m_DrawSeriesPoints != NULL )
				delete []m_DrawSeriesPoints;
			m_DrawSeriesPoints = new CPoint[m_SeriesNumber];
			for ( int i=0; i<m_SeriesNumber; i++ )
			{
				if ( m_SegmentsNumber > 1 )
					m_DrawSeriesPoints[i] = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					m_DrawSeriesPoints[i] = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
			}
		}
		// If setting draw-series graph animation type
		else if ( m_AnimationType == AT_LINE_DRAW_SERIES )
		{
			// Set animation segments completed
			m_DrawSeriesCompleted = 0;

			// Set animation segments completed
			m_DrawSegmentsCompleted = 0;

			// Set segments percent
			m_SegmentsPercent = int(100.0/double(m_SegmentsNumber));

			// Set graph step parameters
			int seg_area = int(m_Size.cx*0.7);
			int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
			int ser_step = int(double(seg_step)/8);
			// Calculate graph x and y coordinate system begining
			int startX = m_Position.x + int( m_Size.cx*0.15 );
			int startY = m_Position.y + int( m_Size.cy*0.95 );
			// Set draw segments points
			if ( m_DrawSeriesPoints != NULL )
				delete []m_DrawSeriesPoints;
			m_DrawSeriesPoints = new CPoint[m_SeriesNumber];
			for ( int i=0; i<m_SeriesNumber; i++ )
			{
				if ( m_SegmentsNumber > 1 )
					m_DrawSeriesPoints[i] = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
				else
					m_DrawSeriesPoints[i] = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
			}
		}
	}
}

void C2DLineGraph::UpdateAnimation()
{
	if ( m_Animation == TRUE )
	{
		// If running draw-all animation
		if ( m_AnimationType == AT_LINE_DRAW_ALL )
		{
			// Check segments completed
			if ( m_AnimationPercent >= ((m_DrawSegmentsCompleted+1)*m_SegmentsPercent) )
			{
				// Increment segments completed
				m_DrawSegmentsCompleted++;
			}

			// Check animation percent
			if ( m_AnimationPercent >= (m_SegmentsNumber-1)*m_SegmentsPercent )
			{
				if ( m_AnimationPause > 20 )
				{
					// Reset animation percent
					m_AnimationPercent = 0;

					// Reset segments completed
					m_DrawSegmentsCompleted = 0;

					// Reset animation pause
					m_AnimationPause = 0;
					m_AnimationPauseStart = FALSE;

					// Set graph step parameters
					int seg_area = int(m_Size.cx*0.7);
					int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
					int ser_step = int(double(seg_step)/8);
					// Calculate graph x and y coordinate system begining
					int startX = m_Position.x + int( m_Size.cx*0.15 );
					int startY = m_Position.y + int( m_Size.cy*0.95 );
					// Set draw segments points
					for ( int i=0; i<m_SeriesNumber; i++ )
					{
						if ( m_SegmentsNumber > 1 )
							m_DrawSeriesPoints[i] = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
						else
							m_DrawSeriesPoints[i] = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
					}
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
		// If running draw-series animation
		else if ( m_AnimationType == AT_LINE_DRAW_SERIES )
		{
			// Check segments completed
			if ( m_AnimationPercent >= ((m_DrawSegmentsCompleted+1)*m_SegmentsPercent) )
			{
				// Increment segments completed
				m_DrawSegmentsCompleted++;
			}

			// Check animation percent
			if ( m_AnimationPercent >= (m_SegmentsNumber-1)*m_SegmentsPercent )
			{
				// Reset segments completed
				m_DrawSegmentsCompleted = 0;

				// Increment series completed
				m_DrawSeriesCompleted++;
				
				// Check series animated
				if ( m_DrawSeriesCompleted >= m_SeriesNumber )
				{
					if ( m_AnimationPause > 20 )
					{
						// Reset animation percent
						m_AnimationPercent = 0;

						// Reset series completed
						m_DrawSeriesCompleted = 0;

						// Reset animation pause
						m_AnimationPause = 0;
						m_AnimationPauseStart = FALSE;

						// Set graph step parameters
						int seg_area = int(m_Size.cx*0.7);
						int seg_step = int(double(seg_area)/(4*double(m_SegmentsNumber)));
						int ser_step = int(double(seg_step)/8);
						// Calculate graph x and y coordinate system begining
						int startX = m_Position.x + int( m_Size.cx*0.15 );
						int startY = m_Position.y + int( m_Size.cy*0.95 );
						// Set draw segments points
						for ( int i=0; i<m_SeriesNumber; i++ )
						{
							if ( m_SegmentsNumber > 1 )
								m_DrawSeriesPoints[i] = CPoint( startX + ser_step*4, startY - int(m_Size.cy*0.01) );
							else
								m_DrawSeriesPoints[i] = CPoint( startX + ser_step*2, startY - int(m_Size.cy*0.01) );
						}
					}
					else
					{
						//  Set animation pause flag
						m_AnimationPauseStart = TRUE;

						// Increment animation pause
						m_AnimationPause++;
					}
				}
				else
				{
					// Reset animation percent
					m_AnimationPercent = 0;
				}
			}

			if ( m_AnimationPauseStart == FALSE )
			{
				// Increment animation percent
				m_AnimationPercent += 2;
			}
		}
	}
}

void C2DLineGraph::SetLabelsColor(COLORREF l_color)
{
	// Set graph labels color
	m_LabelsColor = l_color;
}

COLORREF C2DLineGraph::GetLabelsColor()
{
	return m_LabelsColor;
}

void C2DLineGraph::SetLegendBackgroundColor(COLORREF l_bgcolor)
{
	// Set legend background color
	m_LegendBackgroundColor = l_bgcolor;
}

COLORREF C2DLineGraph::GetLegendBackgroundColor()
{
	return m_LegendBackgroundColor;
}

void C2DLineGraph::SetLegendTextColor(COLORREF t_color)
{
	// Set legend text color
	m_LegendTextColor = t_color;
}

COLORREF C2DLineGraph::GetLegendTextColor()
{
	return m_LegendTextColor;
}
