#ifndef INC_WINMGR_H
#define INC_WINMGR_H

////////////////////////////////////////////////////////////////
// MSDN Magazine -- July 2001
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 6.0. Runs on Win 98 and probably Win 2000 too.
// Set tabsize = 3 in your editor.
//
// WinMgr.h -- Main header file for WinMgr library.
//

// when using screen dimensions, this is infinite
const LONG _INFINITY=0x7fff; // max short

// useful size constants
#define SIZEZERO		CSize(0,0)
#define SIZEMAX		CSize(_INFINITY,_INFINITY)

// handy functions to take the min or max of a CSize
inline CSize minsize(CSize a, CSize b) {
	return CSize(min((UINT)a.cx,(UINT)b.cx),min((UINT)a.cy,(UINT)b.cy));
}

inline CSize maxsize(CSize a, CSize b) {
	return CSize(max((UINT)a.cx,(UINT)b.cx),max((UINT)a.cy,(UINT)b.cy));
}

//////////////////
// Size info about a rectangle/row/column
//
struct SIZEINFO {
	SIZE szAvail;		// total size avail (passed)
	SIZE szDesired;	// desired size: default=current
	SIZE szMin;			// minimum size: default=SIZEZERO
	SIZE szMax;			// maximum size: default=MAXSIZE
};

// types of rectangles:
#define	WRCT_END			0				// end of table
#define	WRCT_FIXED		0x0001		// height/width is fixed
#define	WRCT_PCT			0x0002		// height/width is percent of total
#define	WRCT_REST		0x0003		// height/width is whatever remains
#define	WRCT_TOFIT		0x0004		// height/width to fit contents
#define	WRCF_TYPEMASK	0x000F

// group flags
#define	WRCF_ROWGROUP	0x0010		// beginning of row group
#define	WRCF_COLGROUP	0x0020		// beginning of column group
#define	WRCF_ENDGROUP	0x00F0		// end of group
#define	WRCF_GROUPMASK	0x00F0

//////////////////
// This structure is used to hold a rectangle and describe its layout. Each
// WINRECT corresponds to a child rectangle/window. Each window that uses
// WinMgr provides a table (C array) of these to describe its layout.
//
class WINRECT {
protected:
	// pointers initialized by the window manager for easy traversing:
	WINRECT* next;			// next at this level
	WINRECT* prev;			// prev at this level

	// data
	CRect rc;				// current rectangle position/size
	WORD  flags;			// flags (see above)
	UINT	nID;				// window ID if this WINRECT represents a window
	LONG	param;			// arg depends on type

public:
	WINRECT(WORD f, UINT id, LONG p);

	static WINRECT* InitMap(WINRECT* map, WINRECT* parent=NULL);

	WINRECT* Prev()			{ return prev; }
	WINRECT* Next()			{ return next; }
	WINRECT* Children()		{ return IsGroup() ? this+1 : NULL; }
	WINRECT* Parent();
	WORD GetFlags()			{ return flags; }
	WORD SetFlags(WORD f)	{ return flags=f; }
	LONG GetParam()			{ return param; }
	LONG SetParam(LONG p)	{ return param=p; }
	UINT GetID()				{ return nID; }
	UINT SetID(UINT id)		{ return nID=id; }
	CRect& GetRect()					{ return rc; }
	void SetRect(const CRect& r)	{ rc = r; }
	WORD Type() const			{ return flags & WRCF_TYPEMASK; }
	WORD GroupType() const	{ return flags & WRCF_GROUPMASK; }
	BOOL IsGroup() const		{ return GroupType() && GroupType()!=WRCF_ENDGROUP; }
	BOOL IsEndGroup() const { return flags==0 || flags==WRCF_ENDGROUP; }
	BOOL IsEnd() const		{ return flags==0; }
	BOOL IsWindow() const	{ return nID>0; }
	BOOL IsRowGroup()	const { return (flags & WRCF_GROUPMASK)==WRCF_ROWGROUP; }
	void SetHeight(LONG h)	{ rc.bottom = rc.top + h; }
	void SetWidth(LONG w)	{ rc.right = rc.left + w; }
	LONG GetHeightOrWidth(BOOL bHeight) const {
		return bHeight ? rc.Height() : rc.Width();
	}
	void SetHeightOrWidth(LONG horw, BOOL bHeight) {
		bHeight ? SetHeight(horw) : SetWidth(horw);
	}
	BOOL GetMargins(int& w, int& h);

	// For TOFIT types, param is the TOFIT size, if nonzero. Used in dialogs,
	// with CWinMgr::InitToFitSizeFromCurrent.
	BOOL HasToFitSize()			{ return param != 0; }
	SIZE GetToFitSize()			{ return CSize(LOWORD(param),HIWORD(param)); }
	void SetToFitSize(SIZE sz)	{ param = MAKELONG(sz.cx,sz.cy); }
};

//////////////////
// Below are all the macros to build your window map. 
//

// Begin/end window map. 'name' can be anything you want
#define BEGIN_WINDOW_MAP(name)	WINRECT name[] = {
#define END_WINDOW_MAP()			WINRECT(WRCT_END,-1,0) }; 

// Begin/end a group.
// The first entry in your map must be BEGINROWS or BEGINCOLS.
#define BEGINROWS(type,id,m)	WINRECT(WRCF_ROWGROUP|type,id,m),
#define BEGINCOLS(type,id,m)  WINRECT(WRCF_COLGROUP|type,id,m),
#define ENDGROUP()				WINRECT(WRCF_ENDGROUP,-1,0),

// This macros is used only with BEGINGROWS or BEGINCOLS to specify margins
#define RCMARGINS(w,h)			MAKELONG(w,h)

// Macros for primitive (non-group) entries.
// val applies to height for a row entry; width for a column entry.
#define RCFIXED(id,val)		WINRECT(WRCT_FIXED,id,val),
#define RCPERCENT(id,val)	WINRECT(WRCT_PCT,id,val),
#define RCREST(id)			WINRECT(WRCT_REST,id,0),
#define RCTOFIT(id)			WINRECT(WRCT_TOFIT,id,0),
#define RCSPACE(val)			RCFIXED(-1,val)

//////////////////
// Use this to iterate the entries in a group.
//
//	CWinGroupIterator it;
//	for (it=pGroup; it; it.Next()) {
//   WINRECT* wrc = it;
//   ..
// }
//
class CWinGroupIterator {
protected:
	WINRECT* pCur;	  // current entry
public:
	CWinGroupIterator() { pCur = NULL; }
	CWinGroupIterator& operator=(WINRECT* pg) {
		ASSERT(pg->IsGroup()); // can only iterate a group!
		pCur = pg->Children();
		return *this;
	}
	operator WINRECT*()	{ return pCur; }
	WINRECT* pWINRECT()	{ return pCur; }
	WINRECT* Next()		{ return pCur = pCur ? pCur->Next() : NULL;}
};

// Registered WinMgr message
extern const UINT WM_WINMGR;

// Notification struct, passed as LPARAM
struct NMWINMGR : public NMHDR {
	enum {								// notification codes:
		GET_SIZEINFO = 1,				// WinMgr is requesting size info
		SIZEBAR_MOVED,					// user moved sizer bar
	};

	// each notification code has its own part of union
	union {
		SIZEINFO sizeinfo;	// used for GET_SIZEINFO
		struct {					// used for SIZEBAR_MOVED
			POINT ptMoved;		//  distance moved (x or y = zero)
		} sizebar;
	};

	// ctor: initialize to zeroes
	NMWINMGR() { memset(this,0,sizeof(NMWINMGR)); }
};

///////////////////
// Window manager. This class calculates all the sizes and positions of the
// rectangles in the window map.
//
class CWinMgr : public CObject {
public:
	CWinMgr(WINRECT* map);
	virtual ~CWinMgr();

	virtual void GetWindowPositions(CWnd* pWnd); // load map from window posns
	virtual void SetWindowPositions(CWnd* pWnd); // set window posns from map

	// get min/max/desired size of a rectangle
	virtual void OnGetSizeInfo(SIZEINFO& szi, WINRECT* pwrc, CWnd* pWnd=NULL);

	// calc layout using client area as total area
	void CalcLayout(CWnd* pWnd) {
		ASSERT(pWnd);
		CRect rcClient;
		pWnd->GetClientRect(&rcClient);
		CalcLayout(rcClient, pWnd);
	}

	// calc layout using cx, cy (for OnSize)
	void CalcLayout(int xadj, int yadj, int cx, int cy, CWnd* pWnd=NULL) {
		CalcLayout(CRect(xadj,yadj,cx,cy), pWnd);
	}

	// calc layout using given rect as total area
	void CalcLayout(CRect rcTotal, CWnd* pWnd=NULL) {
		ASSERT(m_map);
		m_map->SetRect(rcTotal);
		CalcGroup(m_map, pWnd);
	}

	// Move rectangle vertically or horizontally. Used with sizer bars.
	void MoveRect(int nID, CPoint ptMove, CWnd* pParentWnd) {
		MoveRect(FindRect(nID), ptMove, pParentWnd);
	}
	void MoveRect(WINRECT* pwrcMove, CPoint ptMove, CWnd* pParentWnd);

	CRect GetRect(UINT nID)						 { return FindRect(nID)->GetRect(); }
	void SetRect(UINT nID, const CRect& rc) { FindRect(nID)->SetRect(rc); }

	// get WINRECT corresponding to ID
	WINRECT* FindRect(UINT nID);

	// Calculate MINMAXINFO
	void GetMinMaxInfo(CWnd* pWnd, MINMAXINFO* lpMMI);
	void GetMinMaxInfo(CWnd* pWnd, SIZEINFO& szi);

	// set TOFIT size for all windows from current window sizes
	void InitToFitSizeFromCurrent(CWnd* pWnd);

	void TRACEDump() const { TRACEDump(m_map); }
	void TRACEDump(WINRECT* map) const;

protected:
	WINRECT*	m_map;			// THE window map

	int  CountWindows();
	BOOL SendGetSizeInfo(SIZEINFO& szi, CWnd* pWnd, UINT nID);

	// you can override to do wierd stuff or fix bugs
	virtual void CalcGroup(WINRECT* group, CWnd* pWnd);
	virtual void AdjustSize(WINRECT* pEntry, BOOL bRow,
		int& hwRemaining, CWnd* pWnd);
	virtual void PositionRects(WINRECT* pGroup,
		const CRect& rcTotal,BOOL bRow);

private:
	CWinMgr() { ASSERT(FALSE); } // no default constructor
};


#endif //INC_WINMGR_H