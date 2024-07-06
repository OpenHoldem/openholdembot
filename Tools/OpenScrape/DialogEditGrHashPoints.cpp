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


// DialogGrHashPoints.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"

#include "OpenScrapeDoc.h"
#include "registry.h"
#include "DialogEditGrHashPoints.h"

// CDlgEditGrHashPoints dialog
CDlgEditGrHashPoints::CDlgEditGrHashPoints(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgEditGrHashPoints::IDD, pParent)
{
	for (int i=0; i<=3; i++)
		working_hash_points[i].RemoveAll();
}

CDlgEditGrHashPoints::~CDlgEditGrHashPoints()
{
}

void CDlgEditGrHashPoints::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HASH_TYPE, m_Hash_Type);
	DDX_Control(pDX, IDC_SAMPLE_IMAGE, m_Sample_Image);
	DDX_Control(pDX, IDC_POINT_LIST, m_Point_List);
	DDX_Control(pDX, IDC_SAMPLE_BITMAP, m_Sample_Bitmap);
	DDX_Control(pDX, IDC_ZOOM_LEVEL, m_Zoom);
	DDX_Control(pDX, IDC_MOUSEPOS, m_MousePos);
}


BEGIN_MESSAGE_MAP(CDlgEditGrHashPoints, CDialog)
	ON_CBN_SELCHANGE(IDC_HASH_TYPE, &CDlgEditGrHashPoints::OnCbnSelchangeHashType)
	ON_CBN_SELCHANGE(IDC_SAMPLE_IMAGE, &CDlgEditGrHashPoints::OnCbnSelchangeSampleImage)
	ON_CBN_SELCHANGE(IDC_ZOOM_LEVEL, &CDlgEditGrHashPoints::OnCbnSelchangeZoomLevel)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()


// CDlgEditGrHashPoints message handlers

BOOL CDlgEditGrHashPoints::OnInitDialog()
{
	COpenScrapeDoc		*pDoc = COpenScrapeDoc::GetDocument();
	int					new_item;
	CString				text;
	Registry			reg;
	int					max_x, max_y;

	CDialog::OnInitDialog();

	// Hash type list
	m_Hash_Type.SetWindowPos(NULL, 0, 0, 86, 200, SWP_NOMOVE | SWP_NOZORDER);
	m_Hash_Type.AddString("Hash1");
	m_Hash_Type.AddString("Hash2");
	m_Hash_Type.AddString("Hash3");

	// Sample image list
	m_Sample_Image.SetWindowPos(NULL, 0, 0, 86, 200, SWP_NOMOVE | SWP_NOZORDER);
	for (IMapCI i_iter=p_tablemap->i$()->begin(); i_iter!=p_tablemap->i$()->end(); i_iter++) 
	{
		text.Format("%s (%dx%d)", i_iter->second.name, i_iter->second.width, i_iter->second.height);
		new_item = m_Sample_Image.AddString(text.GetString());
		m_Sample_Image.SetItemData(new_item, 
			(DWORD_PTR) p_tablemap->CreateI$Index(i_iter->second.name, i_iter->second.width, i_iter->second.height, i_iter->second.pixel));
	}

	// Zoom
	m_Zoom.SetWindowPos(NULL, 0, 0, 86, 200, SWP_NOMOVE | SWP_NOZORDER);
	m_Zoom.AddString("None");
	m_Zoom.AddString("2x");
	m_Zoom.AddString("4x");
	m_Zoom.AddString("8x");
	m_Zoom.AddString("16x");
	m_Zoom.SetCurSel(3);

	// Mouse coords
	m_MousePos.SetWindowText("Mouse: -, -");

	// Restore window location and size
	reg.read_reg();
	max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
	max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);
	::SetWindowPos(m_hWnd, HWND_TOP, min(reg.grhash_x, max_x), min(reg.grhash_y, max_y),
		reg.grhash_dx, reg.grhash_dy, SWP_NOCOPYBITS);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgEditGrHashPoints::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	update_bitmap();
}

void CDlgEditGrHashPoints::OnCbnSelchangeHashType()
{
	CString				text;
	int					type;

	if (m_Hash_Type.GetCurSel() == LB_ERR)
		return;

	m_Hash_Type.GetLBText(m_Hash_Type.GetCurSel(), text);
	type = text.GetString()[4] - '0';

	reset_list_box(type);
	update_bitmap();
}

void CDlgEditGrHashPoints::OnCbnSelchangeSampleImage()
{
	update_bitmap();
}

void CDlgEditGrHashPoints::OnCbnSelchangeZoomLevel()
{
	update_bitmap();
}

void CDlgEditGrHashPoints::update_bitmap() {
	int					x, y, width, height, zoom, type;
	CString			text;
	CDC					*pDC;
	HDC					hdcControl, hdcScreen, hdc_image;
	HBITMAP			bitmap_image, old_bitmap_image, bitmap_control, old_bitmap_control;
	BYTE				*pBits, alpha, red, green, blue;
	IMapCI			sel_image = p_tablemap->i$()->end();
	COLORREF		cr;
	
	// Get pointer to selected image record
	if (m_Sample_Image.GetCurSel() != LB_ERR)
	{
		int index = (int) m_Sample_Image.GetItemData(m_Sample_Image.GetCurSel());
		sel_image = p_tablemap->i$()->find(index);
	}

	if (sel_image != p_tablemap->i$()->end())
	{
		// Get bitmap size
		width = sel_image->second.width;
		height = sel_image->second.height;

		// Copy saved bitmap into a memory dc so we can get the bmi
		pDC = m_Sample_Bitmap.GetDC();
		hdcControl = *pDC;
		hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); 

		// Create new memory DC and new bitmap
		hdc_image = CreateCompatibleDC(hdcScreen);
		bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
		old_bitmap_image = (HBITMAP) SelectObject(hdc_image, bitmap_image);

		// Setup BITMAPINFO
		BITMAPINFO	bmi;
		ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));
		bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
		bmi.bmiHeader.biWidth = width;
		bmi.bmiHeader.biHeight = -height;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biBitCount = 32;
		bmi.bmiHeader.biCompression = BI_RGB; //BI_BITFIELDS;
		bmi.bmiHeader.biSizeImage = width * height * 4;

		// Copy saved image info into pBits array
		pBits = new BYTE[bmi.bmiHeader.biSizeImage];
		for (y=0; y < (int) height; y++) {
			for (x=0; x < (int) width; x++) {
				// image record is stored internally in ABGR format
				alpha = (sel_image->second.pixel[y*width + x] >> 24) & 0xff;
				red = (sel_image->second.pixel[y*width + x] >> 0) & 0xff;
				green = (sel_image->second.pixel[y*width + x] >> 8) & 0xff;
				blue = (sel_image->second.pixel[y*width + x] >> 16) & 0xff;

				// SetDIBits format is BGRA
				pBits[y*width*4 + x*4 + 0] = blue;
				pBits[y*width*4 + x*4 + 1] = green;
				pBits[y*width*4 + x*4 + 2] = red;
				pBits[y*width*4 + x*4 + 3] = alpha; 
			}
		}
		::SetDIBits(hdc_image, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

		// Figure size of stretched bitmap and resize control to fit
		zoom = m_Zoom.GetCurSel()==0 ? 1 :
			   m_Zoom.GetCurSel()==1 ? 2 :
			   m_Zoom.GetCurSel()==2 ? 4 :
			   m_Zoom.GetCurSel()==3 ? 8 :
			   m_Zoom.GetCurSel()==4 ? 16 : 1;

		m_Sample_Bitmap.SetWindowPos(NULL, 0, 0, (width*zoom)+2, (height*zoom)+2, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);

		// Copy from saved bitmap DC copy to our control and stretch it
		bitmap_control = CreateCompatibleBitmap(hdcScreen, width*zoom, height*zoom);
		old_bitmap_control = (HBITMAP) SelectObject(hdcControl, bitmap_control);
		StretchBlt(hdcControl, 1, 1, width*zoom, height*zoom,
				   hdc_image, 0, 0, width, height,
				   SRCCOPY);

		if (m_Hash_Type.GetCurSel() != LB_ERR)
		{
			// Get the seleted hash type
			m_Hash_Type.GetLBText(m_Hash_Type.GetCurSel(), text);
			type = text.GetString()[4] - '0';

			// Draw points on bitmap as circles
			for (int i=0; i<working_hash_points[type].GetSize(); i++)
			{
				unsigned int this_x = working_hash_points[type].GetAt(i).x;
				unsigned int this_y = working_hash_points[type].GetAt(i).y;

				if (this_x <= (unsigned int) width && this_y <= (unsigned int) height)
				{
					// draw in inverted color
					cr = GetPixel(hdc_image, this_x, this_y) ^ 0x00ffffff;

					CPen mypen(PS_SOLID, 1, cr);
					HGDIOBJ oldpen = pDC->SelectObject(mypen);

					CBrush mybrush(cr);
					HGDIOBJ oldbrush = pDC->SelectObject(mybrush);

					pDC->RoundRect(this_x*zoom+1, this_y*zoom+1, (this_x+1)*zoom+1, (this_y+1)*zoom+1, zoom, zoom);
					pDC->SelectObject(oldpen);
					pDC->SelectObject(oldbrush);
					DeleteObject(mypen);
					DeleteObject(mybrush);
				}
			}
		}			


		// Clean up
		delete []pBits;

		SelectObject(hdcControl, old_bitmap_control);
		DeleteObject(bitmap_control);

		SelectObject(hdc_image, old_bitmap_image);
		DeleteObject(bitmap_image);
		DeleteDC(hdc_image);

		DeleteDC(hdcScreen);
		ReleaseDC(pDC);
	}
}

void CDlgEditGrHashPoints::reset_list_box(int type)
{
	CString		text;

	m_Point_List.ResetContent();
	for (int i=0; i<working_hash_points[type].GetSize(); i++)
	{
		text.Format("%2d, %2d", working_hash_points[type].GetAt(i).x, working_hash_points[type].GetAt(i).y);
		m_Point_List.AddString(text.GetString());
	}
}

void CDlgEditGrHashPoints::OnMouseMove(UINT nFlags, CPoint point)
{
	RECT				bmp_rect;
	CString				text;
	int					zoom;

	m_Sample_Bitmap.GetWindowRect(&bmp_rect);
	ScreenToClient(&bmp_rect);

	zoom = m_Zoom.GetCurSel()==0 ? 1 :
	m_Zoom.GetCurSel()==1 ? 2 :
	m_Zoom.GetCurSel()==2 ? 4 :
	m_Zoom.GetCurSel()==3 ? 8 :
	m_Zoom.GetCurSel()==4 ? 16 : 1;

	if (point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
		point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
	{
		text.Format("Mouse: %d, %d", (point.x-bmp_rect.left-1)/zoom, (point.y-bmp_rect.top-1)/zoom);
		m_MousePos.SetWindowText(text.GetString());

	}
	else
	{
		m_MousePos.SetWindowText("Mouse: -, -");
	}

	CDialog::OnMouseMove(nFlags, point);
}

void CDlgEditGrHashPoints::OnLButtonDown(UINT nFlags, CPoint point)
{
	RECT				bmp_rect;
	CString			text;
	int					type, zoom, x, y;
	STablemapHashPoint	temp_hash_point;

	m_Sample_Bitmap.GetWindowRect(&bmp_rect);
	ScreenToClient(&bmp_rect);

	// If the cick was within the bitmap control then...
	if (point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
		point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
	{
		// Get current zoom
		zoom = m_Zoom.GetCurSel()==0 ? 1 :
		m_Zoom.GetCurSel()==1 ? 2 :
		m_Zoom.GetCurSel()==2 ? 4 :
		m_Zoom.GetCurSel()==3 ? 8 :
		m_Zoom.GetCurSel()==4 ? 16 : 1;

		// Get the seleted hash type
		if (m_Hash_Type.GetCurSel() != LB_ERR)
		{
			m_Hash_Type.GetLBText(m_Hash_Type.GetCurSel(), text);
			type = text.GetString()[4] - '0';
		}
		else
		{
			return;
		}

		// Where did we click?
		x = (point.x-bmp_rect.left-1)/zoom;
		y = (point.y-bmp_rect.top-1)/zoom;

		// See if we already have this point
		for (int i=0; i<working_hash_points[type].GetSize(); i++)
		{
			if (working_hash_points[type].GetAt(i).x == x &&
				working_hash_points[type].GetAt(i).y == y)
			{
				return;
			}
		}

		// Add it to the working set
		temp_hash_point.x = x;
		temp_hash_point.y = y;
		working_hash_points[type].Add(temp_hash_point);

		// Update the list box
		reset_list_box(type);

		// Select the one we just added
		text.Format("%2d, %2d", x, y);
		m_Point_List.SelectString(-1, text.GetString());

		// Update display
		update_bitmap();
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgEditGrHashPoints::OnRButtonDown(UINT nFlags, CPoint point)
{
	RECT				bmp_rect;
	CString			text;
	int					type, zoom, x, y, del_index;

	m_Sample_Bitmap.GetWindowRect(&bmp_rect);
	ScreenToClient(&bmp_rect);

	// If the cick was within the bitmap control then...
	if (point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
		point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
	{
		// Get current zoom
		zoom = m_Zoom.GetCurSel()==0 ? 1 :
		m_Zoom.GetCurSel()==1 ? 2 :
		m_Zoom.GetCurSel()==2 ? 4 :
		m_Zoom.GetCurSel()==3 ? 8 :
		m_Zoom.GetCurSel()==4 ? 16 : 1;

		// Get the seleted hash type
		if (m_Hash_Type.GetCurSel() != LB_ERR)
		{
			m_Hash_Type.GetLBText(m_Hash_Type.GetCurSel(), text);
			type = text.GetString()[4] - '0';
		}
		else
		{
			return;
		}

		// Where did we click?
		x = (point.x-bmp_rect.left-1)/zoom;
		y = (point.y-bmp_rect.top-1)/zoom;

		// See if we have this point
		del_index = -1;
		for (int i=0; i<working_hash_points[type].GetSize(); i++)
		{
			if (working_hash_points[type].GetAt(i).x == x &&
				working_hash_points[type].GetAt(i).y == y)
			{
				del_index = i;
			}
		}
		if (del_index==-1)
			return;

		// Remove it from the working set
		working_hash_points[type].RemoveAt(del_index, 1);

		// Update the list box
		reset_list_box(type);

		// Update display
		update_bitmap();
	}

	CDialog::OnRButtonDown(nFlags, point);
}

BOOL CDlgEditGrHashPoints::DestroyWindow()
{
	Registry		reg;
	WINDOWPLACEMENT wp;

	// Save window position
	reg.read_reg();
	GetWindowPlacement(&wp);

	reg.grhash_x = wp.rcNormalPosition.left;
	reg.grhash_y = wp.rcNormalPosition.top;
	reg.grhash_dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
	reg.grhash_dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
	reg.write_reg();

	return CDialog::DestroyWindow();
}
