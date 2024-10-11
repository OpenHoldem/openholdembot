// ColorPickerCB.cpp
//


#include "stdafx.h"
#include "ColorPickerCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Color g_arrColors[] =
{
	Color( RGB( 000, 000, 000 ), "Black" ),
	Color( RGB( 255, 255, 255 ), "White" ),
	Color( RGB( 127, 127, 127 ), "Grey" ),
	Color( RGB( 255, 000, 000 ), "Red" ),
	Color( RGB( 255, 127, 000 ), "Orange" ),
	Color( RGB( 255, 255, 000 ), "Yellow" ),
	Color( RGB( 000, 255, 000 ), "Green" ),
	Color( RGB( 000, 255, 255 ), "Cyan" ),
	Color( RGB( 000, 000, 255 ), "Blue" ),
	Color( RGB( 255, 000, 255 ), "Fuchsia" )
};

CColorPickerCB::CColorPickerCB()
{
}

CColorPickerCB::~CColorPickerCB()
{
}

void DDX_ColorPickerCB( CDataExchange *pDX, int nIDC, COLORREF& rgbColor )
{
	HWND hWndCtrl = pDX->PrepareCtrl( nIDC );
	ASSERT( hWndCtrl );

	CColorPickerCB *pPicker = (CColorPickerCB*)CWnd::FromHandle( hWndCtrl );
	ASSERT( pPicker );

	// only support getting of color.
	if( pDX->m_bSaveAndValidate )
	{
		rgbColor = pPicker->GetSelectedColorValue();
	}
}


BEGIN_MESSAGE_MAP(CColorPickerCB, CComboBox)
	//{{AFX_MSG_MAP(CColorPickerCB)
	//ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorPickerCB message handlers

void CColorPickerCB::PreSubclassWindow() 
{
	CComboBox::PreSubclassWindow();

	Initialize();
}

void CColorPickerCB::Initialize()
{
	// add colors.
	int iNumColors = sizeof( g_arrColors ) / sizeof( g_arrColors[0] );

	for( int iNum = 0; iNum < iNumColors; iNum++ )
	{
		Color& color = g_arrColors[iNum];
		AddColor( color.m_strName, color.m_crColor );
	}

	// add a custom item on the end.
	AddColor( "Custom...", RGB( 255, 255, 255 ) );

	SetCurSel( 0 );
}

void CColorPickerCB::DrawItem( LPDRAWITEMSTRUCT pDIStruct )
{
	CString strColor;
	CDC dcContext;
	CRect rItemRect( pDIStruct->rcItem );
	CRect rBlockRect( rItemRect );
	CRect rTextRect( rBlockRect );
	CBrush brFrameBrush;
	int iFourthWidth = 0;
	int iItem = pDIStruct->itemID;
	int iAction = pDIStruct->itemAction;
	int iState = pDIStruct->itemState;
	COLORREF crColor = NULL;
	COLORREF crNormal = GetSysColor( COLOR_WINDOW );
	COLORREF crSelected = GetSysColor( COLOR_HIGHLIGHT );
	COLORREF crText = GetSysColor( COLOR_WINDOWTEXT );

	if( !dcContext.Attach( pDIStruct->hDC ) )
	{
		return;
	}

	iFourthWidth = ( rBlockRect.Width() / 4 );
	brFrameBrush.CreateStockObject( BLACK_BRUSH );

	if( iState & ODS_SELECTED )
	{
		dcContext.SetTextColor(	( 0x00FFFFFF & ~( crText ) ) );
		dcContext.SetBkColor( crSelected );
		dcContext.FillSolidRect( &rBlockRect, crSelected );
	}
	else
	{
		dcContext.SetTextColor( crText );
		dcContext.SetBkColor( crNormal );
		dcContext.FillSolidRect( &rBlockRect, crNormal );
	}

	if( iState & ODS_FOCUS )
	{
		dcContext.DrawFocusRect( &rItemRect );
	}

	// calculate text area.
	rTextRect.left += ( iFourthWidth + 2 );
	rTextRect.top += 2;

	// calculate color block area.
	rBlockRect.DeflateRect( CSize( 2, 2 ) );
	rBlockRect.right = iFourthWidth;

	// draw color text and block.
	if( iItem != -1 )
	{
		GetLBText( iItem, strColor );

		if( iState & ODS_DISABLED )
		{
			crColor = GetSysColor( COLOR_INACTIVECAPTIONTEXT );
			dcContext.SetTextColor( crColor );
		}
		else
		{
			crColor = GetItemData( iItem );
		}

		dcContext.SetBkMode( TRANSPARENT );
		dcContext.TextOut( rTextRect.left, rTextRect.top,	strColor );

		dcContext.FillSolidRect( &rBlockRect, crColor );
				
		dcContext.FrameRect( &rBlockRect, &brFrameBrush );
	}

	dcContext.Detach();
}

COLORREF CColorPickerCB::GetSelectedColorValue()
{
	return GetItemData( GetCurSel() );
}

void CColorPickerCB::AddColor( CString strName, COLORREF crColor )
{
	int iIndex = AddString( strName );

	if( iIndex >= 0 )
	{
		SetItemData( iIndex, crColor );
	}
}

/*
void CColorPickerCB::OnSelchange() 
{
	// the last item is "Custom...".
	CString strText;
	GetLBText( GetCurSel(), strText );

	if( strText == "Custom..." )
	{
		CColorDialog dlg;

		if( dlg.DoModal() == IDOK )
		{
			SetItemData( GetCurSel(), dlg.GetColor() );
		}
	}
}
*/
