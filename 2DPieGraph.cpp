// 2DPieGraph.cpp: implementation of the C2DPieGraph class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "2DPieGraph.h"
#include <math.h>

#define PI 3.1415926535897932384626433832795

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2DPieGraph::~C2DPieGraph()
{
	// Delete graph legend font
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = NULL;

	// Free graph blend DC and bitmap
	::SelectObject( m_hBlendDC, m_hOldBlendBitmap );
	::DeleteDC( m_hBlendDC );
	::DeleteObject( m_hBlendBitmap );
}

C2DPieGraph::C2DPieGraph(CPoint g_position, CSize g_size)
{
	// Set graph position and size
	m_Position = g_position;
	m_Size = g_size;

	// Set graph segments number
	m_SegmentsNumber = 0;
	m_Segments = NULL;

	// Set graph legend font
	m_LegendFont = new CFont();
	m_LegendFont->CreateFont( int(m_Size.cy*0.075), 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );

	// Set default graph background color
	m_GraphBkColor = RGB(255,255,2555);

	// Set default graph legend background and text color
	m_LegendBackgroundColor = RGB(255,255,255);
	m_LegendTextColor = RGB(0,0,0);
}

void C2DPieGraph::BuildGraph(HDC hDC)
{
	_2DPieGraphSegments *curr = m_Segments;

	double angle;
	int endX, endY;
	CBrush* pBrush = NULL;
	HGDIOBJ hOldBrush;
	bool animFlag = false;

	m_DrawPercentCompleted = 0;

	// Set start angle
	double startAngle = 0.0;

	// Set start point
	int startX = m_Position.x + m_Size.cx;
	int startY = m_Position.y + m_Size.cy/2;

	// Draw graph segments
	while ( curr != NULL )
	{
		// If animation running
		if ( m_Animation == TRUE )
		{
			// If running draw animation
			if ( m_AnimationType == AT_PIE_DRAW )
			{
				int seg_percent = int((double(m_DrawPercentCompleted+curr->percent)/double(m_DrawTotalPercent)) * 100.0 + 0.5);
				// If drawing complete graph segment
				if ( m_AnimationPercent > seg_percent )
				{
					// Calculate completed animation percent
					m_DrawPercentCompleted += curr->percent;

					// Set graph segment start angle
					angle = startAngle;
					// Calculate graph segment start position
					if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
						startX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
					else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
						startX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
					if ( ( angle >= 0 ) && ( angle <= (PI) ) )
						startY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);
					else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
						startY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);

					// Set graph segment end angle
					angle = startAngle + (double(curr->percent)/100.0) * (2*PI);
					// Calculate graph segment end position
					if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
						endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
					else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
						endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
					if ( ( angle >= 0 ) && ( angle <= (PI) ) )
						endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);
					else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
						endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);

					// Set new animation start point
					m_AnimationStartPoint = CPoint( endX, endY );

					if ( ( startX != endX ) || ( startY != endY ) )
					{
						// Draw graph segment
						pBrush = new CBrush();
						pBrush->CreateSolidBrush( curr->color );
						hOldBrush = ::SelectObject( hDC, pBrush->GetSafeHandle() );
						::Pie( hDC, m_Position.x, m_Position.y, m_Position.x + m_Size.cx, m_Position.y + m_Size.cy, 
							startX, startY, endX, endY );
						::SelectObject( hDC, hOldBrush );
						pBrush->DeleteObject();
						delete pBrush;
					}
				}
				// If drawing part of the graph segment
				else
				{
					// Calculate graph segment animation angle
					if ( ( ( m_DrawAnimationAngle >= 0 ) && ( m_DrawAnimationAngle <= (PI/2) ) ) || ( ( m_DrawAnimationAngle >= (3*PI/2) ) && ( m_DrawAnimationAngle <= (2*PI) ) ) )
						endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(m_DrawAnimationAngle) + 0.5);
					else if ( ( m_DrawAnimationAngle >= (PI/2) ) && ( m_DrawAnimationAngle <= (3*PI/2) ) )
						endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(m_DrawAnimationAngle) + 0.5);
					if ( ( m_DrawAnimationAngle >= 0 ) && ( m_DrawAnimationAngle <= (PI) ) )
						endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(m_DrawAnimationAngle) + 0.5);
					else if ( ( m_DrawAnimationAngle >= (PI) ) && ( m_DrawAnimationAngle <= (2*PI) ) )
						endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(m_DrawAnimationAngle) + 0.5);

					if ( ( m_AnimationStartPoint.x != endX ) || ( m_AnimationStartPoint.y != endY ) )
					{
						// Draw graph segment
						pBrush = new CBrush();
						pBrush->CreateSolidBrush( curr->color );
						hOldBrush = ::SelectObject( hDC, pBrush->GetSafeHandle() );
						::Pie( hDC, (int)m_Position.x, (int)m_Position.y, (int)m_Position.x + (int)m_Size.cx, (int)m_Position.y + (int)m_Size.cy,
							m_AnimationStartPoint.x, m_AnimationStartPoint.y, endX, endY );
						::SelectObject( hDC, hOldBrush );
						pBrush->DeleteObject();
						delete pBrush;
					}

					// Exit animation loop
					break;
				}
			}
			// If running blend animation
			if ( m_AnimationType == AT_PIE_BLEND )
			{
				// Check current graph segment index
				if ( curr->index > m_BlendSegments )
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

					// Set graph segment end angle
					angle = startAngle + (double(curr->percent)/100.0) * (2*PI);
					// Calculate graph segment end position
					if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
						endX = m_Position.x + m_Size.cx/2 + int( (m_Size.cx/2) * cos( angle ) );
					else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
						endX = m_Position.x + m_Size.cx/2 + int( (m_Size.cx/2) * cos( angle ) );
					if ( ( angle >= 0 ) && ( angle <= (PI) ) )
						endY = m_Position.y + m_Size.cy/2 - int( (m_Size.cy/2) * sin( angle ) );
					else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
						endY = m_Position.y + m_Size.cy/2 - int( (m_Size.cy/2) * sin( angle ) );

					// Draw graph segment
					pBrush = new CBrush();
					pBrush->CreateSolidBrush( curr->color );
					hOldBrush = ::SelectObject( m_hBlendDC, pBrush->GetSafeHandle() );
					::Pie( m_hBlendDC, m_Position.x, m_Position.y, m_Position.x + m_Size.cx, m_Position.y + m_Size.cy, 
						m_AnimationStartPoint.x, m_AnimationStartPoint.y, endX, endY );
					::SelectObject( m_hBlendDC, hOldBrush );
					pBrush->DeleteObject();
					delete pBrush;

					// Do alpha blending on temporary DC
					BLENDFUNCTION bf;
					bf.BlendOp = AC_SRC_OVER;
					bf.BlendFlags = 0;
					bf.AlphaFormat = 0;
					bf.SourceConstantAlpha = BYTE((double(m_AnimationPercent)/100.0)*255);
					::AlphaBlend( tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, m_hBlendDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, bf );

					// Do transparent bliting on destination DC
					::TransparentBlt( hDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, tempDC, m_Position.x, m_Position.y, m_Size.cx, m_Size.cy, ::GetPixel(tempDC,m_Position.x,m_Position.y) );

					// Draw finished graph segments
					double angle_total = 0.0;
					_2DPieGraphSegments *tmp_seg = m_Segments;
					while ( tmp_seg->index-1 < m_BlendSegments )
					{
						// Set graph segment start angle
						angle = angle_total;
						// Calculate graph segment start position
						if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
							startX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
						else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
							startX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
						if ( ( angle >= 0 ) && ( angle <= (PI) ) )
							startY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);
						else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
							startY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);

						// Set graph segment end angle
						angle = angle_total + (double(tmp_seg->percent)/100.0) * (2*PI);
						// Calculate graph segment end position
						if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
							endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
						else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
							endX = m_Position.x + int(double(m_Size.cx)/2.0 + (double(m_Size.cx)/2.0) * cos(angle) + 0.5);
						if ( ( angle >= 0 ) && ( angle <= (PI) ) )
							endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);
						else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
							endY = m_Position.y + int(double(m_Size.cy)/2.0 - (double(m_Size.cy)/2.0) * sin(angle) + 0.5);

						// Draw graph segment
						pBrush = new CBrush();
						pBrush->CreateSolidBrush( tmp_seg->color );
						hOldBrush = ::SelectObject( hDC, pBrush->GetSafeHandle() );
						::Pie( hDC, m_Position.x, m_Position.y, m_Position.x + m_Size.cx, m_Position.y + m_Size.cy, 
							startX, startY, endX, endY );
						::SelectObject( hDC, hOldBrush );
						pBrush->DeleteObject();
						delete pBrush;

						// Calculate start angle
						angle_total += (double(tmp_seg->percent)/100.0) * (2*PI);

						// Get next segment
						tmp_seg = tmp_seg->next;
					}

					// Delete graph blend DC and bitmap
					::SelectObject( m_hBlendDC, m_hOldBlendBitmap );
					::DeleteDC( m_hBlendDC );
					::DeleteObject( m_hBlendBitmap );

					// Delete temporary DC and bitmap
					::SelectObject( tempDC, m_tempOldBitmap );
					::DeleteDC( tempDC );
					::DeleteObject( tempBitmap );

					// Set next graph segment start point
					if ( m_AnimationPercent >= 100 )
						m_AnimationStartPoint = CPoint( endX, endY );

					// Exit animation loop
					break;
				}
			}
		}
		// If no animation running
		else
		{
			// Set graph segment end angle
			angle = startAngle + ((curr->percent)/100.0) * (2*PI);
			// Calculate graph segment end position
			if ( ( ( angle >= 0 ) && ( angle <= (PI/2) ) ) || ( ( angle >= (3*PI/2) ) && ( angle <= (2*PI) ) ) )
				endX = m_Position.x + m_Size.cx/2 + int( (m_Size.cx/2) * cos( angle ) );
			else if ( ( angle >= (PI/2) ) && ( angle <= (3*PI/2) ) )
				endX = m_Position.x + m_Size.cx/2 + int( (m_Size.cx/2) * cos( angle ) );
			if ( ( angle >= 0 ) && ( angle <= (PI) ) )
				endY = m_Position.y + m_Size.cy/2 - int( (m_Size.cy/2) * sin( angle ) );
			else if ( ( angle >= (PI) ) && ( angle <= (2*PI) ) )
				endY = m_Position.y + m_Size.cy/2 - int( (m_Size.cy/2) * sin( angle ) );

			// Draw graph segment
			pBrush = new CBrush();
			pBrush->CreateSolidBrush( curr->color );
			hOldBrush = ::SelectObject( hDC, pBrush->GetSafeHandle() );
			::Pie( hDC, m_Position.x, m_Position.y, m_Position.x + m_Size.cx, m_Position.y + m_Size.cy, 
				startX, startY, endX, endY );
			::SelectObject( hDC, hOldBrush );
			pBrush->DeleteObject();
			delete pBrush;

			// Set next graph segment start point
			startX = endX;
			startY = endY;
		}

		// Calculate start angle
		startAngle += ((curr->percent)/100.0) * (2*PI);

		// Next graph segment
		curr = curr->next;
	}
}

void C2DPieGraph::AddSegment(double s_percent, COLORREF s_color, CString s_text)
{
	// Get total percent
	double percentTotal = 0;
	_2DPieGraphSegments* cs = m_Segments;
	while ( cs != NULL )
	{
		percentTotal += cs->percent;

		cs = cs->next;
	}

	// Set total percent
	m_DrawTotalPercent = percentTotal;

	// Check percent bounds
	if ( s_percent < 0 )
		s_percent = 0;
	else if ( s_percent > 100 )
		s_percent = 100;
	else if ( ( percentTotal + s_percent ) > 100 )
		s_percent = 100 - percentTotal;

	// Check percent left
	if ( s_percent > 0 )
	{
		m_DrawTotalPercent += s_percent;

		// Increment segments number
		m_SegmentsNumber++;

		// Create new graph segment
		_2DPieGraphSegments* newSegment = new _2DPieGraphSegments;
		newSegment->percent = s_percent;
		newSegment->color = s_color;
		newSegment->index = m_SegmentsNumber;
		newSegment->text = s_text;
		newSegment->next = NULL;

		// Add new segment to the graph
		_2DPieGraphSegments* curr = m_Segments;
		if ( curr == NULL )
			m_Segments = newSegment;
		else
		{
			while ( curr->next != NULL )
				curr = curr->next;
			curr->next = newSegment;
		}
	}
}

void C2DPieGraph::DeleteSegment(int s_index)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SegmentsNumber ) )
		AfxMessageBox( "Index is out of bounds...", MB_OK, NULL );
	else
	{
		_2DPieGraphSegments *curr, *prev;

		curr = m_Segments;
		prev = NULL;

		// Delete first graph segment
		if ( curr->index == s_index )
		{
			if ( m_SegmentsNumber == 1 )
			{
				// Set segment parameters
				m_Segments = NULL;
				m_SegmentsNumber = 0;
			}
			else
			{
				// Decrement segments number
				m_SegmentsNumber--;
			}

			m_Segments = curr->next;
			delete curr;
		}
		// Delete other graph segment
		else
		{
			while ( curr->index < s_index )
			{
				prev = curr;
				curr = curr->next;
			}

			prev->next = curr->next;
			delete curr;

			// Decrement segments number
			m_SegmentsNumber--;
		}

		// Get total percent
		int percentTotal = 0;
		_2DPieGraphSegments* cs = m_Segments;
		int ind = 1;
		while ( cs != NULL )
		{
			// Reindex graph segments
			cs->index = ind;

			percentTotal += cs->percent;

			cs = cs->next;
			ind++;
		}

		// Set total percent
		m_DrawTotalPercent = percentTotal;
	}
}

void C2DPieGraph::ClearGraph()
{
	_2DPieGraphSegments *curr, *prev;

	// Delete all graph segments
	curr = m_Segments;
	while ( curr != NULL )
	{
		prev = curr;
		curr = curr->next;
		delete prev;
	}

	// Set graph segment parameters
	m_Segments = NULL;
	m_SegmentsNumber = 0;
}

void C2DPieGraph::SetGraphAnimation(BOOL g_animation, int a_type)
{
	// Set graph animation parameters
	m_Animation = g_animation;
	m_AnimationType = a_type;
	m_AnimationPercent = 0;
	m_AnimationStartPoint = CPoint( m_Position.x + m_Size.cx, m_Position.y + m_Size.cy/2 );

	// Check graph animation flag
	if ( m_Animation == TRUE )
	{
		// If setting draw graph animation type
		if ( m_AnimationType == AT_PIE_DRAW )
		{
			m_DrawAnimationAngle = 0.0;
			m_DrawPercentCompleted = 0;
		}
		// If setting blend graph animation type
		else if ( m_AnimationType == AT_PIE_BLEND )
		{
			// Set total blended segments
			m_BlendSegments = 0;
		}
	}
}

BOOL C2DPieGraph::GetGraphAnimation()
{
	return m_Animation;
}

int C2DPieGraph::GetGraphAnimationPercent()
{
	return m_AnimationPercent;
}

void C2DPieGraph::BuildGraphLegend(HDC hDC)
{
	_2DPieGraphSegments *curr = m_Segments;

	// Select legend font
	HGDIOBJ hOldFont = ::SelectObject( hDC, m_LegendFont->GetSafeHandle() );

	// Get text size
	CSize ts;
	::GetTextExtentPoint32( hDC, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26, &ts );

	int offset = (m_Size.cy - int(m_SegmentsNumber*ts.cy));

	CPoint start = CPoint( m_Position.x + int(m_Size.cx*1.15), m_Position.y + int(m_Size.cy*0.014) + offset );
	CSize sz;
	CString str = "";
	CRect rect;
	CBrush* rBrush = NULL;

	// Draw graph legend
	CBrush bgBrush( m_LegendBackgroundColor );
	HBRUSH hOldBrush = (HBRUSH)::SelectObject( hDC, bgBrush.GetSafeHandle() );
	::Rectangle( hDC, m_Position.x + int(m_Size.cx*1.05), m_Position.y + offset - int(m_Size.cy*0.02), m_Position.x + int(m_Size.cx*1.85), m_Position.y + int((m_SegmentsNumber+0.5)*ts.cy + offset) );
	::SelectObject( hDC, hOldBrush );
	bgBrush.DeleteObject();

	COLORREF oldTextColor = ::SetTextColor( hDC, m_LegendTextColor );
	while ( curr != NULL )
	{
		// Draw segment color field
		rect = CRect( start.x - int(m_Size.cx*0.05), start.y + int(m_Size.cy*0.05)/2, start.x + int(m_Size.cx*0.05), start.y + int(m_Size.cy*0.075) );
		rBrush = new CBrush();
		rBrush->CreateSolidBrush( curr->color );
		::FillRect( hDC, rect, (HBRUSH)rBrush->GetSafeHandle() );
		rBrush->DeleteObject();
		delete rBrush;

		// Draw segment text
		str.Format( "(%3.1f%s)  %s", curr->percent, "%", curr->text );
		::SetBkMode( hDC, TRANSPARENT );
		::TextOut( hDC, start.x + rect.Width(), start.y, str, str.GetLength() );
		::SetBkMode( hDC, OPAQUE );

		// Recalculate next text position
		::GetTextExtentPoint32( hDC, curr->text, curr->text.GetLength(), &sz );
		start.y += sz.cy;

		// Get next graph segment
		curr = curr->next;
	}
	::SetTextColor( hDC, oldTextColor );

	// Select old font
	::SelectObject( hDC, hOldFont );
}

void C2DPieGraph::UpdateAnimation()
{
	if ( m_Animation == TRUE )
	{
		// If running draw animation
		if ( m_AnimationType == AT_PIE_DRAW )
		{
			// Set total angle
			double totalAngle = (double(m_DrawTotalPercent)/100.0) * (2*PI);

			// Check completed animation
			if ( ( m_DrawPercentCompleted > m_DrawTotalPercent ) || ( m_AnimationPercent > 100 ) )
			{
				m_DrawPercentCompleted = 0;
				m_AnimationPercent = 0;
				m_AnimationStartPoint = CPoint( m_Position.x + m_Size.cx, m_Position.y + m_Size.cy/2 );
			}

			// Increment animation percent
			m_AnimationPercent++;

			// Set animation angle
			m_DrawAnimationAngle = totalAngle * double(m_AnimationPercent)/100.0;
		}
		// If running blend animation
		else if ( m_AnimationType == AT_PIE_BLEND )
		{
			// Check completed animation percent
			if ( m_AnimationPercent >= 100 )
			{
				m_AnimationPercent = 0;

				// Increment completed graph segments
				m_BlendSegments++;

				// Check completed graph segments
				if ( m_BlendSegments >= m_SegmentsNumber )
				{
					m_BlendSegments = 0;
					m_AnimationStartPoint = CPoint( m_Position.x + m_Size.cx, m_Position.y + m_Size.cy/2 );
				}
			}

			// Increment animation percent
			m_AnimationPercent += 2;
		}
	}
}

void C2DPieGraph::SetGraphBkColor(COLORREF g_bkColor)
{
	// Set graph background color
	m_GraphBkColor = g_bkColor;
}

void C2DPieGraph::UpdateSegment(int s_index, double s_percent, COLORREF s_color, CString s_text)
{
	// Check index bounds
	if ( ( s_index < 1 ) || ( s_index > m_SegmentsNumber ) )
		AfxMessageBox( "Index is out of bounds...", MB_OK, NULL );
	// Check percent
	else if ( s_percent < 0 )
		AfxMessageBox( "Percent is not valid...", MB_OK, NULL );
	else
	{
		_2DPieGraphSegments *curr = m_Segments;

		// Find graph segment
		while ( curr->index != s_index )
			curr = curr->next;

		// Update segment data
		curr->percent = s_percent;
		curr->color = s_color;
		curr->text = s_text;

		// Get total percent
		double percentTotal = 0;
		_2DPieGraphSegments* cs = m_Segments;
		int ind = 1;
		while ( cs != NULL )
		{
			percentTotal += cs->percent;

			cs = cs->next;
			ind++;
		}

		// Set total percent
		m_DrawTotalPercent = percentTotal;

		// Update graph animation parameters
		m_AnimationPercent = 0;
		m_AnimationStartPoint = CPoint( m_Position.x + m_Size.cx, m_Position.y + m_Size.cy/2 );

		// Check graph animation flag
		if ( m_Animation == TRUE )
		{
			// If setting draw graph animation type
			if ( m_AnimationType == AT_PIE_DRAW )
			{
				m_DrawAnimationAngle = 0.0;
				m_DrawPercentCompleted = 0;
			}
			// If setting blend graph animation type
			else if ( m_AnimationType == AT_PIE_BLEND )
			{
				// Set total blended segments
				m_BlendSegments = 0;
			}
		}
	}
}

void C2DPieGraph::SetGraphSize(CSize g_size)
{
	// Set new graph size
	m_Size = g_size;

	// Set new graph legend font size
	m_LegendFont->DeleteObject();
	delete m_LegendFont;
	m_LegendFont = new CFont();
	m_LegendFont->CreateFont( int(m_Size.cy*0.1), int(m_Size.cx*0.04), 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
		DEFAULT_CHARSET, OUT_CHARACTER_PRECIS, CLIP_CHARACTER_PRECIS, PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial" );
}

void C2DPieGraph::SetGraphPosition(CPoint g_position)
{
	// Set new graph position
	m_Position = g_position;
}

void C2DPieGraph::SetFullSize(CSize full_size)
{
	// Set full size
	m_FullSize = full_size;
}

void C2DPieGraph::SetLegendBackgroundColor(COLORREF l_bgcolor)
{
	// Set legend background color
	m_LegendBackgroundColor = l_bgcolor;
}

COLORREF C2DPieGraph::GetLegendBackgroundColor()
{
	return m_LegendBackgroundColor;
}

void C2DPieGraph::SetLegendTextColor(COLORREF t_color)
{
	// Set legend text color
	m_LegendTextColor = t_color;
}

COLORREF C2DPieGraph::GetLegendTextColor()
{
	return m_LegendTextColor;
}
