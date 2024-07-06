////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0. Runs on Win 98 and probably Win 2000 too.
// Set tabsize = 3 in your editor.
//
#include "StdAfx.h"
#include "SizerBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// standard sizing cursors
static HCURSOR hcSizeEW = ::LoadCursor(NULL,(LPCTSTR)IDC_SIZEWE);
static HCURSOR hcSizeNS = ::LoadCursor(NULL,(LPCTSTR)IDC_SIZENS);

IMPLEMENT_DYNCREATE(CSizerBar, CWnd)
BEGIN_MESSAGE_MAP(CSizerBar, CWnd)
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_CANCELMODE()
	ON_WM_CHAR()
END_MESSAGE_MAP()

CSizerBar::CSizerBar()
{
	m_pWinMgr = NULL;
	m_bHorz = -1;			// undetermined; I will compute
	m_bDragging=FALSE;	// not dragging yet
}

CSizerBar::~CSizerBar()
{
}

BOOL CSizerBar::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.style |= SS_NOTIFY; // because static doesn't usually send mouse events
	return CStatic::PreCreateWindow(cs);
}

BOOL CSizerBar::Create(DWORD dwStyle, CWnd* pParentWnd, CWinMgr& wm,
	UINT nID, const RECT& rc)
{
	m_pWinMgr = &wm;
	return CStatic::Create(NULL, dwStyle, rc, pParentWnd, nID);
}

//////////////////
// Determine whether I am horizontal or vertical by looking at dimensions.
// Remember the result for speed.
//
BOOL CSizerBar::IsHorizontal()
{
	if (!m_hWnd)
		return FALSE;	 // not created yet: doesn't matter
	if (m_bHorz!=-1)
		return m_bHorz; // I already figured it out
	
	// If width is greater than height, I must be horizontal. Duh.
	CRect rc;
	GetWindowRect(&rc);
	return m_bHorz = (rc.Width() > rc.Height());
}

//////////////////
// Set cursor to indicate sizing is possible
//
BOOL CSizerBar::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT msg)
{
	::SetCursor(IsHorizontal() ? hcSizeNS : hcSizeEW);
	return TRUE;
}

//////////////////
// User pressed mouse: intialize and enter drag state
//
void CSizerBar::OnLButtonDown(UINT nFlags, CPoint pt)
{
	m_bDragging=TRUE;
	m_ptOriginal = m_ptPrevious = Rectify(pt);
	
	GetWindowRect(&m_rcBar); // bar location in screen coords
	
	DrawBar();					 // draw it
	SetCapture();				 // all mouse messages are MINE
	m_hwndPrevFocus = ::SetFocus(m_hWnd);  // set focus to me to get Escape key

	ASSERT(m_pWinMgr);
	CWinMgr& wm = *m_pWinMgr;

	// get WINRECTs on either side of me
	WINRECT* pwrcSizeBar = wm.FindRect(GetDlgCtrlID());
	ASSERT(pwrcSizeBar);
	WINRECT* prev = pwrcSizeBar->Prev();
	ASSERT(prev);
	WINRECT* next = pwrcSizeBar->Next();
	ASSERT(next);

	// get rectangles on eithr side of me
	CRect rcPrev = prev->GetRect();
	CRect rcNext = next->GetRect();

	// get parent window
	CWnd * pParentWnd = GetParent();
	ASSERT(pParentWnd);

	// Get size info for next/prev rectangles, so I know what the min/max
	// sizes are and don't violate them. Max size never tested.
	SIZEINFO szi;
	wm.OnGetSizeInfo(szi, prev, pParentWnd);
	CRect rcPrevMin(rcPrev.TopLeft(),szi.szMin);
	CRect rcPrevMax(rcPrev.TopLeft(),szi.szMax);

	wm.OnGetSizeInfo(szi, next, pParentWnd);
	CRect rcNextMin(rcNext.BottomRight()-szi.szMin, rcNext.BottomRight());
	CRect rcNextMax(rcNext.BottomRight()-szi.szMax, rcNext.BottomRight());

	// Initialize m_rcConstrain. This is the box the user is allowed to move
	// the sizer bar in. Can't go outside of this--would violate min/max
	// constraints of windows on either side.
	m_rcConstrain.SetRect(
		max(rcPrevMin.right, rcNextMax.left),
		max(rcPrevMin.bottom,rcNextMax.top),
		min(rcPrevMax.right, rcNextMin.left),
		min(rcPrevMax.bottom,rcNextMin.top));

	// convert to my client coords
	pParentWnd->ClientToScreen(&m_rcConstrain);
	ScreenToClient(&m_rcConstrain);

	// Now adjust m_rcConstrain for the fact the bar is not a pure line, but
	// has solid width -- so I have to make a little bigger/smaller according
	// to the offset of mouse coords within the sizer bar rect iteself.
	ClientToScreen(&pt);
	m_rcConstrain.SetRect(m_rcConstrain.TopLeft() + (pt - m_rcBar.TopLeft()),
		m_rcConstrain.BottomRight() - (m_rcBar.BottomRight()-pt));
}

//////////////////
// User moved mouse: erase old bar and draw in new position. XOR makes this
// easy. Keep track of previous point.
//
void CSizerBar::OnMouseMove(UINT nFlags, CPoint pt)
{
	if (m_bDragging) {
		DrawBar();				// erase old bar
		pt = Constrain(pt);	// don't go outside constrained rect!
		pt = Rectify(pt);		// clip x or y depending if horizontal or vert
		CPoint ptDelta = pt-m_ptPrevious;
		m_rcBar += ptDelta;	// move bar...
		DrawBar();				// and draw
		m_ptPrevious = pt;	// remember for next mousemove
	}
}

//////////////////
// User let go of mouse: leave size-drag mode
//
void CSizerBar::OnLButtonUp(UINT nFlags, CPoint pt)
{
	if (m_bDragging) {
		pt = Constrain(pt);					 // don't go outside constraints
		pt = Rectify(pt);						 // clip x or y
		CPoint ptDelta = pt-m_ptOriginal; // distance moved
		CancelDrag();							 // cancel drag mode
		NotifyMoved(ptDelta);				 // notify parent
	}
}

//////////////////
// Cancel drag mode: release capture, erase bar, restore focus
//
void CSizerBar::CancelDrag()
{
	DrawBar();								// erase bar
	ReleaseCapture();						// release mouse
	::SetFocus(m_hwndPrevFocus);		// restore original focus window
	m_bDragging = FALSE;					// stop dragging
}

//////////////////
// Escape key cancels sizing.
//
void CSizerBar::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_ESCAPE && m_bDragging) {
		CancelDrag();
		return;
	}
	CStatic::OnChar(nChar, nRepCnt, nFlags);
}

//////////////////
// Handle WM_CANCELMODE. This is probably unnecessary.
//
void CSizerBar::OnCancelMode()
{
	if (m_bDragging)
		CancelDrag();
}

//////////////////
// Notify parent I moved. It's up to the parent to actually move me by
// calling CWinMgr. I just report how much to move by.
//
void CSizerBar::NotifyMoved(CPoint ptDelta)
{
	NMWINMGR nmr;
	nmr.code = NMWINMGR::SIZEBAR_MOVED;		 // notification subcode
	nmr.idFrom = GetDlgCtrlID();				 // my ID
	nmr.sizebar.ptMoved = ptDelta;			 // distance moved
	GetParent()->SendMessage(WM_WINMGR, nmr.idFrom, (LPARAM)&nmr);
}

//////////////////
// Draw sizer bar. Set up DC, then call virt fn to do it.  Draw on parent
// window's DC to overpaint siblings. Can't use screen DC because that would
// also paint on any WS_EX_TOPMOST windows above me--oops!
//
void CSizerBar::DrawBar()
{
	CWnd* pParentWnd = GetParent();
	CWindowDC dc(pParentWnd);
	CRect rcWin;
	pParentWnd->GetWindowRect(&rcWin);	 // parent window's screen rect
	CRect rc = m_rcBar;						 // bar in screen coords
	rc -= rcWin.TopLeft();					 // convert parent window coords 
	OnDrawBar(dc, rc);						 // calll virtual fn
}

//////////////////
// Draw sizer bar window using XOR op (PATINVERT) so next time will erase.
// YOu can override as long as you use an XOR op.
//
void CSizerBar::OnDrawBar(CDC& dc, CRect& rc)
{
	CBrush brush(GetSysColor(COLOR_3DFACE));
	CBrush* pOldBrush = dc.SelectObject(&brush);
	dc.PatBlt(rc.left, rc.top, rc.Width(), rc.Height(), PATINVERT);
	dc.SelectObject(pOldBrush);
}

//////////////////
// Helper--force point to be inside m_rcConstrain.
// Returns constrained point.
//
CPoint CSizerBar::Constrain(CPoint pt)
{
	if (pt.x<m_rcConstrain.left)
		pt.x = m_rcConstrain.left;
	if (pt.x>m_rcConstrain.right)
		pt.x = m_rcConstrain.right;
	if (pt.y<m_rcConstrain.top)
		pt.y = m_rcConstrain.top;
	if (pt.y>m_rcConstrain.bottom)
		pt.y = m_rcConstrain.bottom;
	return pt;
}
