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

// OpenScrapeView.cpp : implementation of the COpenScrapeView class
//
#include "stdafx.h"
#include "OpenScrape.h"
#include "OpenScrapeDoc.h"
#include "OpenScrapeView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// COpenScrapeView

IMPLEMENT_DYNCREATE(COpenScrapeView, CView)

BEGIN_MESSAGE_MAP(COpenScrapeView, CView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// COpenScrapeView construction/destruction

COpenScrapeView::COpenScrapeView()
{
	black_pen.CreatePen(PS_SOLID, 1, COLOR_BLACK);
	green_pen.CreatePen(PS_SOLID, 1, COLOR_GREEN);
	red_pen.CreatePen(PS_SOLID, 1, COLOR_RED);
	blue_pen.CreatePen(PS_SOLID, 1, COLOR_BLUE);
	white_dot_pen.CreatePen(PS_DOT, 1, COLOR_WHITE);
	black_dot_pen.CreatePen(PS_DOT, 1, COLOR_BLACK);
	null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);

	white_brush.CreateSolidBrush(COLOR_WHITE);
	gray_brush.CreateSolidBrush(COLOR_GRAY);
	red_brush.CreateSolidBrush(COLOR_RED);
	yellow_brush.CreateSolidBrush(COLOR_YELLOW);

	dragging = false;
	dragged_region = "";

	drawing_rect = drawing_started = false;
	hCurDrawRect = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_DRAWRECTCURSOR));
	hCurStandard = ::LoadCursor(NULL, IDC_ARROW);
}

COpenScrapeView::~COpenScrapeView()
{
}

BOOL COpenScrapeView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

// COpenScrapeView drawing

void COpenScrapeView::OnDraw(CDC* pDC)
{
	COpenScrapeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CPen		*pTempPen, oldpen;
	CBrush		*pTempBrush, oldbrush;	
	HDC			hdc = *pDC;
	HDC			hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 
	HDC			hdcCompatible = CreateCompatibleDC(hdcScreen);
	HBITMAP		hbmp = CreateCompatibleBitmap(hdcScreen, 
										      pDoc->attached_rect.right - pDoc->attached_rect.left, 
										      pDoc->attached_rect.bottom - pDoc->attached_rect.top);
	HBITMAP		old_bitmap1, old_bitmap2;
	CMainFrame	*pmyframe = (CMainFrame*)AfxGetMainWnd(); 
	RECT		crect;

	// Draw attached window's bitmap as background
	if (pDoc->attached_bitmap)
	{
		old_bitmap1 = (HBITMAP) SelectObject(hdcCompatible, pDoc->attached_bitmap);
		old_bitmap2 = (HBITMAP) SelectObject(hdc, hbmp);
		BitBlt(hdc, 0, 0,
			   pDoc->attached_rect.right - pDoc->attached_rect.left,
			   pDoc->attached_rect.bottom - pDoc->attached_rect.top,
			   hdcCompatible, 0, 0, SRCCOPY);
		SelectObject(hdc, old_bitmap2);
		SelectObject(hdcCompatible, old_bitmap1);
	}
	else
	{
		pTempPen = (CPen*)pDC->SelectObject(null_pen);
		oldpen.FromHandle((HPEN)pTempPen);
		pTempBrush = (CBrush*)pDC->SelectObject(white_brush);
		oldbrush.FromHandle((HBRUSH)pTempBrush);

		GetClientRect(&crect);
		pDC->Rectangle(&crect);

		pDC->SelectObject(oldpen);
		pDC->SelectObject(oldbrush);
	}

	// Draw all region rectangles
	for (RMapCI r_iter=p_tablemap->r$()->begin(); r_iter!=p_tablemap->r$()->end(); r_iter++)
	{
		if ( (r_iter->second.name==dragged_region && dragging) || 
				(r_iter->second.name==drawrect_region && drawing_rect && drawing_started) )
		{
			// Set pen and brush
			pTempPen = (CPen*)pDC->SelectObject(black_dot_pen);
			oldpen.FromHandle((HPEN)pTempPen);
			pTempBrush = (CBrush*)pDC->SelectObject(GetStockObject(NULL_BRUSH));
			oldbrush.FromHandle((HBRUSH)pTempBrush);

			pDC->Rectangle(r_iter->second.left-1, r_iter->second.top-1, r_iter->second.right+2, r_iter->second.bottom+2);

			pDC->SelectObject(oldpen);
			pDC->SelectObject(oldbrush);
		}
		else
		{
			// Set pen and brush
			pTempPen = (CPen*)pDC->SelectObject(red_pen);
			oldpen.FromHandle((HPEN)pTempPen);
			pTempBrush = (CBrush*)pDC->SelectObject(GetStockObject(NULL_BRUSH));
			oldbrush.FromHandle((HBRUSH)pTempBrush);

			pDC->Rectangle(r_iter->second.left-1, r_iter->second.top-1, r_iter->second.right+2, r_iter->second.bottom+2);

			pDC->SelectObject(oldpen);
			pDC->SelectObject(oldbrush);
		}
	}

	// Clean Up
	DeleteObject(hbmp);
	DeleteDC(hdcCompatible);
	DeleteDC(hdcScreen);
}


// COpenScrapeView diagnostics

#ifdef _DEBUG
void COpenScrapeView::AssertValid() const
{
	CView::AssertValid();
}

void COpenScrapeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

COpenScrapeDoc* COpenScrapeView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(COpenScrapeDoc)));
	return (COpenScrapeDoc*)m_pDocument;
}
#endif //_DEBUG


// COpenScrapeView message handlers

void COpenScrapeView::OnLButtonDown(UINT nFlags, CPoint point)
{
	COpenScrapeDoc		*pDoc = GetDocument();
	CString				sel = theApp.m_TableMapDlg->m_TableMapTree.GetItemText(theApp.m_TableMapDlg->m_TableMapTree.GetSelectedItem());	
	CString				text;

	// If we are drawing a rectangle for a region, handle that
	if (drawing_rect)
	{
		drawrect_start = point;
		drawing_started = true;
		
		RMapI r_iter = p_tablemap->set_r$()->find(sel.GetString());
		
		if (r_iter != p_tablemap->r$()->end())
		{
			drawrect_region = r_iter->second.name;

			// Update internal structure
			r_iter->second.left = point.x;
			r_iter->second.top = point.y;
			r_iter->second.right = point.x;
			r_iter->second.bottom = point.y;

			// Update table map dialog
			text.Format("%d", point.x);
			theApp.m_TableMapDlg->m_Left.SetWindowText(text.GetString());
			theApp.m_TableMapDlg->m_Right.SetWindowText(text.GetString());
			text.Format("%d", point.y);
			theApp.m_TableMapDlg->m_Top.SetWindowText(text.GetString());
			theApp.m_TableMapDlg->m_Bottom.SetWindowText(text.GetString());

			Invalidate(false);
		}
	}

	// Otherwise...
	else
	{
		// Shift click means we want to drag the region
		if (nFlags & MK_SHIFT)
		{
			RMapCI r_iter = p_tablemap->r$()->find(sel.GetString());
			
			if (r_iter != p_tablemap->r$()->end())
			{
				if (point.x >= (LONG) r_iter->second.left-1 &&
					point.x <= (LONG) r_iter->second.right+1 &&
					point.y >= (LONG) r_iter->second.top-1 &&
					point.y <= (LONG) r_iter->second.bottom+1)
				{
					dragging = true;
					dragged_region = r_iter->second.name;
					drag_left_offset = point.x - r_iter->second.left;
					drag_top_offset = point.y - r_iter->second.top;
					Invalidate(false);
				}
			}
		}

		// No shift means just select the region in the tree
		else
		{
			for (RMapCI r_iter=p_tablemap->r$()->begin(); r_iter!=p_tablemap->r$()->end(); r_iter++)
			{
				if (point.x >= (LONG) r_iter->second.left-1 &&
					point.x <= (LONG) r_iter->second.right+1 &&
					point.y >= (LONG) r_iter->second.top-1 &&
					point.y <= (LONG) r_iter->second.bottom+1 &&
					r_iter->second.name != sel)
				{

					// Find parent node
					HTREEITEM parent_node = theApp.m_TableMapDlg->GetTypeNode("Regions");

					// Find correct leaf node
					HTREEITEM item = theApp.m_TableMapDlg->FindItem(r_iter->second.name, parent_node);

					if (item)
						theApp.m_TableMapDlg->m_TableMapTree.SelectItem(item);
				}
			}
		}
	}

	CView::OnLButtonDown(nFlags, point);
}

void COpenScrapeView::OnLButtonUp(UINT nFlags, CPoint point)
{
	COpenScrapeDoc		*pDoc = GetDocument();

	if (drawing_rect)
	{
		drawing_rect = false;
		drawing_started = false;

		RMapI r_iter=p_tablemap->set_r$()->find(drawrect_region.GetString());

		if (r_iter != p_tablemap->r$()->end())
		{
			r_iter->second.left = drawrect_start.x<point.x ? drawrect_start.x : point.x;
			r_iter->second.top = drawrect_start.y<point.y ? drawrect_start.y : point.y;
			r_iter->second.right = drawrect_start.x>=point.x ? drawrect_start.x : point.x;
			r_iter->second.bottom = drawrect_start.y>=point.y ? drawrect_start.y : point.y;

			theApp.m_TableMapDlg->m_DrawRect.OnBnClicked();
			Invalidate(false);
			theApp.m_TableMapDlg->Invalidate(false);
		}
	}


	else if (dragging)
	{
		dragging = false;
		dragged_region = "";
		Invalidate(false);
		theApp.m_TableMapDlg->Invalidate(false);
	}


	CView::OnLButtonUp(nFlags, point);
}

void COpenScrapeView::OnMouseMove(UINT nFlags, CPoint point)
{
	COpenScrapeDoc		*pDoc = GetDocument();
	int					width, height;
	CString				text;

	if (drawing_rect && drawing_started)
	{
		RMapI r_iter=p_tablemap->set_r$()->find(drawrect_region.GetString());

		if (r_iter != p_tablemap->r$()->end())
		{
			// Update internal structure for selected region
			r_iter->second.left = drawrect_start.x<point.x ? drawrect_start.x : point.x;
			r_iter->second.top = drawrect_start.y<point.y ? drawrect_start.y : point.y;
			r_iter->second.right = drawrect_start.x>=point.x ? drawrect_start.x : point.x;
			r_iter->second.bottom = drawrect_start.y>=point.y ? drawrect_start.y : point.y;

			// Update table map dialog
			text.Format("%d", r_iter->second.left);
			theApp.m_TableMapDlg->m_Left.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.top);
			theApp.m_TableMapDlg->m_Top.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.right);
			theApp.m_TableMapDlg->m_Right.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.bottom);
			theApp.m_TableMapDlg->m_Bottom.SetWindowText(text.GetString());

			theApp.m_TableMapDlg->update_display();
			theApp.m_TableMapDlg->Invalidate(false);
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}
	}

	else if (dragging)
	{
		RMapI r_iter=p_tablemap->set_r$()->find(dragged_region.GetString());

		if (r_iter != p_tablemap->r$()->end())
		{
			width = r_iter->second.right - r_iter->second.left;
			height = r_iter->second.bottom - r_iter->second.top;

			// Update internal structure for selected region
			r_iter->second.left = point.x - drag_left_offset;
			r_iter->second.top = point.y - drag_top_offset;
			r_iter->second.right = r_iter->second.left + width;
			r_iter->second.bottom = r_iter->second.top + height;

			// Update table map dialog
			text.Format("%d", r_iter->second.left);
			theApp.m_TableMapDlg->m_Left.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.top);
			theApp.m_TableMapDlg->m_Top.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.right);
			theApp.m_TableMapDlg->m_Right.SetWindowText(text.GetString());
			text.Format("%d", r_iter->second.bottom);
			theApp.m_TableMapDlg->m_Bottom.SetWindowText(text.GetString());

			theApp.m_TableMapDlg->update_display();
			theApp.m_TableMapDlg->Invalidate(false);
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}
	}

	CView::OnMouseMove(nFlags, point);
}

void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	
}


/**
 * handles Keyboard presses, currently used for moving/resizing regions.
 * \param nChar the virtual key pressed down
 */
void COpenScrapeView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(nChar==VK_UP || nChar==VK_DOWN || nChar==VK_LEFT || nChar==VK_RIGHT
		|| nChar==VK_NUMPAD1 || nChar==VK_NUMPAD3 || nChar==VK_NUMPAD7 || nChar==VK_NUMPAD9) {
		// find the currently selected
		CString sel = theApp.m_TableMapDlg->m_TableMapTree.GetItemText(theApp.m_TableMapDlg->m_TableMapTree.GetSelectedItem());	
		RMapI r_iter = p_tablemap->set_r$()->find(sel.GetString());

		if (r_iter != p_tablemap->r$()->end())
		{
			// check what key combinations are down
			bool shiftKeyDown = GetKeyState(VK_SHIFT)>>7 != 0;
			bool sizeChangeKeyDown = GetKeyState(VK_CONTROL)>>7 != 0;

			int speed = 1;
			if(shiftKeyDown) {
				speed = 5;
			} 
			
			if(sizeChangeKeyDown) {
				// change size of region
				if(nChar == VK_UP)	{
					r_iter->second.top = r_iter->second.top-speed;
				} else if(nChar == VK_DOWN)	{
					r_iter->second.top = r_iter->second.top+speed;
				} else if(nChar == VK_LEFT)	{
					r_iter->second.right = r_iter->second.right-speed;
				} else if(nChar == VK_RIGHT)	{
					r_iter->second.right = r_iter->second.right+speed;
				} 
			} else {

				// Default is just to move the selected region
				if(nChar == VK_UP)	{
					r_iter->second.top = r_iter->second.top-speed;
					r_iter->second.bottom = r_iter->second.bottom-speed;
				} else if(nChar == VK_DOWN)	{
					r_iter->second.top = r_iter->second.top+speed;
					r_iter->second.bottom = r_iter->second.bottom+speed;
				} else if(nChar == VK_LEFT)	{
					r_iter->second.left = r_iter->second.left-speed;
					r_iter->second.right = r_iter->second.right-speed;
				} else if(nChar == VK_RIGHT)	{
					r_iter->second.left = r_iter->second.left+speed;
					r_iter->second.right = r_iter->second.right+speed;
				} else if(nChar == VK_NUMPAD1)	{
					r_iter->second.top = r_iter->second.top+speed;
					r_iter->second.bottom = r_iter->second.bottom+speed;
					r_iter->second.left = r_iter->second.left-speed;
					r_iter->second.right = r_iter->second.right-speed;
				} else if(nChar == VK_NUMPAD3)	{
					r_iter->second.top = r_iter->second.top+speed;
					r_iter->second.bottom = r_iter->second.bottom+speed;
					r_iter->second.left = r_iter->second.left+speed;
					r_iter->second.right = r_iter->second.right+speed;
				} else if(nChar == VK_NUMPAD7)	{
					r_iter->second.top = r_iter->second.top-speed;
					r_iter->second.bottom = r_iter->second.bottom-speed;
					r_iter->second.left = r_iter->second.left-speed;
					r_iter->second.right = r_iter->second.right-speed;
				} else if(nChar == VK_NUMPAD9)	{
					r_iter->second.top = r_iter->second.top-speed;
					r_iter->second.bottom = r_iter->second.bottom-speed;
					r_iter->second.left = r_iter->second.left+speed;
					r_iter->second.right = r_iter->second.right+speed;
				} 
			}

			theApp.m_TableMapDlg->update_display();
			theApp.m_TableMapDlg->Invalidate(false);
			Invalidate(false);
			GetDocument()->SetModifiedFlag(true);
		}
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL COpenScrapeView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (drawing_rect)
	{
		::SetCursor(hCurDrawRect);
		return true;
	}

	return CView::OnSetCursor(pWnd, nHitTest, message);
}

void COpenScrapeView::blink_rect(void)
{
	COpenScrapeDoc *pDoc = COpenScrapeDoc::GetDocument();
	CString				 sel = theApp.m_TableMapDlg->m_TableMapTree.GetItemText(theApp.m_TableMapDlg->m_TableMapTree.GetSelectedItem());	
	CDC					   *pDC = GetDC();
	CPen				   *pTempPen, oldpen;
	CBrush				 *pTempBrush, oldbrush;	

  if (dragging) {
    return;
  }
  // Set pen and brush
	pTempPen = (CPen*)pDC->SelectObject(pDoc->blink_on ? green_pen : red_pen);
	oldpen.FromHandle((HPEN)pTempPen);					// Save old pen
	pTempBrush = (CBrush*)pDC->SelectObject(GetStockObject(NULL_BRUSH));
	oldbrush.FromHandle((HBRUSH)pTempBrush);			// Save old brush

	RMapCI r_iter=p_tablemap->r$()->find(sel.GetString());
  if (r_iter != p_tablemap->r$()->end()) {
    pDC->Rectangle(r_iter->second.left - 1, r_iter->second.top - 1, r_iter->second.right + 2, r_iter->second.bottom + 2);
  }
	// Clean up
	pDC->SelectObject(oldpen);
	pDC->SelectObject(oldbrush);
	ReleaseDC(pDC);
}


COpenScrapeView *COpenScrapeView::GetView()
{
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);

	CView * pView = pFrame->GetActiveView();

	if ( !pView )
		return NULL;

	// Fail if view is of wrong kind
	// (this could occur with splitter windows, or additional
	// views on a single document
	if ( ! pView->IsKindOf( RUNTIME_CLASS(COpenScrapeView) ) )
		return NULL;

	return (COpenScrapeView *) pView;
} 

