// ColorPickerCB.h
//


#if !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)
#define AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

void DDX_ColorPickerCB( CDataExchange *pDX, int nIDC, COLORREF& prgbColor );

struct Color
{
	Color( COLORREF crColor, CString strName )
		: m_crColor( crColor )
		, m_strName( strName )
	{};

	COLORREF m_crColor;
	CString m_strName;
};


class CColorPickerCB : public CComboBox
{
// Construction
public:
	CColorPickerCB();
	virtual	~CColorPickerCB();

// Attributes
private:
	CString m_strColorName;

private:
	void Initialize();

public:
	COLORREF GetSelectedColorValue();

	void AddColor( CString strName, COLORREF crColor );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerCB)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );

	// Generated message map functions
protected:
	//{{AFX_MSG(CColorPickerCB)
	//afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERCB_H__C74333B7_A13A_11D1_ADB6_C04D0BC10000__INCLUDED_)
