﻿//******************************************************************************
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


// DialogTableMap.cpp : implementation file
//
#include "stdafx.h"
#include <assert.h>
#include <math.h>
//#include <regex>
//#include <sstream>
//#include "HTMLparser.h"
//#include "xmlmemory.h"
//#include "parser.h"

#include "DialogTableMap.h"
#include "ListOfSymbols.h"
#include "resource.h"
#include "OpenScrape.h"
#include "OpenScrapeDoc.h"
#include "OpenScrapeView.h"
#include "Registry.h"
#include "MainFrm.h"
#include "DialogEdit.h"
#include "DialogEditSizes.h"
#include "DialogEditSymbols.h"
#include "DialogEditRegion.h"
#include "DialogEditFont.h"
#include "DialogEditHashPoint.h"
#include "DialogEditGrHashPoints.h"

#include "..\CTablemap\CTablemap.h"
#include "..\OpenHoldem\NumericalFunctions.h"


const char* fontsList = "aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ0123456789.,_-$";

const char* cardsList[] = { "2c", "2s", "2h", "2d", "3c", "3s", "3h", "3d", "4c", "4s", "4h", "4d",
	"5c", "5s", "5h", "5d",	"6c", "6s", "6h", "6d", "7c", "7s", "7h", "7d", "8c", "8s", "8h", "8d",
	"9c", "9s", "9h", "9d",	"Tc", "Ts", "Th", "Td", "Jc", "Js", "Jh", "Jd", "Qc", "Qs", "Qh", "Qd",
	"Kc", "Ks", "Kh", "Kd", "Ac", "As", "Ah", "Ad" };

// Auto OCR constants
const int kNumberOfMatchModes = 6;					 // Number of Auto OCR Pdiff template match modes
const char* tplMatchModes[kNumberOfMatchModes] = {   //  Auto OCR Pdiff template match modes
	"TM_SQDIFF", /*!< \f[R(x,y)= \sum _{x',y'} (T(x',y')-I(x+x',y+y'))^2\f]
				 with mask:
				 \f[R(x,y)= \sum _{x',y'} \left( (T(x',y')-I(x+x',y+y')) \cdot
				 M(x',y') \right)^2\f] */
	"TM_SQDIFF_NORMED", /*!< \f[R(x,y)= \frac{\sum_{x',y'} (T(x',y')-I(x+x',y+y'))^2}{\sqrt{\sum_{
						x',y'}T(x',y')^2 \cdot \sum_{x',y'} I(x+x',y+y')^2}}\f]
						with mask:
						\f[R(x,y)= \frac{\sum _{x',y'} \left( (T(x',y')-I(x+x',y+y')) \cdot
						M(x',y') \right)^2}{\sqrt{\sum_{x',y'} \left( T(x',y') \cdot
						M(x',y') \right)^2 \cdot \sum_{x',y'} \left( I(x+x',y+y') \cdot
						M(x',y') \right)^2}}\f] */
	"TM_CCORR", /*!< \f[R(x,y)= \sum _{x',y'} (T(x',y') \cdot I(x+x',y+y'))\f]
				with mask:
				\f[R(x,y)= \sum _{x',y'} (T(x',y') \cdot I(x+x',y+y') \cdot M(x',y')
				^2)\f] */
	"TM_CCORR_NORMED", /*!< \f[R(x,y)= \frac{\sum_{x',y'} (T(x',y') \cdot I(x+x',y+y'))}{\sqrt{
					   \sum_{x',y'}T(x',y')^2 \cdot \sum_{x',y'} I(x+x',y+y')^2}}\f]
					   with mask:
					   \f[R(x,y)= \frac{\sum_{x',y'} (T(x',y') \cdot I(x+x',y+y') \cdot
					   M(x',y')^2)}{\sqrt{\sum_{x',y'} \left( T(x',y') \cdot M(x',y')
					   \right)^2 \cdot \sum_{x',y'} \left( I(x+x',y+y') \cdot M(x',y')
					   \right)^2}}\f] */
	"TM_CCOEFF", /*!< \f[R(x,y)= \sum _{x',y'} (T'(x',y') \cdot I'(x+x',y+y'))\f]
				 where
				 \f[\begin{array}{l} T'(x',y')=T(x',y') - 1/(w \cdot h) \cdot \sum _{
				 x'',y''} T(x'',y'') \\ I'(x+x',y+y')=I(x+x',y+y') - 1/(w \cdot h)
				 \cdot \sum _{x'',y''} I(x+x'',y+y'') \end{array}\f]
				 with mask:
				 \f[\begin{array}{l} T'(x',y')=M(x',y') \cdot \left( T(x',y') -
				 \frac{1}{\sum _{x'',y''} M(x'',y'')} \cdot \sum _{x'',y''}
				 (T(x'',y'') \cdot M(x'',y'')) \right) \\ I'(x+x',y+y')=M(x',y')
				 \cdot \left( I(x+x',y+y') - \frac{1}{\sum _{x'',y''} M(x'',y'')}
				 \cdot \sum _{x'',y''} (I(x+x'',y+y'') \cdot M(x'',y'')) \right)
				 \end{array} \f] */
	"TM_CCOEFF_NORMED"  /*!< \f[R(x,y)= \frac{ \sum_{x',y'} (T'(x',y') \cdot I'(x+x',y+y')) }{
						\sqrt{\sum_{x',y'}T'(x',y')^2 \cdot \sum_{x',y'} I'(x+x',y+y')^2}
						}\f] */
						// Always leave that at the very end
};

// Declare DNN-Recognizer
//TextRecognitionModel recognizer;
TessBaseAPI* api = new TessBaseAPI();
TessBaseAPI* api2 = new TessBaseAPI();


// CDlgTableMap dialog
CDlgTableMap::CDlgTableMap(CWnd* pParent /*=NULL*/) : CDialog(CDlgTableMap::IDD, pParent)
{
	// Create the scroll helper
	// and attach it to this dialog.
	m_scrollHelper = new CScrollHelper;
	m_scrollHelper->AttachWnd(this);

	// We also set the display size
	// equal to the dialog size.
	// This is the size of the dialog
	// in pixels as laid out
	// in the resource editor.
	m_scrollHelper->SetDisplaySize(0, kSizeYForEditor - kYOffsetScroll);


	BuildVectorsOfScraperSymbols();

	black_pen.CreatePen(PS_SOLID, 1, COLOR_BLACK);
	green_pen.CreatePen(PS_SOLID, 1, COLOR_GREEN);
	red_pen.CreatePen(PS_SOLID, 1, COLOR_RED);
	blue_pen.CreatePen(PS_SOLID, 1, COLOR_BLUE);
	white_dot_pen.CreatePen(PS_DOT, 1, COLOR_WHITE);
	null_pen.CreatePen(PS_NULL, 0, COLOR_BLACK);

	white_brush.CreateSolidBrush(COLOR_WHITE);
	lt_gray_brush.CreateSolidBrush(COLOR_SILVER);
	gray_brush.CreateSolidBrush(COLOR_GRAY);
	red_brush.CreateSolidBrush(COLOR_RED);
	yellow_brush.CreateSolidBrush(COLOR_YELLOW);

	separation_font_size = 6;
	lf_fixed.lfWidth = 0;
	lf_fixed.lfHeight = separation_font_size;
	lf_fixed.lfEscapement = 0;
	lf_fixed.lfOrientation = 0;
	lf_fixed.lfItalic = 0;
	lf_fixed.lfUnderline = 0;
	lf_fixed.lfStrikeOut = 0;
	lf_fixed.lfCharSet = 0;
	lf_fixed.lfOutPrecision = 0;
	lf_fixed.lfClipPrecision = 0;
	lf_fixed.lfQuality = PROOF_QUALITY;
	lf_fixed.lfPitchAndFamily = 0;
	strcpy_s(lf_fixed.lfFaceName, 32, "Courier New");

	separation_font.CreateFontIndirect(&lf_fixed);

	nudge_font.CreatePointFont(100, "Wingdings");

	picker_cursor = false;
	hCurPicker = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_PICKERCURSOR));
	hCurStandard = ::LoadCursor(NULL, IDC_ARROW);
}

CDlgTableMap::~CDlgTableMap()
{
	delete m_scrollHelper;

	// Unload network
	api->End();
	api2->End();
	delete api;
	delete api2;
}

void CDlgTableMap::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TABLEMAP_TREE, m_TableMapTree);
	DDX_Control(pDX, IDC_CURRENTREGION, m_BitmapFrame);
	DDX_Control(pDX, IDC_OCR_VIEW, m_MatFrame);
	DDX_Control(pDX, IDC_LEFT, m_Left);
	DDX_Control(pDX, IDC_LEFT_SPIN, m_LeftSpin);
	DDX_Control(pDX, IDC_TOP, m_Top);
	DDX_Control(pDX, IDC_TOP_SPIN, m_TopSpin);
	DDX_Control(pDX, IDC_BOTTOM, m_Bottom);
	DDX_Control(pDX, IDC_BOTTOM_SPIN, m_BottomSpin);
	DDX_Control(pDX, IDC_RIGHT, m_Right);
	DDX_Control(pDX, IDC_RIGHT_SPIN, m_RightSpin);
	DDX_Control(pDX, IDC_TRANSFORM, m_Transform);
	DDX_Control(pDX, IDC_ALPHA, m_Alpha);
	DDX_Control(pDX, IDC_RED, m_Red);
	DDX_Control(pDX, IDC_GREEN, m_Green);
	DDX_Control(pDX, IDC_BLUE, m_Blue);
	DDX_Control(pDX, IDC_RED_AVG, m_RedAvg);
	DDX_Control(pDX, IDC_GREEN_AVG, m_GreenAvg);
	DDX_Control(pDX, IDC_BLUE_AVG, m_BlueAvg);
	DDX_Control(pDX, IDC_PICKER, m_Picker);
	DDX_Control(pDX, IDC_RADIUS, m_Radius);
	DDX_Control(pDX, IDC_RESULT, m_Result);
	DDX_Control(pDX, IDC_NEW, m_New);
	DDX_Control(pDX, IDC_DELETE, m_Delete);
	DDX_Control(pDX, IDC_EDITXY, m_xy);
	DDX_Control(pDX, IDC_EDIT, m_Edit);
	DDX_Control(pDX, IDC_CREATE_IMAGE, m_CreateImage);
	DDX_Control(pDX, IDC_CREATE_FONT, m_CreateFont);
	DDX_Control(pDX, IDC_PIXELSEPARATION, m_PixelSeparation);
	DDX_Control(pDX, IDC_FONTPLUS, m_FontPlus);
	DDX_Control(pDX, IDC_FONTMINUS, m_FontMinus);
	DDX_Control(pDX, IDC_DRAWRECT, m_DrawRect);
	DDX_Control(pDX, IDC_ZOOM, m_Zoom);
	DDX_Control(pDX, IDC_RADIUS_SPIN, m_RadiusSpin);
	DDX_Control(pDX, IDC_NUDGE_TALLER, m_NudgeTaller);
	DDX_Control(pDX, IDC_NUDGE_SHORTER, m_NudgeShorter);
	DDX_Control(pDX, IDC_NUDGE_WIDER, m_NudgeWider);
	DDX_Control(pDX, IDC_NUDGE_NARROWER, m_NudgeNarrower);
	DDX_Control(pDX, IDC_NUDGE_BIGGER, m_NudgeBigger);
	DDX_Control(pDX, IDC_NUDGE_SMALLER, m_NudgeSmaller);
	DDX_Control(pDX, IDC_NUDGE_UPLEFT, m_NudgeUpLeft);
	DDX_Control(pDX, IDC_NUDGE_UP, m_NudgeUp);
	DDX_Control(pDX, IDC_NUDGE_UPRIGHT, m_NudgeUpRight);
	DDX_Control(pDX, IDC_NUDGE_RIGHT, m_NudgeRight);
	DDX_Control(pDX, IDC_NUDGE_DOWNRIGHT, m_NudgeDownRight);
	DDX_Control(pDX, IDC_NUDGE_DOWN, m_NudgeDown);
	DDX_Control(pDX, IDC_NUDGE_DOWNLEFT, m_NudgeDownLeft);
	DDX_Control(pDX, IDC_NUDGE_LEFT, m_NudgeLeft);
	DDX_Control(pDX, IDC_TRACKER_FONT_SET, m_TrackerFontSet);
	DDX_Control(pDX, IDC_TRACKER_FONT_NUM, m_TrackerFontNum);
	DDX_Control(pDX, IDC_TRACKER_CARD_NUM, m_TrackerCardNum);
	DDX_Control(pDX, IDC_MISSING_CARDS, m_status_cards);
	DDX_Control(pDX, IDC_MISSING_FONTS, m_status_fonts);
	DDX_Control(pDX, IDC_CREATE_HASH0, m_CreateHash0);
	DDX_Control(pDX, IDC_CREATE_HASH1, m_CreateHash1);
	DDX_Control(pDX, IDC_CREATE_HASH2, m_CreateHash2);
	DDX_Control(pDX, IDC_CREATE_HASH3, m_CreateHash3);
	DDX_Control(pDX, IDC_IMG_PROC, m_ImgProc);
	DDX_Control(pDX, IDC_USE_DEFAULT, m_UseDefault);
	DDX_Control(pDX, IDC_THRESHOLD, m_Threshold);
	DDX_Text(pDX, IDC_THRESHOLD, threshold);
	DDV_MinMaxInt(pDX, threshold, 0, 300);
	DDX_Control(pDX, IDC_THRESHOLD_SPIN, m_ThresholdSpin);
	DDX_Control(pDX, IDC_MATCH_MODE, m_MatchMode);
	DDX_Control(pDX, IDC_USE_CROP, m_UseCrop);
	DDX_Control(pDX, IDC_CROP_SIZE, m_CropSize);
	DDX_Text(pDX, IDC_CROP_SIZE, crop_size);
	DDV_MinMaxInt(pDX, crop_size, 1, 100);
	DDX_Control(pDX, IDC_CROP_SPIN, m_CropSpin);
	DDX_Control(pDX, IDC_BOX_COLOR, m_BoxColor);
	DDX_ColorPickerCB(pDX, IDC_BOX_COLOR, m_crColor);
}


BEGIN_MESSAGE_MAP(CDlgTableMap, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TABLEMAP_TREE, &CDlgTableMap::OnTvnSelchangedTablemapTree)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_TRANSFORM, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_ALPHA, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_RED, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_GREEN, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_BLUE, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_RADIUS, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_LEFT, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_TOP, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_BOTTOM, &CDlgTableMap::OnRegionChange)
	ON_EN_KILLFOCUS(IDC_RIGHT, &CDlgTableMap::OnRegionChange)
	ON_CBN_SELCHANGE(IDC_ZOOM, &CDlgTableMap::OnZoomChange)
	ON_CBN_SELCHANGE(IDC_TRACKER_FONT_SET, &CDlgTableMap::UpdateStatus)
	ON_CBN_SELCHANGE(IDC_TRACKER_FONT_NUM, &CDlgTableMap::UpdateStatus)
	ON_CBN_SELCHANGE(IDC_TRACKER_CARD_NUM, &CDlgTableMap::UpdateStatus)
	ON_NOTIFY(UDN_DELTAPOS, IDC_LEFT_SPIN, &CDlgTableMap::OnDeltaposLeftSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_TOP_SPIN, &CDlgTableMap::OnDeltaposTopSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_BOTTOM_SPIN, &CDlgTableMap::OnDeltaposBottomSpin)
	ON_NOTIFY(UDN_DELTAPOS, IDC_RIGHT_SPIN, &CDlgTableMap::OnDeltaposRightSpin)

	ON_BN_CLICKED(IDC_NEW, &CDlgTableMap::OnBnClickedNew)
	ON_BN_CLICKED(IDC_DELETE, &CDlgTableMap::OnBnClickedDelete)
	ON_BN_CLICKED(IDC_EDIT, &CDlgTableMap::OnBnClickedEdit)
	ON_BN_CLICKED(IDC_CREATE_IMAGE, &CDlgTableMap::OnBnClickedCreateImage)
	ON_BN_CLICKED(IDC_CREATE_FONT, &CDlgTableMap::OnBnClickedCreateFont)
	ON_BN_CLICKED(IDC_FONTPLUS, &CDlgTableMap::OnBnClickedFontplus)
	ON_BN_CLICKED(IDC_FONTMINUS, &CDlgTableMap::OnBnClickedFontminus)
	ON_BN_CLICKED(IDC_NUDGE_TALLER, &CDlgTableMap::OnBnClickedNudgeTaller)
	ON_BN_CLICKED(IDC_NUDGE_SHORTER, &CDlgTableMap::OnBnClickedNudgeShorter)
	ON_BN_CLICKED(IDC_NUDGE_WIDER, &CDlgTableMap::OnBnClickedNudgeWider)
	ON_BN_CLICKED(IDC_NUDGE_NARROWER, &CDlgTableMap::OnBnClickedNudgeNarrower)
	ON_BN_CLICKED(IDC_NUDGE_BIGGER, &CDlgTableMap::OnBnClickedNudgeBigger)
	ON_BN_CLICKED(IDC_NUDGE_SMALLER, &CDlgTableMap::OnBnClickedNudgeSmaller)
	ON_BN_CLICKED(IDC_NUDGE_UPLEFT, &CDlgTableMap::OnBnClickedNudgeUpleft)
	ON_BN_CLICKED(IDC_NUDGE_UP, &CDlgTableMap::OnBnClickedNudgeUp)
	ON_BN_CLICKED(IDC_NUDGE_UPRIGHT, &CDlgTableMap::OnBnClickedNudgeUpright)
	ON_BN_CLICKED(IDC_NUDGE_RIGHT, &CDlgTableMap::OnBnClickedNudgeRight)
	ON_BN_CLICKED(IDC_NUDGE_DOWNRIGHT, &CDlgTableMap::OnBnClickedNudgeDownright)
	ON_BN_CLICKED(IDC_NUDGE_DOWN, &CDlgTableMap::OnBnClickedNudgeDown)
	ON_BN_CLICKED(IDC_NUDGE_DOWNLEFT, &CDlgTableMap::OnBnClickedNudgeDownleft)
	ON_BN_CLICKED(IDC_NUDGE_LEFT, &CDlgTableMap::OnBnClickedNudgeLeft)

	ON_REGISTERED_MESSAGE(WM_STICKYBUTTONDOWN, OnStickyButtonDown)
	ON_REGISTERED_MESSAGE(WM_STICKYBUTTONUP, OnStickyButtonUp)
	ON_WM_SETCURSOR()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(UDN_DELTAPOS, IDC_RADIUS_SPIN, &CDlgTableMap::OnDeltaposRadiusSpin)
	ON_NOTIFY_EX(TTN_NEEDTEXT, 0, OnToolTipText)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEACTIVATE()
	ON_NOTIFY(TVN_KEYDOWN, IDC_TABLEMAP_TREE, &CDlgTableMap::OnTvnKeydownTablemapTree)
	ON_BN_CLICKED(IDC_CREATE_HASH0, &CDlgTableMap::OnBnClickedCreateHash0)
	ON_BN_CLICKED(IDC_CREATE_HASH1, &CDlgTableMap::OnBnClickedCreateHash1)
	ON_BN_CLICKED(IDC_CREATE_HASH2, &CDlgTableMap::OnBnClickedCreateHash2)
	ON_BN_CLICKED(IDC_CREATE_HASH3, &CDlgTableMap::OnBnClickedCreateHash3)
	ON_BN_CLICKED(IDC_USE_DEFAULT, &CDlgTableMap::OnOcrRegionChange)
	ON_EN_KILLFOCUS(IDC_THRESHOLD, &CDlgTableMap::OnOcrRegionChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_THRESHOLD_SPIN, &CDlgTableMap::OnDeltaposThresholdSpin)
	ON_CBN_SELCHANGE(IDC_MATCH_MODE, &CDlgTableMap::OnOcrRegionChange)
	ON_BN_CLICKED(IDC_USE_CROP, &CDlgTableMap::OnOcrRegionChange)
	ON_EN_KILLFOCUS(IDC_CROP_SIZE, &CDlgTableMap::OnOcrRegionChange)
	ON_NOTIFY(UDN_DELTAPOS, IDC_CROP_SPIN, &CDlgTableMap::OnDeltaposCropSpin)
	ON_CBN_SELCHANGE(IDC_BOX_COLOR, &CDlgTableMap::OnBoxColorChange)
END_MESSAGE_MAP()

// Mandatory value for the spin-buttons.
// Both the default (100) and the former value (1600) are no good.
// http://www.maxinmontreal.com/forums/viewtopic.php?f=338&t=22799
const int kTablemapMaxCoords = 32000;

BOOL CDlgTableMap::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_SIZE)
	{
		if (pMsg->hwnd != this->GetSafeHwnd()) {
			proceed_scroll = false;
			return FALSE;
		}
	}

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		HWND hwndFocus = GetFocus()->GetSafeHwnd();

		if (hwndFocus == m_Left || hwndFocus == m_Right || hwndFocus == m_Top || hwndFocus == m_Bottom || hwndFocus == m_Radius
			|| hwndFocus == m_Alpha || hwndFocus == m_Red || hwndFocus == m_Green || hwndFocus == m_Blue)
			OnRegionChange();

		else if (hwndFocus == m_Threshold || hwndFocus == m_CropSize)
			OnOcrRegionChange();
	}

	proceed_scroll = true;
	return CDialog::PreTranslateInput(pMsg);
}

// CDlgTableMap message handlers
BOOL CDlgTableMap::OnInitDialog()
{
	Registry		reg;
	int				max_x, max_y;
	CString			text;

	CDialog::OnInitDialog();

	create_tree();

	// Setup text entry fields and spinners
	m_Left.SetWindowText("0");
	m_LeftSpin.SetRange(0, kTablemapMaxCoords);
	m_LeftSpin.SetPos(0);
	m_LeftSpin.SetBuddy(&m_Left);

	m_Top.SetWindowText("0");
	m_TopSpin.SetRange(0, kTablemapMaxCoords);
	m_TopSpin.SetPos(0);
	m_TopSpin.SetBuddy(&m_Top);

	m_Right.SetWindowText("0");
	m_RightSpin.SetRange(0, kTablemapMaxCoords);
	m_RightSpin.SetPos(0);
	m_RightSpin.SetBuddy(&m_Right);

	m_Bottom.SetWindowText("0");
	m_BottomSpin.SetRange(0, kTablemapMaxCoords);
	m_BottomSpin.SetPos(0);
	m_BottomSpin.SetBuddy(&m_Bottom);

	// Set bitmap on front of draw rect button
	drawrect_bitmap.LoadBitmap(IDB_DRAWRECTBITMAP);
	h_drawrect_bitmap = (HBITMAP)drawrect_bitmap.GetSafeHandle();
	m_DrawRect.SetBitmap(h_drawrect_bitmap);

	m_Transform.AddString("Color");
	m_Transform.AddString("Image");
	m_Transform.AddString("AutoOcr0");
	m_Transform.AddString("AutoOcr1");
	m_Transform.AddString("Text0");
	m_Transform.AddString("Text1");
	m_Transform.AddString("Text2");
	m_Transform.AddString("Text3");
	m_Transform.AddString("Text4");
	m_Transform.AddString("Text5");
	m_Transform.AddString("Text6");
	m_Transform.AddString("Text7");
	m_Transform.AddString("Text8");
	m_Transform.AddString("Text9");
	m_Transform.AddString("Hash0");
	m_Transform.AddString("Hash1");
	m_Transform.AddString("Hash2");
	m_Transform.AddString("Hash3");
	m_Transform.AddString("WebColour");
	m_Transform.AddString("None");
	m_Transform.SetWindowPos(NULL, 0, 0, 72, 200, SWP_NOMOVE | SWP_NOZORDER);

	m_TrackerFontSet.SetCurSel(0);
	m_TrackerFontNum.SetCurSel(1);
	m_TrackerCardNum.SetCurSel(1);

	m_Alpha.SetWindowText("");
	m_Red.SetWindowText("");
	m_Green.SetWindowText("");
	m_Blue.SetWindowText("");
	m_RedAvg.SetWindowText("");
	m_GreenAvg.SetWindowText("");
	m_BlueAvg.SetWindowText("");

	m_Radius.SetWindowText("");
	m_RadiusSpin.SetRange(-441, 441);
	m_RadiusSpin.SetPos(0);
	m_RadiusSpin.SetBuddy(&m_Radius);

	m_Result.SetWindowText("");

	m_PixelSeparation.SetFont(&separation_font);


	//m_ImgProc.SetWindowText("Inbuilt image processing");
	m_UseDefault.SetCheck(true);
	threshold = 0;
	m_Threshold.SetWindowText(to_string(threshold).c_str());
	m_ThresholdSpin.SetRange(0, 300);
	m_ThresholdSpin.SetPos(0);
	m_ThresholdSpin.SetBuddy(&m_Threshold);
	match_mode = -1;
	for (int i = 0; i < kNumberOfMatchModes; i++)
		m_MatchMode.AddString(tplMatchModes[i]);
	m_MatchMode.SetCurSel(match_mode);

	m_UseCrop.SetCheck(false);
	crop_size = 0;
	m_CropSize.SetWindowText(to_string(crop_size).c_str());
	m_BoxColor.SetWindowPos(NULL, 0, 0, 72, 200, SWP_NOMOVE | SWP_NOZORDER);
	m_CropSpin.SetRange(2, 100);
	m_CropSpin.SetPos(0);
	m_CropSpin.SetBuddy(&m_CropSize);
	box_color = -1;
	m_BoxColor.SetCurSel(box_color);

	m_Zoom.AddString("None");
	m_Zoom.AddString("2x");
	m_Zoom.AddString("4x");
	m_Zoom.AddString("8x");
	m_Zoom.AddString("16x");
	m_Zoom.SetCurSel(2);
	m_Zoom.SetWindowPos(NULL, 0, 0, 72, 200, SWP_NOMOVE | SWP_NOZORDER);

	m_xy.SetWindowPos(NULL, 0, 0, 60, 12, SWP_NOMOVE | SWP_NOZORDER);

	// Set bitmap on front of picker button
	picker_bitmap.LoadBitmap(IDB_PICKERBITMAP);
	h_picker_bitmap = (HBITMAP)picker_bitmap.GetSafeHandle();
	m_Picker.SetBitmap(h_picker_bitmap);

	// Set text on nudge buttons
	m_NudgeTaller.SetFont(&nudge_font);
	m_NudgeShorter.SetFont(&nudge_font);
	m_NudgeWider.SetFont(&nudge_font);
	m_NudgeNarrower.SetFont(&nudge_font);
	m_NudgeBigger.SetFont(&nudge_font);
	m_NudgeSmaller.SetFont(&nudge_font);
	m_NudgeUpLeft.SetFont(&nudge_font);
	m_NudgeUp.SetFont(&nudge_font);
	m_NudgeUpRight.SetFont(&nudge_font);
	m_NudgeRight.SetFont(&nudge_font);
	m_NudgeDownRight.SetFont(&nudge_font);
	m_NudgeDown.SetFont(&nudge_font);
	m_NudgeDownLeft.SetFont(&nudge_font);
	m_NudgeLeft.SetFont(&nudge_font);

	text.Format("%c", 0xD9);
	m_NudgeTaller.SetWindowText(text.GetString());
	text.Format("%c", 0xDA);
	m_NudgeShorter.SetWindowText(text.GetString());
	text.Format("%c", 0xD8);
	m_NudgeWider.SetWindowText(text.GetString());
	text.Format("%c", 0xD7);
	m_NudgeNarrower.SetWindowText(text.GetString());
	text.Format("%c", 0xB2);
	m_NudgeBigger.SetWindowText(text.GetString());
	text.Format("%c", 0xB3);
	m_NudgeSmaller.SetWindowText(text.GetString());
	text.Format("%c", 0xE3);
	m_NudgeUpLeft.SetWindowText(text.GetString());
	text.Format("%c", 0xE1);
	m_NudgeUp.SetWindowText(text.GetString());
	text.Format("%c", 0xE4);
	m_NudgeUpRight.SetWindowText(text.GetString());
	text.Format("%c", 0xE0);
	m_NudgeRight.SetWindowText(text.GetString());
	text.Format("%c", 0xE6);
	m_NudgeDownRight.SetWindowText(text.GetString());
	text.Format("%c", 0xE2);
	m_NudgeDown.SetWindowText(text.GetString());
	text.Format("%c", 0xE5);
	m_NudgeDownLeft.SetWindowText(text.GetString());
	text.Format("%c", 0xDF);
	m_NudgeLeft.SetWindowText(text.GetString());

	// Restore window location and size
	reg.read_reg();
	max_x = GetSystemMetrics(SM_CXSCREEN) - GetSystemMetrics(SM_CXICON);
	max_y = GetSystemMetrics(SM_CYSCREEN) - GetSystemMetrics(SM_CYICON);
	::SetWindowPos(m_hWnd, HWND_TOP, min(reg.tablemap_x, max_x), min(reg.tablemap_y, max_y),
		reg.tablemap_dx, reg.tablemap_dy, SWP_NOCOPYBITS);

	// Region grouping
	region_grouping = reg.region_grouping;

	//  Display missing cards and fonts
	UpdateStatus();

	picker_cursor = false;
	ignore_changes = false;

	EnableToolTips(true);

	// New automatic OCR based on tesseract-ocr
	// Load Tesseract text recognition network
	if (api->Init("tessdata", "eng") == -1) {		// OEM_LSTM_ONLY
		MessageBox("Failed to load tessdata files.\nMake sure tessdata folder is present and/or datas are not corrupted.", "AutoOcr error", MB_OK);
		return FALSE;
	}
	if (api2->Init("tessdata", "eng") == -1) {		// OEM_LSTM_ONLY
		MessageBox("Failed to load tessdata files.\nMake sure tessdata folder is present and/or datas are not corrupted.", "AutoOcr error", MB_OK);
		return FALSE;
	}
	//api->SetPageSegMode(PSM_SINGLE_LINE);
	//api->SetVariable("user_defined_dpi", "300");

	// OpenCV DNN text recognition
	/*
	String recModelPath = "crnn_cs.onnx";
	CV_Assert(!recModelPath.empty());
	recognizer = readNet(recModelPath);
	// Load vocabulary
	String vocPath = "alphabet_94.txt";
	CV_Assert(!vocPath.empty());
	ifstream vocFile;
	vocFile.open(samples::findFile(vocPath));
	CV_Assert(vocFile.is_open());
	String vocLine;
	vector<String> vocabulary;
	while (getline(vocFile, vocLine)) {
	vocabulary.push_back(vocLine);
	}
	recognizer.setVocabulary(vocabulary);
	recognizer.setDecodeType("CTC-greedy");
	// Parameters for Recognition
	double recScale = 1.0 / 127.5;
	Scalar recMean = Scalar(127.5, 127.5, 127.5);
	Size recInputSize = Size(100, 32);
	recognizer.setInputParams(recScale, recInputSize, recMean);
	*/

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgTableMap::DestroyWindow()
{
	Registry		reg;
	WINDOWPLACEMENT wp;

	// Save window position
	reg.read_reg();
	GetWindowPlacement(&wp);

	reg.tablemap_x = wp.rcNormalPosition.left;
	reg.tablemap_y = wp.rcNormalPosition.top;
	reg.tablemap_dx = wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1;
	reg.tablemap_dy = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top + 1;
	reg.write_reg();

	return CDialog::DestroyWindow();
}

void CDlgTableMap::OnOK()
{
	// prevents enter key from closing dialog
	//CDialog::OnOK();
}

void CDlgTableMap::OnCancel()
{
	// prevents esc key from closing dialog
	//CDialog::OnCancel();
}

void CDlgTableMap::UpdateDisplayOfAllRegions()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	update_r$_display(false);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);
}

HTREEITEM CDlgTableMap::GetRecordTypeNode(HTREEITEM item)
{
	if (m_TableMapTree.GetParentItem(item) == NULL)
	{
		return NULL;
	}

	else
	{
		HTREEITEM parent = m_TableMapTree.GetParentItem(item);

		while (m_TableMapTree.GetParentItem(parent) != NULL)
			parent = m_TableMapTree.GetParentItem(parent);

		return parent;
	}
}

// Gets the text of the selected item and the selected item's record type
// Parameters:
//   sel_text: set to the text of the selected item
//   type_text: set to the text of the selected item's record type
// Returns: the node of the selected item's record type (String, Symbol, Image, etc), 
//          or NULL if the record type node is already selected
HTREEITEM CDlgTableMap::GetTextSelItemAndRecordType(CString* sel_text, CString* type_text)
{
	sel_text->Empty();
	type_text->Empty();

	if (!m_TableMapTree.GetSelectedItem())
		return NULL;

	*(sel_text) = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());
	HTREEITEM record_type_node = GetRecordTypeNode(m_TableMapTree.GetSelectedItem());

	if (record_type_node == NULL)
		*(type_text) = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());
	else
		*(type_text) = m_TableMapTree.GetItemText(record_type_node);

	return record_type_node;
}


void CDlgTableMap::OnPaint()
{
	CPaintDC			dc(this);

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// A root item was selected, or nothing has been selected yet
	if (type_node == NULL || m_TableMapTree.GetSelectedItem() == NULL)
	{
		clear_bitmap_control();
		clear_mat_control();
		m_BitmapFrame.SetWindowPos(NULL, 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
		m_MatFrame.SetWindowPos(NULL, 0, 0, MAT_WIDTH, MAT_HEIGHT, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
	}

	// A non root item was selected
	else
	{

		// Display currently selected region bitmap from saved bitmap
		if (type_text == "Regions")
		{
			draw_region_bitmap();
		}

		else if (type_text == "Templates")
		{
			TPLMapCI			sel_template = p_tablemap->tpl$()->end();
			CString				sel = "";

			// Get name of currently selected item
			if (m_TableMapTree.GetSelectedItem())
				sel = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());

			// Get iterator for selected region
			sel_template = p_tablemap->tpl$()->find(sel.GetString());

			// Exit if we can't find the region
			if (sel_template == p_tablemap->tpl$()->end())
				return;

			if (sel_template->second.created)
				draw_template_bitmap();
			else
				draw_region_bitmap();
		}

		else if (type_text == "Images")
		{
			draw_image_bitmap();
		}

		else
		{
			m_BitmapFrame.SetBitmap(NULL);
			m_BitmapFrame.SetWindowPos(NULL, 0, 0, BITMAP_WIDTH, BITMAP_HEIGHT, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
			m_MatFrame.SetBitmap(NULL);
			m_MatFrame.SetWindowPos(NULL, 0, 0, MAT_WIDTH, MAT_HEIGHT, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
		}
	}

	// Do not call CDialog::OnPaint() for painting messages
}

void CDlgTableMap::clear_bitmap_control(void)
{
	CDC* pDC = m_BitmapFrame.GetDC();
	CPen* pTempPen, oldpen;
	CBrush* pTempBrush, oldbrush;
	RECT			rect;

	pTempPen = (CPen*)pDC->SelectObject(&null_pen);
	oldpen.FromHandle((HPEN)pTempPen);
	pTempBrush = (CBrush*)pDC->SelectObject(&lt_gray_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);
	m_BitmapFrame.GetWindowRect(&rect);

	pDC->Rectangle(1, 1, rect.right - rect.left, rect.bottom - rect.top);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);
	ReleaseDC(pDC);
}

void CDlgTableMap::clear_mat_control(void)
{
	CDC* pDC = m_MatFrame.GetDC();
	CPen* pTempPen, oldpen;
	CBrush* pTempBrush, oldbrush;
	RECT			rect;

	pTempPen = (CPen*)pDC->SelectObject(&null_pen);
	oldpen.FromHandle((HPEN)pTempPen);
	pTempBrush = (CBrush*)pDC->SelectObject(&lt_gray_brush);
	oldbrush.FromHandle((HBRUSH)pTempBrush);
	m_MatFrame.GetWindowRect(&rect);

	pDC->Rectangle(1, 1, rect.right - rect.left, rect.bottom - rect.top);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);
	ReleaseDC(pDC);
}

void CDlgTableMap::draw_region_bitmap(void)
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CDC* pDC;
	HDC					hdcControl, hdcScreen;
	HDC					hdc_bitmap_orig, hdc_bitmap_subset;
	HBITMAP				old_bitmap_orig, bitmap_subset, old_bitmap_subset, bitmap_control, old_bitmap_control;
	int					left, right, top, bottom, zoom;
	CPen* pTempPen, oldpen;
	CBrush* pTempBrush, oldbrush;
	RMapCI				sel_region = p_tablemap->r$()->end();
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();
	CString				sel = "";

	// Get name of currently selected item
	if (m_TableMapTree.GetSelectedItem())
		sel = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());

	// Get iterator for selected region
	sel_region = p_tablemap->r$()->find(sel.GetString());
	sel_template = p_tablemap->tpl$()->find(sel.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	pDC = m_BitmapFrame.GetDC();
	hdcControl = *pDC;
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	// Select saved bitmap into memory hdc
	hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);
	old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, pDoc->attached_bitmap);

	// Figure out the size of the subset bitmap
	if (sel_region != p_tablemap->r$()->end()) {
		left = sel_region->second.left - 5;
		top = sel_region->second.top - 5;
		right = sel_region->second.right + 5;
		bottom = sel_region->second.bottom + 5;
	}
	else {
		left = sel_template->second.left - 5;
		top = sel_template->second.top - 5;
		right = sel_template->second.right + 5;
		bottom = sel_template->second.bottom + 5;
	}
	zoom = m_Zoom.GetCurSel() == 0 ? 1 :
		m_Zoom.GetCurSel() == 1 ? 2 :
		m_Zoom.GetCurSel() == 2 ? 4 :
		m_Zoom.GetCurSel() == 3 ? 8 :
		m_Zoom.GetCurSel() == 4 ? 16 : 1;

	// Create a memory DC to hold only the subset bitmap and copy
	hdc_bitmap_subset = CreateCompatibleDC(hdcScreen);
	bitmap_subset = CreateCompatibleBitmap(hdcScreen, right - left + 1, bottom - top + 1);
	old_bitmap_subset = (HBITMAP)SelectObject(hdc_bitmap_subset, bitmap_subset);
	BitBlt(hdc_bitmap_subset, 0, 0, right - left + 1, bottom - top + 1, hdc_bitmap_orig, left, top, SRCCOPY);

	// Draw selection rectangle on the copy of the bitmap
	pTempPen = (CPen*)SelectObject(hdc_bitmap_subset, red_pen);
	oldpen.FromHandle((HPEN)pTempPen);
	pTempBrush = (CBrush*)SelectObject(hdc_bitmap_subset, GetStockObject(NULL_BRUSH));
	oldbrush.FromHandle((HBRUSH)pTempBrush);
	if (sel_region != p_tablemap->r$()->end()) {
		Rectangle(hdc_bitmap_subset, 4, 4,
			6 + sel_region->second.right - sel_region->second.left + 1,
			6 + sel_region->second.bottom - sel_region->second.top + 1);
	}
	else {
		Rectangle(hdc_bitmap_subset, 4, 4,
			6 + sel_template->second.right - sel_template->second.left + 1,
			6 + sel_template->second.bottom - sel_template->second.top + 1);
	}
	SelectObject(hdc_bitmap_subset, oldpen);
	SelectObject(hdc_bitmap_subset, oldbrush);

	// Resize bitmap control to fit
	m_BitmapFrame.SetWindowPos(NULL, 0, 0, ((right - left + 1) * zoom) + 2, ((bottom - top + 1) * zoom) + 2, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);

	// Copy from bitmap subset to our control and stretch it
	bitmap_control = CreateCompatibleBitmap(hdcScreen, (right - left + 1) * zoom, (bottom - top + 1) * zoom);
	old_bitmap_control = (HBITMAP)SelectObject(hdcControl, bitmap_control);
	if (sel_region != p_tablemap->r$()->end() && sel_region->second.right >= sel_region->second.left &&
		sel_region->second.bottom >= sel_region->second.top ||
		sel_template != p_tablemap->tpl$()->end() && sel_template->second.right >= sel_template->second.left &&
		sel_template->second.bottom >= sel_template->second.top)
	{
		StretchBlt(hdcControl, 1, 1, (right - left + 1) * zoom, (bottom - top + 1) * zoom,
			hdc_bitmap_subset, 0, 0, right - left + 1, bottom - top + 1,
			SRCCOPY);
	}

	// Clean up
	SelectObject(hdcControl, old_bitmap_control);
	DeleteObject(bitmap_control);

	SelectObject(hdc_bitmap_subset, old_bitmap_subset);
	DeleteObject(bitmap_subset);
	DeleteDC(hdc_bitmap_subset);

	SelectObject(hdc_bitmap_orig, old_bitmap_orig);
	DeleteDC(hdc_bitmap_orig);

	DeleteDC(hdcScreen);
	ReleaseDC(pDC);
}

void CDlgTableMap::draw_template_bitmap(void)
{
	int					x, y, width, height, zoom;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CDC* pDC;
	HDC					hdcControl, hdcScreen, hdc_image;
	HBITMAP				bitmap_image, old_bitmap_image, bitmap_control, old_bitmap_control;
	BYTE* pBits, alpha, red, green, blue;
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();
	CString				sel = "";


	// Get name of currently selected item
	if (m_TableMapTree.GetSelectedItem())
		sel = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());

	// Get iterator for selected region
	sel_template = p_tablemap->tpl$()->find(sel.GetString());

	// Exit if we can't find the region
	if (sel_template == p_tablemap->tpl$()->end())
		return;

	// Get bitmap size
	width = sel_template->second.width;
	height = sel_template->second.height;

	// Copy saved bitmap into a memory dc so we can get the bmi
	pDC = m_BitmapFrame.GetDC();
	hdcControl = *pDC;
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	// Create new memory DC and new bitmap
	hdc_image = CreateCompatibleDC(hdcScreen);
	bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
	old_bitmap_image = (HBITMAP)SelectObject(hdc_image, bitmap_image);

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
	for (y = 0; y < (int)height; y++) {
		for (x = 0; x < (int)width; x++) {
			// image record is stored internally in ABGR format
			alpha = (sel_template->second.pixel[y * width + x] >> 24) & 0xff;
			red = (sel_template->second.pixel[y * width + x] >> 0) & 0xff;
			green = (sel_template->second.pixel[y * width + x] >> 8) & 0xff;
			blue = (sel_template->second.pixel[y * width + x] >> 16) & 0xff;

			// SetDIBits format is BGRA
			pBits[y * width * 4 + x * 4 + 0] = blue;
			pBits[y * width * 4 + x * 4 + 1] = green;
			pBits[y * width * 4 + x * 4 + 2] = red;
			pBits[y * width * 4 + x * 4 + 3] = alpha;
		}
	}
	::SetDIBits(hdc_image, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

	Mat bmp(height, width, CV_8UC4);
	bmp.data = pBits;
	//imshow("Output", bmp);

	// Figure size of stretched bitmap and resize control to fit
	zoom = m_Zoom.GetCurSel() == 0 ? 1 :
		m_Zoom.GetCurSel() == 1 ? 2 :
		m_Zoom.GetCurSel() == 2 ? 4 :
		m_Zoom.GetCurSel() == 3 ? 8 :
		m_Zoom.GetCurSel() == 4 ? 16 : 1;

	m_BitmapFrame.SetWindowPos(NULL, 0, 0, width * zoom, height * zoom, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);

	// Copy from saved bitmap DC copy to our control and stretch it
	bitmap_control = CreateCompatibleBitmap(hdcScreen, width * zoom, height * zoom);
	old_bitmap_control = (HBITMAP)SelectObject(hdcControl, bitmap_control);
	StretchBlt(hdcControl, 0, 0, width * zoom, height * zoom,
		hdc_image, 0, 0, width, height,
		SRCCOPY);

	// Clean up
	delete[]pBits;

	SelectObject(hdcControl, old_bitmap_control);
	DeleteObject(bitmap_control);

	SelectObject(hdc_image, old_bitmap_image);
	DeleteObject(bitmap_image);
	DeleteDC(hdc_image);

	DeleteDC(hdcScreen);
	ReleaseDC(pDC);
}

void CDlgTableMap::draw_image_bitmap(void)
{
	int					x, y, width, height, zoom;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CDC* pDC;
	HDC					hdcControl, hdcScreen, hdc_image;
	HBITMAP				bitmap_image, old_bitmap_image, bitmap_control, old_bitmap_control;
	BYTE* pBits, alpha, red, green, blue;
	IMapCI				sel_image = p_tablemap->i$()->end();

	// Get selected image record
	if (m_TableMapTree.GetSelectedItem())
	{
		int index = (int)m_TableMapTree.GetItemData(m_TableMapTree.GetSelectedItem());
		sel_image = p_tablemap->i$()->find(index);
	}
	else
	{
		return;
	}

	// Get bitmap size
	width = sel_image->second.width;
	height = sel_image->second.height;

	// Copy saved bitmap into a memory dc so we can get the bmi
	pDC = m_BitmapFrame.GetDC();
	hdcControl = *pDC;
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	// Create new memory DC and new bitmap
	hdc_image = CreateCompatibleDC(hdcScreen);
	bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
	old_bitmap_image = (HBITMAP)SelectObject(hdc_image, bitmap_image);

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
	for (y = 0; y < (int)height; y++) {
		for (x = 0; x < (int)width; x++) {
			// image record is stored internally in ABGR format
			alpha = (sel_image->second.pixel[y * width + x] >> 24) & 0xff;
			red = (sel_image->second.pixel[y * width + x] >> 0) & 0xff;
			green = (sel_image->second.pixel[y * width + x] >> 8) & 0xff;
			blue = (sel_image->second.pixel[y * width + x] >> 16) & 0xff;

			// SetDIBits format is BGRA
			pBits[y * width * 4 + x * 4 + 0] = blue;
			pBits[y * width * 4 + x * 4 + 1] = green;
			pBits[y * width * 4 + x * 4 + 2] = red;
			pBits[y * width * 4 + x * 4 + 3] = alpha;
		}
	}
	::SetDIBits(hdc_image, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

	// Figure size of stretched bitmap and resize control to fit
	zoom = m_Zoom.GetCurSel() == 0 ? 1 :
		m_Zoom.GetCurSel() == 1 ? 2 :
		m_Zoom.GetCurSel() == 2 ? 4 :
		m_Zoom.GetCurSel() == 3 ? 8 :
		m_Zoom.GetCurSel() == 4 ? 16 : 1;

	m_BitmapFrame.SetWindowPos(NULL, 0, 0, (width * zoom) + 2, (height * zoom) + 2, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);

	// Copy from saved bitmap DC copy to our control and stretch it
	bitmap_control = CreateCompatibleBitmap(hdcScreen, width * zoom, height * zoom);
	old_bitmap_control = (HBITMAP)SelectObject(hdcControl, bitmap_control);
	StretchBlt(hdcControl, 1, 1, width * zoom, height * zoom,
		hdc_image, 0, 0, width, height,
		SRCCOPY);

	// Clean up
	delete[]pBits;

	SelectObject(hdcControl, old_bitmap_control);
	DeleteObject(bitmap_control);

	SelectObject(hdc_image, old_bitmap_image);
	DeleteObject(bitmap_image);
	DeleteDC(hdc_image);

	DeleteDC(hdcScreen);
	ReleaseDC(pDC);
}

void CDlgTableMap::OnBoxColorChange()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text, alpha, red, green, blue;
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	// the last item is "Custom...".
	CString strText;
	m_BoxColor.GetLBText(m_BoxColor.GetCurSel(), strText);

	if (strText == "Custom...")
	{
		CColorDialog dlg;

		if (dlg.DoModal() == IDOK)
		{
			m_BoxColor.SetItemData(m_BoxColor.GetCurSel(), dlg.GetColor());
		}
	}

	sel_region->second.box_color = m_BoxColor.GetCurSel();

	update_ocr_r$_display();
	Invalidate(false);
}

void CDlgTableMap::OnOcrRegionChange()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text, alpha, red, green, blue;
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_template != p_tablemap->tpl$()->end()) {
		// OCR image detecting settings
		sel_template->second.use_default = m_UseDefault.GetCheck();
		sel_template->second.match_mode = m_MatchMode.GetCurSel();
	}


	if (sel_region != p_tablemap->r$()->end()) {
		// OCR image processing settings
		if (sel_region->second.transform.Find("A", 0) != -1) {
			sel_region->second.use_default = m_UseDefault.GetCheck();
			m_Threshold.GetWindowText(text);
			sel_region->second.threshold = strtoul(text.GetString(), NULL, 10);

			sel_region->second.use_cropping = m_UseCrop.GetCheck();
			m_CropSize.GetWindowText(text);
			sel_region->second.crop_size = strtoul(text.GetString(), NULL, 10);
			sel_region->second.box_color = m_BoxColor.GetCurSel();
		}
		if (sel_region->second.transform == "I") {
			sel_region->second.use_default = m_UseDefault.GetCheck();
			m_Threshold.GetWindowText(text);
			sel_region->second.threshold = strtoul(text.GetString(), NULL, 10);
		}
	}


	update_ocr_r$_display();
	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);

	pDoc->SetModifiedFlag(true);
}

void CDlgTableMap::OnRegionChange()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text, alpha, red, green, blue;
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_region != p_tablemap->r$()->end()) {
		// left/top/right/bottom
		m_Left.GetWindowText(text);
		sel_region->second.left = strtoul(text.GetString(), NULL, 10);
		m_Top.GetWindowText(text);
		sel_region->second.top = strtoul(text.GetString(), NULL, 10);
		m_Right.GetWindowText(text);
		sel_region->second.right = strtoul(text.GetString(), NULL, 10);
		m_Bottom.GetWindowText(text);
		sel_region->second.bottom = strtoul(text.GetString(), NULL, 10);

		// color/radius (color is stored internally and in the .tm file in ABGR (COLORREF) format)
		m_Alpha.GetWindowText(alpha);
		m_Red.GetWindowText(red);
		m_Green.GetWindowText(green);
		m_Blue.GetWindowText(blue);
		sel_region->second.color = ((strtoul(alpha.GetString(), NULL, 16)) << 24) +  // alpha
			((strtoul(blue.GetString(), NULL, 16)) << 16) +   // red
			((strtoul(green.GetString(), NULL, 16)) << 8) +   // green
			(strtoul(red.GetString(), NULL, 16));          // blue
		m_Radius.GetWindowText(text);
		sel_region->second.radius = strtoul(text.GetString(), NULL, 10);

		// transform type
		m_Transform.GetLBText(m_Transform.GetCurSel(), text);
		sel_region->second.transform =
			text == "Color" ? "C" :
			text == "Image" ? "I" :
			text == "AutoOcr0" ? "A0" :
			text == "AutoOcr1" ? "A1" :
			text == "Text0" ? "T0" :
			text == "Text1" ? "T1" :
			text == "Text2" ? "T2" :
			text == "Text3" ? "T3" :
			text == "Text4" ? "T4" :
			text == "Text5" ? "T5" :
			text == "Text6" ? "T6" :
			text == "Text7" ? "T7" :
			text == "Text8" ? "T8" :
			text == "Text9" ? "T9" :
			text == "Hash0" ? "H0" :
			text == "Hash1" ? "H1" :
			text == "Hash2" ? "H2" :
			text == "Hash3" ? "H3" :
			text == "WebColour" ? "WC" :
			text == "None" ? "N" :
			"";

		// OCR image processing settings
		if (sel_region->second.transform.Find("A", 0) != -1) {
			sel_region->second.use_default = m_UseDefault.GetCheck();
			m_Threshold.GetWindowText(text);
			sel_region->second.threshold = strtoul(text.GetString(), NULL, 10);

			sel_region->second.use_cropping = m_UseCrop.GetCheck();
			m_CropSize.GetWindowText(text);
			sel_region->second.crop_size = strtoul(text.GetString(), NULL, 10);
			sel_region->second.box_color = m_BoxColor.GetCurSel();
		}
		if (sel_region->second.transform == "I") {
			sel_region->second.use_default = m_UseDefault.GetCheck();
			m_Threshold.GetWindowText(text);
			sel_region->second.threshold = strtoul(text.GetString(), NULL, 10);
		}
	}


	if (sel_template != p_tablemap->tpl$()->end()) {
		// left/top/right/bottom
		//sel_template->second.width = sel_template->second.right - sel_template->second.left;
		//sel_template->second.height = sel_template->second.bottom - sel_template->second.top;
		if (sel_template->second.created == true) {
			sel_template->second.left = 0;
			sel_template->second.top = 0;
			sel_template->second.right = 0;
			sel_template->second.bottom = 0;
		}
		else {
			m_Left.GetWindowText(text);
			sel_template->second.left = strtoul(text.GetString(), NULL, 10);
			m_Top.GetWindowText(text);
			sel_template->second.top = strtoul(text.GetString(), NULL, 10);
			m_Right.GetWindowText(text);
			sel_template->second.right = strtoul(text.GetString(), NULL, 10);
			m_Bottom.GetWindowText(text);
			sel_template->second.bottom = strtoul(text.GetString(), NULL, 10);
		}

		sel_template->second.use_default = m_UseDefault.GetCheck();
		sel_template->second.match_mode = m_MatchMode.GetCurSel();
	}


	update_display();
	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);

	pDoc->SetModifiedFlag(true);
}

void CDlgTableMap::OnZoomChange()
{
	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);
}

void CDlgTableMap::OnTvnSelchangedTablemapTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTREEVIEW	pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	update_display();

	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);

	*pResult = 0;
}

void CDlgTableMap::update_ocr_display(void)
{
	CString				text, hashname, type, charstr, sel_ch, sel_type, sel_temp;
	CString				num, x, y;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Don't trigger OnChange messages
	ignore_changes = true;

	disable_ocr_and_clear_all();

	m_UseDefault.EnableWindow(true);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (sel_region != p_tablemap->r$()->end()) {
		if (sel_region->second.transform.Find("A", 0) != -1)
			m_UseCrop.EnableWindow(true);

		if (m_UseDefault.GetCheck() == false) {
			m_Threshold.EnableWindow(true);
			m_ThresholdSpin.EnableWindow(true);
		}
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		if (sel_template->second.created)
			m_Result.SetWindowText("Created");
		else
			m_Result.SetWindowText("Not Created");

		if (m_UseDefault.GetCheck() == false)
			m_MatchMode.EnableWindow(true);
	}

	if (m_UseCrop.GetCheck()) {
		m_CropSize.EnableWindow(true);
		m_CropSpin.EnableWindow(true);
		m_BoxColor.EnableWindow(true);
	}

	update_ocr_r$_display();

	// Re-enable triggering of OnChange messages
	ignore_changes = false;
}

void CDlgTableMap::update_display(void)
{
	CString				text, hashname, type, charstr, sel_ch, sel_type, sel_temp;
	CString				num, x, y;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	CString	sel_text, type_text;
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Don't trigger OnChange messages
	ignore_changes = true;

	clear_mat_control();
	disable_and_clear_all();

	m_UseDefault.SetCheck(true);
	m_UseCrop.SetCheck(false);

	m_CreateFont.SetWindowText("Create Font");
	m_CreateImage.SetWindowText("Create Image");

	// A root item was selected
	if (type_node == NULL || m_TableMapTree.ItemHasChildren(m_TableMapTree.GetSelectedItem()))
	{
		if (type_text == "Sizes")
		{
			m_New.EnableWindow(true);
		}

		else if (type_text == "Symbols")
		{
			m_New.EnableWindow(true);
		}

		else if (type_text == "Regions")
		{
			m_New.EnableWindow(true);
		}

		else if (type_text == "Fonts")
		{
		}

		else if (type_text == "Hash Points")
		{
			m_New.EnableWindow(true);
			m_Edit.EnableWindow(true);
		}

		else if (type_text == "Hashes")
		{
		}

		else if (type_text == "Images")
		{
		}

		else if (type_text == "Templates")
		{
			m_New.EnableWindow(true);
		}
	}

	// A leaf item was selected
	else
	{
		if (type_text == "Sizes")
		{
			m_New.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(true);

			// Display selected size record
			if (m_TableMapTree.GetSelectedItem())
			{
				ZMap::const_iterator z_iter = p_tablemap->z$()->find(sel_text.GetString());
				if (z_iter != p_tablemap->z$()->end())
				{
					text.Format("%d x %d", z_iter->second.width, z_iter->second.height);
					m_Result.SetWindowText(text.GetString());
				}
			}
		}

		else if (type_text == "Symbols")
		{
			m_New.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(true);

			// Display selected symbol record
			if (m_TableMapTree.GetSelectedItem())
			{
				SMap::const_iterator s_iter = p_tablemap->s$()->find(sel_text.GetString());
				if (s_iter != p_tablemap->s$()->end())
					m_Result.SetWindowText(s_iter->second.text.GetString());
			}
		}

		else if (type_text == "Regions")
		{
			m_Left.EnableWindow(true);
			m_Top.EnableWindow(true);
			m_Right.EnableWindow(true);
			m_Bottom.EnableWindow(true);
			m_DrawRect.EnableWindow(true);
			m_Transform.EnableWindow(true);
			m_Alpha.EnableWindow(true);
			m_Red.EnableWindow(true);
			m_Green.EnableWindow(true);
			m_Blue.EnableWindow(true);
			m_Picker.EnableWindow(true);
			m_Radius.EnableWindow(true);
			m_CreateImage.EnableWindow(true);
			m_Result.EnableWindow(true);
			m_PixelSeparation.EnableWindow(true);
			m_FontPlus.EnableWindow(true);
			m_FontMinus.EnableWindow(true);
			m_New.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(false);
			m_Zoom.EnableWindow(true);
			m_NudgeTaller.EnableWindow(true);
			m_NudgeShorter.EnableWindow(true);
			m_NudgeWider.EnableWindow(true);
			m_NudgeNarrower.EnableWindow(true);
			m_NudgeBigger.EnableWindow(true);
			m_NudgeSmaller.EnableWindow(true);
			m_NudgeUpLeft.EnableWindow(true);
			m_NudgeUp.EnableWindow(true);
			m_NudgeUpRight.EnableWindow(true);
			m_NudgeRight.EnableWindow(true);
			m_NudgeDownRight.EnableWindow(true);
			m_NudgeDown.EnableWindow(true);
			m_NudgeDownLeft.EnableWindow(true);
			m_NudgeLeft.EnableWindow(true);

			RMap::const_iterator r_iter = p_tablemap->r$()->find(sel_text.GetString());

			if (r_iter->second.transform == "I") {
				m_UseDefault.EnableWindow(true);
			}

			if (r_iter->second.transform.Find("T", 0) != -1) {
				m_CreateFont.EnableWindow(true);
			}

			if (r_iter->second.transform.Find("A", 0) != -1) {
				m_Alpha.EnableWindow(false);
				m_Red.EnableWindow(false);
				m_Green.EnableWindow(false);
				m_Blue.EnableWindow(false);
				m_Picker.EnableWindow(false);
				m_Radius.EnableWindow(false);

				m_CreateImage.EnableWindow(false);
				m_UseDefault.EnableWindow(true);
				m_UseCrop.EnableWindow(true);
			}

			if (sel_text.Find("area") != -1) {
				m_Transform.EnableWindow(false);
				m_Alpha.EnableWindow(false);
				m_Red.EnableWindow(false);
				m_Green.EnableWindow(false);
				m_Blue.EnableWindow(false);
				m_Picker.EnableWindow(false);
				m_Radius.EnableWindow(false);

				m_CreateImage.EnableWindow(false);
			}

			update_r$_display(false);
		}

		else if (type_text == "Fonts")
		{
			m_PixelSeparation.EnableWindow(true);
			m_FontPlus.EnableWindow(true);
			m_FontMinus.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(true);
			update_t$_display();
		}

		else if (type_text == "Hash Points")
		{
			m_New.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(true);
		}

		else if (type_text == "Hashes")
		{
			m_Delete.EnableWindow(true);

			// Display selected hash value record
			int hash_type = GetType(sel_text);
			bool displayed = false;
			for (HMapCI h_iter = p_tablemap->h$(hash_type)->begin(); h_iter != p_tablemap->h$(hash_type)->end() && !displayed; h_iter++)
			{
				int start = 0;
				if (h_iter->second.name == sel_text.Tokenize(" ", start))
				{
					text.Format("%08x", h_iter->second.hash);
					m_Result.SetWindowText(text.GetString());
					displayed = true;
				}
			}
		}

		else if (type_text == "Images")
		{
			m_Delete.EnableWindow(true);
			m_Zoom.EnableWindow(true);
			m_CreateHash0.EnableWindow(true);
			m_CreateHash1.EnableWindow(true);
			m_CreateHash2.EnableWindow(true);
			m_CreateHash3.EnableWindow(true);

			// Get image name
			int start = 0;
			sel_text = sel_text.Tokenize(" ", start);

			// See which hash types are already present for this image
			// and disable the relevant create hash button
			for (int j = 0; j <= 3; j++)
			{
				bool found = false;
				for (HMapCI h_iter = p_tablemap->h$(j)->begin(); h_iter != p_tablemap->h$(j)->end() && !found; h_iter++)
				{
					if (h_iter->second.name == sel_text)
					{
						switch (j)
						{
						case 0:
							m_CreateHash0.EnableWindow(false);
							break;
						case 1:
							m_CreateHash1.EnableWindow(false);
							break;
						case 2:
							m_CreateHash2.EnableWindow(false);
							break;
						case 3:
							m_CreateHash3.EnableWindow(false);
							break;
						}
					}
				}
			}

		}

		else if (type_text == "Templates")
		{
			m_New.EnableWindow(true);
			m_Delete.EnableWindow(true);
			m_Edit.EnableWindow(false);

			m_Left.EnableWindow(true);
			m_Top.EnableWindow(true);
			m_Right.EnableWindow(true);
			m_Bottom.EnableWindow(true);
			m_NudgeTaller.EnableWindow(true);
			m_NudgeShorter.EnableWindow(true);
			m_NudgeWider.EnableWindow(true);
			m_NudgeNarrower.EnableWindow(true);
			m_NudgeBigger.EnableWindow(true);
			m_NudgeSmaller.EnableWindow(true);
			m_NudgeUpLeft.EnableWindow(true);
			m_NudgeUp.EnableWindow(true);
			m_NudgeUpRight.EnableWindow(true);
			m_NudgeRight.EnableWindow(true);
			m_NudgeDownRight.EnableWindow(true);
			m_NudgeDown.EnableWindow(true);
			m_NudgeDownLeft.EnableWindow(true);
			m_NudgeLeft.EnableWindow(true);
			m_DrawRect.EnableWindow(true);

			m_CreateFont.SetWindowText("Detect Template");
			m_CreateFont.EnableWindow(true);
			m_CreateImage.SetWindowText("Create Template");
			m_CreateImage.EnableWindow(true);

			m_UseDefault.EnableWindow(true);

			update_r$_display(false);
		}
	}

	// Re-enable triggering of OnChange messages
	ignore_changes = false;
}

void CDlgTableMap::disable_and_clear_all(void)
{
	m_Left.EnableWindow(false);
	m_Left.SetWindowText("");
	m_Top.EnableWindow(false);
	m_Top.SetWindowText("");
	m_Right.EnableWindow(false);
	m_Right.SetWindowText("");
	m_Bottom.EnableWindow(false);
	m_Bottom.SetWindowText("");
	m_DrawRect.EnableWindow(false);

	m_Transform.EnableWindow(false);
	m_Transform.SetCurSel(-1);
	m_Alpha.EnableWindow(false);
	m_Alpha.SetWindowText("");
	m_Red.EnableWindow(false);
	m_Red.SetWindowText("");
	m_Green.EnableWindow(false);
	m_Green.SetWindowText("");
	m_Blue.EnableWindow(false);
	m_Blue.SetWindowText("");
	m_RedAvg.EnableWindow(false);
	m_RedAvg.SetWindowText("");
	m_GreenAvg.EnableWindow(false);
	m_GreenAvg.SetWindowText("");
	m_BlueAvg.EnableWindow(false);
	m_BlueAvg.SetWindowText("");
	m_Picker.EnableWindow(false);
	m_Radius.EnableWindow(false);
	m_Radius.SetWindowText("");

	m_TrackerFontSet.SetCurSel(0);
	m_TrackerFontNum.SetCurSel(1);
	m_TrackerCardNum.SetCurSel(1);

	m_CreateImage.EnableWindow(false);
	m_CreateFont.EnableWindow(false);
	m_CreateHash0.EnableWindow(false);
	m_CreateHash1.EnableWindow(false);
	m_CreateHash2.EnableWindow(false);
	m_CreateHash3.EnableWindow(false);

	m_PixelSeparation.EnableWindow(false);
	m_PixelSeparation.SetWindowText("");
	m_FontPlus.EnableWindow(false);
	m_FontMinus.EnableWindow(false);

	m_Result.EnableWindow(false);
	m_Result.SetWindowText("");

	m_New.EnableWindow(false);
	m_Delete.EnableWindow(false);
	m_Edit.EnableWindow(false);
	m_Zoom.EnableWindow(false);

	m_NudgeTaller.EnableWindow(false);
	m_NudgeShorter.EnableWindow(false);
	m_NudgeWider.EnableWindow(false);
	m_NudgeNarrower.EnableWindow(false);
	m_NudgeBigger.EnableWindow(false);
	m_NudgeSmaller.EnableWindow(false);
	m_NudgeUpLeft.EnableWindow(false);
	m_NudgeUp.EnableWindow(false);
	m_NudgeUpRight.EnableWindow(false);
	m_NudgeRight.EnableWindow(false);
	m_NudgeDownRight.EnableWindow(false);
	m_NudgeDown.EnableWindow(false);
	m_NudgeDownLeft.EnableWindow(false);
	m_NudgeLeft.EnableWindow(false);

	disable_ocr_and_clear_all();
}

void CDlgTableMap::disable_ocr_and_clear_all(void)
{
	//m_ImgProc.SetWindowText("Inbuilt image processing");
	m_UseDefault.EnableWindow(false);
	m_Threshold.EnableWindow(false);
	m_ThresholdSpin.EnableWindow(false);
	threshold = 0;
	m_Threshold.SetWindowText(to_string(threshold).c_str());
	m_MatchMode.EnableWindow(false);
	match_mode = -1;
	m_MatchMode.SetCurSel(match_mode);

	m_UseCrop.EnableWindow(false);
	m_CropSize.EnableWindow(false);
	m_CropSpin.EnableWindow(false);
	crop_size = 0;
	m_CropSize.SetWindowText(to_string(crop_size).c_str());
	m_BoxColor.EnableWindow(false);
	box_color = -1;
	m_BoxColor.SetCurSel(box_color);
}


////  Automatic Text Detection and Recognition functions  ///////////
Mat CDlgTableMap::binarize_array_opencv(Mat image, int threshold) {
	// Binarize image from gray channel with 100 as threshold
	Mat img;
	cvtColor(image, img, COLOR_BGR2RGB);
	cvtColor(img, img, COLOR_BGR2GRAY);
	Mat thresh, blur;
	cv::threshold(img, thresh, threshold, 255, THRESH_BINARY_INV); // 100 threshold
	float kernel_data[9] = { 1, 2, 1, 2, 4, 2, 1, 2, 1 };
	Mat kernel = Mat(1, 1, CV_32F, kernel_data);
	cv::filter2D(thresh, blur, -1, kernel);
	Mat ret;
	cv::threshold(blur, ret, 250, 255, THRESH_BINARY); // 250 threshold
	return ret;
}

Mat CDlgTableMap::prepareImage(Mat img_orig, bool binarize, int threshold, bool second_pass) {
	// Prepare image for OCR
	//  !!  Do not change those settings and values !!   //
	//  Or display on OCR view control will be distorded //
	Mat img_resized;
	int basewidth, hsize;
	float wpercent;
	if (img_orig.cols > img_orig.rows * 1.25) {
		basewidth = MAT_WIDTH;
		wpercent = (basewidth / static_cast<float>(img_orig.cols));
		hsize = static_cast<int>(static_cast<float>(img_orig.rows) * wpercent);
	}
	else {
		hsize = MAT_HEIGHT;
		wpercent = (hsize / static_cast<float>(img_orig.rows));
		basewidth = static_cast<int>(static_cast<float>(img_orig.cols) * wpercent);
	}
	cvtColor(img_orig, img_resized, COLOR_BGR2GRAY);
	resize(img_resized, img_resized, Size(basewidth, hsize), INTER_LANCZOS4);

	if (binarize) {
		img_resized = binarize_array_opencv(img_resized, threshold);
	}

	Mat img_bounded = img_resized.clone();
	img_bounded.convertTo(img_bounded, CV_8UC3);
	cvtColor(img_bounded, img_bounded, COLOR_GRAY2BGR);
	m_crColor = m_BoxColor.GetSelectedColorValue();

	/* Add border to bestRect ROI + resize it to fit 30-33px height for capital letter
	// for optimal 0% error rate on Tesseract recognition: https://groups.google.com/g/tesseract-ocr/c/Wdh_JJwnw94/m/24JHDYQbBQAJ
	const int border = 25; // 10
	//const int borderType = BORDER_CONSTANT | BORDER_ISOLATED;
	const Scalar value(255, 255, 255);
	const Mat roi(img_resized.rows + 2 * border, img_resized.cols + 2 * border, CV_8UC1, value);
	img_resized.copyTo(roi(Rect(border, border, img_resized.cols, img_resized.rows)));
	img_resized = roi;
	int height = 70; // 29 with 10 border
	float pct = (height / static_cast<float>(img_resized.rows));
	int width = static_cast<int>(static_cast<float>(img_resized.cols) * pct);
	resize(img_resized, img_resized, Size(width, height), INTER_LANCZOS4);
	*/

	// Cropping management ///////////////////////////
	if (m_UseCrop.GetCheck() == true) {
		CString txt;
		m_CropSize.GetWindowText(txt);
		double cropSize = (double)atoi(txt) / 100;
		if (cropSize < 0.01)
			return img_bounded;

		process_ocr(img_resized, second_pass);
		vector<pair<Rect, CString>> resBoxes;
		if (second_pass)
			resBoxes = ResultBoxes2;
		else
			resBoxes = ResultBoxes;

		// Exit here if no box found (generally an empty region)
		if (resBoxes.empty())
			return img_bounded;

		// filter contours
		vector<Rect> boundRect, boundRect2;
		for (size_t idx = 0; idx < resBoxes.size(); idx++)
		{
			Rect rect = resBoxes[idx].first;
			if (rect.area() > 50) {
				boundRect.push_back(rect);
			}
		}

		// Exit if no valid box found (generally an empty region)
		if (boundRect.empty())
			return img_bounded;

		vector<double> boxArea, boxDist;
		double wCenter = img_bounded.cols / 2;
		double hCenter = img_bounded.rows / 2;
		Point pCenter(wCenter, hCenter);
		Rect best_rect = Rect();
		if (second_pass)
			bestRect2 = Rect();
		else
			bestRect = Rect();
		//  Find best box match for recognition
		// First find biggest box(es)
		for (size_t i = 0; i < boundRect.size(); i++) {
			boxArea.push_back(boundRect[i].width * boundRect[i].height);
		}
		// Select the element with the maximum value
		auto ita = max_element(boxArea.begin(), boxArea.end());
		int maxArea = *ita;
		vector<int> maxIndex;
		for (size_t i = 0; i < boxArea.size(); i++) {
			if (boxArea[i] > maxArea * (1 - cropSize)) {  // default to 40% diff factor for concurrent boxes
				maxIndex.push_back(i);
			}
		}
		for (size_t i = 0; i < maxIndex.size(); i++) {
			int j = maxIndex[i];
			best_rect = boundRect[j];
			rectangle(img_bounded, best_rect, Scalar(GetBValue(m_crColor), GetGValue(m_crColor), GetRValue(m_crColor)), 1);
			boundRect2.push_back(best_rect);
		}
		// Second find nearest big box from region center
		if (boundRect2.size() > 1) {
			for (size_t i = 0; i < boundRect2.size(); i++) {
				double wcenter = boundRect2[i].x + boundRect2[i].width / 2;
				double hcenter = boundRect2[i].y + boundRect2[i].height / 2;
				Point pcenter(wcenter, hcenter);
				double dist = abs(norm(pCenter - pcenter));
				boxDist.push_back(dist);
			}
			// Select the element with the minimum value
			auto itd = min_element(boxDist.begin(), boxDist.end());
			int minDist = *itd;
			for (size_t i = 0; i < boxDist.size(); i++) {
				if (boxDist[i] == minDist) {
					best_rect = boundRect2[i];
					break;
				}
			}
		}

		// Set the corresponding global bestRect
		if (second_pass)
			bestRect2 = best_rect;
		else
			bestRect = best_rect;

		// Draw best box
		rectangle(img_bounded, best_rect, Scalar(GetBValue(m_crColor), GetGValue(m_crColor), GetRValue(m_crColor)), 2);

		return img_bounded;
	}
	//////////////////////////////////////////////////

	process_ocr(img_resized, second_pass);
	return img_bounded;
}

void CDlgTableMap::process_ocr(Mat img_orig, bool fast, bool second_pass) {
	if (!m_TableMapTree.GetSelectedItem())
		return;

	api->SetImage(img_orig.data, img_orig.cols, img_orig.rows, img_orig.channels(), img_orig.step);
	api->Recognize(0);

	if (m_UseCrop.GetCheck() == true) {
		ResultIterator* ri = api->GetIterator();
		PageIteratorLevel level = tesseract::RIL_WORD;
		if (ri != 0) {
			do {
				CString word = ri->GetUTF8Text(level);
				float conf = ri->Confidence(level);
				int x1, y1, x2, y2;
				ri->BoundingBox(level, &x1, &y1, &x2, &y2);
				Mat img_cropped;
				try {
					img_cropped = img_orig({ x1, y1, x2 - x1, y2 - y1 });
				}
				catch (exception e) {
					continue;
				}
				api2->SetImage(img_cropped.data, img_cropped.cols, img_cropped.rows, img_cropped.channels(), img_cropped.step);
				api2->Recognize(0);
				word = trim(api2->GetUTF8Text()).c_str();
				pair<Rect, CString> matchPair({ x1, y1, x2 - x1, y2 - y1 }, word);
				if (second_pass)
					ResultBoxes2.push_back(matchPair);
				else
					ResultBoxes.push_back(matchPair);
			} while (ri->Next(level));
		}
	}
	else {
		if (second_pass)
			ResultString2 = trim(api->GetUTF8Text()).c_str();
		else
			ResultString = trim(api->GetUTF8Text()).c_str();
	}
	api->Clear();
	api2->Clear();
}

CString CDlgTableMap::get_ocr_result(Mat img_orig, CString transform, bool fast) {
	// Return string value from image. "" when OCR failed
	Mat img_resized, img_resized2;
	ResultBoxes.clear(); ResultBoxes2.clear();
	ResultString = ResultString2 = "";

	CString txt;
	m_Threshold.GetWindowText(txt);
	threshold = atoi(txt);

	if (transform == "AutoOcr0") {
		img_resized = prepareImage(img_orig, true);
		img_resized2 = prepareImage(img_orig, true, threshold, true);
	}
	if (transform == "AutoOcr1") {
		img_resized = prepareImage(img_orig, true, threshold);
		img_resized2 = prepareImage(img_orig, true, 76, true);
	}

	vector<CString> result_list;
	CString ocr_result, ocr_result2;
	if (m_UseCrop.GetCheck() == true) {
		for (auto& element : ResultBoxes) {
			if (element.first == bestRect) {
				ocr_result = element.second;
				break;
			}
		}
		for (auto& element : ResultBoxes2) {
			if (element.first == bestRect2) {
				ocr_result2 = element.second;
				break;
			}
		}
	}
	else {
		ocr_result = ResultString;
		ocr_result2 = ResultString2;
	}

	// Clean results from unwanted chars
	const char* blacklist = "®©℗ⓒ™!%&*+;=?@²^æÆÇçÉéèêëïîíìÄÅÂÀàáâäåúùûüôöòñÑÿÖÜ€£¥₧ƒ~ªº¿⌐¬½¼¡«»/\"`#<{([])}>|│░▒▓┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■";
	for (size_t i = 0; i < strlen(blacklist); i++) {
		if (ocr_result.Find(blacklist[i]) != -1)
			ocr_result.Replace(blacklist[i], '\0');
		if (ocr_result2.Find(blacklist[i]) != -1)
			ocr_result2.Replace(blacklist[i], '\0');
	}


	if (ocr_result != "")
		result_list.push_back(ocr_result);
	if (ocr_result2 != "")
		result_list.push_back(ocr_result2);


	// Display OCR image on OCR view
	CDC* pDC;
	HDC					hdcControl;

	//m_MatFrame.Invalidate(true);
	//m_MatFrame.RedrawWindow();
	pDC = m_MatFrame.GetDC();
	hdcControl = *pDC;
	BITMAPINFOHEADER bih = { sizeof(bih), img_resized.cols, -img_resized.rows, 1, 24, BI_RGB };
	vector<RGBQUAD> pal(256);
	for (int32_t i(0); i < 256; ++i) {
		pal[i].rgbRed = pal[i].rgbGreen = pal[i].rgbBlue = i;
		pal[i].rgbReserved = 0;
	}
	BITMAPINFO bi = { bih, pal[1] };
	if (result_list.empty() || result_list.back() == ocr_result) {
		m_MatFrame.SetWindowPos(NULL, 0, 0, img_resized.cols, img_resized.rows, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
		SetDIBitsToDevice(hdcControl, 0, 0, img_resized.cols, img_resized.rows,
			0, 0, 0, img_resized.rows, img_resized.data, &bi,
			DIB_RGB_COLORS);
	}
	else {
		m_MatFrame.SetWindowPos(NULL, 0, 0, img_resized2.cols, img_resized2.rows, SWP_NOMOVE | SWP_NOZORDER | SWP_NOCOPYBITS);
		SetDIBitsToDevice(hdcControl, 0, 0, img_resized2.cols, img_resized2.rows,
			0, 0, 0, img_resized2.rows, img_resized2.data, &bi,
			DIB_RGB_COLORS);
	}
	// Clean up
	DeleteDC(hdcControl);
	ReleaseDC(pDC);


	// Display OCR recognition result
	try {
		if (!result_list.empty()) {
			return result_list.back();
		}
		else {
			return "";
		}
	}
	catch (invalid_argument) {
		if (fast) {
			return "";
		}
		// , img_min, img_mod, img_med, img_sharp]
		vector<Mat> images = { img_orig, img_resized };
		int i = 0;
		while (i < 2) {
			size_t j = 0;
			while (j < images.size()) {
				CString ocr_str;
				process_ocr(images[j]);
				for (auto& element : ResultBoxes) {
					if (element.first == bestRect) {
						ocr_str = element.second;
						break;
					}
				}
				if (ocr_str != "")
					result_list.push_back(ocr_str);
				j++;
			}
			i++;
		}
	}
#pragma warning(push)
#pragma warning(disable:4101)
	for (const auto& element : result_list) {
		try {
			return element;
		}
		catch (const invalid_argument& e) {
			continue;
			// log.warning(f"Not recognized: {element}")
		}
	}
#pragma warning(pop)

	return "";
}

void CDlgTableMap::DetectAndShowTemplate(string name) {
	//  Detect template	
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CDC* pDC;
	HDC					hdcControl, hdcScreen, hdc_bitmap_orig, hdc_bitmap_transform_ocr;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform_ocr;
	RMapCI				r_iter = p_tablemap->r$()->end();
	Mat area_mat, template_mat;

	// Go calc the result and display it
	pDC = m_BitmapFrame.GetDC();
	hdcControl = *pDC;
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);

	CString area_name;
	bool template_found = false;
	for (r_iter = p_tablemap->r$()->begin(); r_iter != p_tablemap->r$()->end(); r_iter++) {
		bool search_template = false;
		area_name = r_iter->second.name;
		if (area_name == "area_cards_common" && name.find("card_common_") != -1)
			search_template = true;
		else if (area_name.Find("area_cards_player") != -1 && name.find("card_player_") != -1)
			search_template = true;
		else if (area_name.Find("area_buttons_zone") != -1)
			search_template = true;

		if (search_template) {
			old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, pDoc->attached_bitmap);

			// Get bitmap size
			int w = r_iter->second.right - r_iter->second.left + 1;
			int h = r_iter->second.bottom - r_iter->second.top + 1;

			hdc_bitmap_transform_ocr = CreateCompatibleDC(hdcScreen);
			bitmap_transform_ocr = CreateCompatibleBitmap(hdcScreen, w, h);
			old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform_ocr, bitmap_transform_ocr);

			BitBlt(hdc_bitmap_transform_ocr, 0, 0, w, h,
				hdc_bitmap_orig,
				r_iter->second.left - 1, r_iter->second.top - 1,
				SRCCOPY);

			area_mat = Mat(h, w, CV_8UC4);
			BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
			GetDIBits(hdc_bitmap_transform_ocr, bitmap_transform_ocr, 0, h, area_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

			int					x, y, width, height, zoom;
			HBITMAP				bitmap_image, old_bitmap_image, bitmap_control, old_bitmap_control;
			BYTE* pBits, alpha, red, green, blue;
			TPLMapCI			sel_template = p_tablemap->tpl$()->end();

			// Get selected template record
			if (m_TableMapTree.GetSelectedItem())
			{
				sel_template = p_tablemap->tpl$()->find(name.c_str());
			}
			else
			{
				return;
			}

			// Get template size
			width = sel_template->second.width;
			height = sel_template->second.height;

			// Create new memory DC and new bitmap
			bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
			old_bitmap_image = (HBITMAP)SelectObject(hdc_bitmap_orig, bitmap_image);

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
			for (y = 0; y < (int)height; y++) {
				for (x = 0; x < (int)width; x++) {
					// image record is stored internally in ABGR format
					alpha = (sel_template->second.pixel[y * width + x] >> 24) & 0xff;
					red = (sel_template->second.pixel[y * width + x] >> 0) & 0xff;
					green = (sel_template->second.pixel[y * width + x] >> 8) & 0xff;
					blue = (sel_template->second.pixel[y * width + x] >> 16) & 0xff;

					// SetDIBits format is BGRA
					pBits[y * width * 4 + x * 4 + 0] = blue;
					pBits[y * width * 4 + x * 4 + 1] = green;
					pBits[y * width * 4 + x * 4 + 2] = red;
					pBits[y * width * 4 + x * 4 + 3] = alpha;
				}
			}
			::SetDIBits(hdc_bitmap_orig, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

			template_mat = Mat(height, width, CV_8UC4);
			template_mat.data = pBits;


			// Display match result
			Mat match_roi;
			int match_method = m_MatchMode.GetCurSel();
			// Do the Matching and Normalize
			matchTemplate(area_mat, template_mat, match_roi, match_method);
			//normalize(match_roi, match_roi, 0, 1, NORM_MINMAX, -1, Mat());

			/// Localizing the best match with minMaxLoc
			double matchVal, minVal, maxVal;
			Point minLoc, maxLoc, matchLoc;

			minMaxLoc(match_roi, &minVal, &maxVal, &minLoc, &maxLoc);

			/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
			if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
			{
				matchLoc = minLoc;
				matchVal = minVal;
			}
			else
			{
				matchLoc = maxLoc;
				matchVal = maxVal;
			}

			// Exact match
			if (-0.02 <= matchVal && matchVal <= 0.02 || 0.98 <= matchVal && matchVal <= 1.02) {  // 0.02% tolerance
				template_found = true;
				// Show me what you got
				rectangle(area_mat, matchLoc, Point(matchLoc.x + template_mat.cols, matchLoc.y + template_mat.rows), Scalar(0, 255, 0), 2, 8, 0);

				imshow("Result view", area_mat);
				//imshow("Result view", match_roi);
				//waitKey();
				break;
			}
			// Clean up
			delete[]pBits;
			SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
			DeleteObject(bitmap_transform_ocr);
			DeleteDC(hdc_bitmap_transform_ocr);

			SelectObject(hdc_bitmap_orig, old_bitmap_orig);
			DeleteDC(hdc_bitmap_orig);

			DeleteDC(hdcScreen);
		}
	}

	// No match found
	if (!template_found)
		MessageBox("No match found.", "Info", MB_OK);


	// Clean up
	SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
	DeleteObject(bitmap_transform_ocr);
	DeleteDC(hdc_bitmap_transform_ocr);

	SelectObject(hdc_bitmap_orig, old_bitmap_orig);
	DeleteDC(hdc_bitmap_orig);

	DeleteDC(hdcScreen);

}

CString CDlgTableMap::GetDetectTemplateResult(CString area_name, CString tpl_name, RECT* rect_result) {
	//  Detect template
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	HDC					hdc = ::GetDC(pDoc->attached_hwnd);
	HDC					hdcScreen, hdcCompat, hdc_bitmap_orig, hdc_bitmap_transform_ocr;
	CString				text, selected_transform, separation;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform_ocr;
	RMapCI				r_iter = p_tablemap->r$()->find(area_name.GetString());
	TPLMapCI			sel_template = p_tablemap->tpl$()->find(tpl_name.GetString());

	// Exit if the area or template doesn't exist
	if (r_iter == p_tablemap->r$()->end() || sel_template == p_tablemap->tpl$()->end())
		return "";

	// Exit if template isn't created
	if (!sel_template->second.created)
		return "";

	// Bitblt the attached windows bitmap into a HDC
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	hdcCompat = CreateCompatibleDC(hdcScreen);
	RECT rect;
	::GetClientRect(pDoc->attached_hwnd, &rect);
	HBITMAP attached_bitmap = CreateCompatibleBitmap(hdcScreen, rect.right, rect.bottom);
	HBITMAP	old_bitmap = (HBITMAP)SelectObject(hdcCompat, attached_bitmap);
	BitBlt(hdcCompat, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);

	hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);
	old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, attached_bitmap);
	//SaveHBITMAPToFile(attached_bitmap, "output.bmp");

	// Get bitmap size
	int w = r_iter->second.right - r_iter->second.left + 1;
	int h = r_iter->second.bottom - r_iter->second.top + 1;

	hdc_bitmap_transform_ocr = CreateCompatibleDC(hdcScreen);
	bitmap_transform_ocr = CreateCompatibleBitmap(hdcScreen, w, h);
	old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform_ocr, bitmap_transform_ocr);

	BitBlt(hdc_bitmap_transform_ocr, 0, 0, w, h,
		hdc,
		r_iter->second.left - 1, r_iter->second.top - 1,
		SRCCOPY);

	Mat area_mat(h, w, CV_8UC4);
	BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
	GetDIBits(hdc_bitmap_transform_ocr, bitmap_transform_ocr, 0, h, area_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	int					x, y, width, height, match_mode;
	HBITMAP				bitmap_image, old_bitmap_image;
	BYTE* pBits, alpha, red, green, blue;
	RECT roi, zero_rect = RECT{ 0 };
	vector<pair<int, CString>> listROI;

	// Get template size
	width = sel_template->second.width;
	height = sel_template->second.height;
	match_mode = sel_template->second.match_mode;
	if (width < 1 || height < 1 || match_mode < 0)
		return "";

	// Create new memory DC and new bitmap
	bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
	old_bitmap_image = (HBITMAP)SelectObject(hdc_bitmap_orig, bitmap_image);

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
	for (y = 0; y < (int)height; y++) {
		for (x = 0; x < (int)width; x++) {
			// image record is stored internally in ABGR format
			alpha = (sel_template->second.pixel[y * width + x] >> 24) & 0xff;
			red = (sel_template->second.pixel[y * width + x] >> 0) & 0xff;
			green = (sel_template->second.pixel[y * width + x] >> 8) & 0xff;
			blue = (sel_template->second.pixel[y * width + x] >> 16) & 0xff;

			// SetDIBits format is BGRA
			pBits[y * width * 4 + x * 4 + 0] = blue;
			pBits[y * width * 4 + x * 4 + 1] = green;
			pBits[y * width * 4 + x * 4 + 2] = red;
			pBits[y * width * 4 + x * 4 + 3] = alpha;
		}
	}
	::SetDIBits(hdc_bitmap_orig, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

	Mat template_mat(height, width, CV_8UC4);
	template_mat.data = pBits;

	// Clean up
	delete[]pBits;
	SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
	DeleteObject(bitmap_transform_ocr);
	DeleteDC(hdc_bitmap_transform_ocr);

	SelectObject(hdc_bitmap_orig, old_bitmap_orig);
	DeleteDC(hdc_bitmap_orig);

	DeleteDC(hdcScreen);


	roi = detectTemplate(area_mat, template_mat, match_mode);
	if (!EqualRect(&roi, &zero_rect)) {
		if (area_name.Find("area_buttons_zone") != -1) {
			*rect_result = roi;
			return "true";
		}
	}

	*rect_result = RECT{ 0 };
	return "";
}

CString CDlgTableMap::GetDetectTemplatesResult(CString area_name) {
	//  Detect template
	RMapCI				r_iter = p_tablemap->r$()->find(area_name.GetString());
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();

	// Exit because the area doesn't exist
	if (r_iter == p_tablemap->r$()->end())
		return "";

	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text, selected_transform, separation;
	HDC					hdcControl, hdcScreen, hdc_bitmap_orig, hdc_bitmap_transform_ocr;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform_ocr;
	// Go calc the result and display it
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);
	old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, pDoc->attached_bitmap);

	// Get bitmap size
	int w = r_iter->second.right - r_iter->second.left + 1;
	int h = r_iter->second.bottom - r_iter->second.top + 1;

	if (w <= 1 || h <= 1 || m_DrawRect.m_bState == true) {
		SelectObject(hdc_bitmap_orig, old_bitmap_orig);
		DeleteDC(hdc_bitmap_orig);

		DeleteDC(hdcScreen);
		return "";
	}

	hdc_bitmap_transform_ocr = CreateCompatibleDC(hdcScreen);
	bitmap_transform_ocr = CreateCompatibleBitmap(hdcScreen, w, h);
	old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform_ocr, bitmap_transform_ocr);

	BitBlt(hdc_bitmap_transform_ocr, 0, 0, w, h,
		hdc_bitmap_orig,
		r_iter->second.left - 1, r_iter->second.top - 1,
		SRCCOPY);

	Mat area_mat(h, w, CV_8UC4);
	BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
	GetDIBits(hdc_bitmap_transform_ocr, bitmap_transform_ocr, 0, h, area_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);


	int					x, y, width, height, match_mode;
	HBITMAP				bitmap_image, old_bitmap_image, bitmap_control, old_bitmap_control;
	BYTE* pBits, alpha, red, green, blue;
	RECT roi, zero = RECT{ 0 };
	vector<pair<int, CString>> listROI;

	// Get search templates in area
	for (sel_template = p_tablemap->tpl$()->begin(); sel_template != p_tablemap->tpl$()->end(); sel_template++) {
		if (!sel_template->second.created)
			continue;
		if (area_name == "area_cards_common" && sel_template->second.name.Find("card_common_") == -1)
			continue;
		if (area_name.Find("area_cards_player") != -1 && sel_template->second.name.Find("card_player_") == -1)
			continue;
		// Get template size
		width = sel_template->second.width;
		height = sel_template->second.height;
		match_mode = sel_template->second.match_mode;
		if (width < 1 || height < 1 || match_mode < 0)
			continue;

		// Create new memory DC and new bitmap
		bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
		old_bitmap_image = (HBITMAP)SelectObject(hdc_bitmap_orig, bitmap_image);

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
		for (y = 0; y < (int)height; y++) {
			for (x = 0; x < (int)width; x++) {
				// image record is stored internally in ABGR format
				alpha = (sel_template->second.pixel[y * width + x] >> 24) & 0xff;
				red = (sel_template->second.pixel[y * width + x] >> 0) & 0xff;
				green = (sel_template->second.pixel[y * width + x] >> 8) & 0xff;
				blue = (sel_template->second.pixel[y * width + x] >> 16) & 0xff;

				// SetDIBits format is BGRA
				pBits[y * width * 4 + x * 4 + 0] = blue;
				pBits[y * width * 4 + x * 4 + 1] = green;
				pBits[y * width * 4 + x * 4 + 2] = red;
				pBits[y * width * 4 + x * 4 + 3] = alpha;
			}
		}
		::SetDIBits(hdc_bitmap_orig, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

		Mat template_mat(height, width, CV_8UC4);
		template_mat.data = pBits;

		roi = detectTemplate(area_mat, template_mat, match_mode);
		if (!EqualRect(&roi, &zero)) {
			if (area_name == "area_cards_common") {
				CString value = sel_template->second.name;
				value.Replace("card_common_", "");
				pair<int, CString> matchPair(roi.left, value);
				listROI.push_back(matchPair);
			}
			if (area_name.Find("area_cards_player") != -1) {
				CString value = sel_template->second.name;
				value.Replace("card_player_", "");
				pair<int, CString> matchPair(roi.left, value);
				listROI.push_back(matchPair);
			}
			if (area_name.Find("area_buttons_zone") != -1) {
				CString value = sel_template->second.name;
				value.Replace("button_action_", "");
				value.Replace("button_", "");
				pair<int, CString> matchPair(roi.left, value);
				listROI.push_back(matchPair);
			}
		}
	}

	// Order ROI array
	sort(listROI.begin(), listROI.end());

	// Display result
	CString result;
	for (auto& element : listROI) {
		result.Append(element.second + " ");
	}


	// Clean up
	delete[]pBits;
	SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
	DeleteObject(bitmap_transform_ocr);
	DeleteDC(hdc_bitmap_transform_ocr);

	SelectObject(hdc_bitmap_orig, old_bitmap_orig);
	DeleteDC(hdc_bitmap_orig);

	DeleteDC(hdcScreen);

	return result;
}

RECT CDlgTableMap::detectTemplate(Mat area, Mat tpl, int match_mode) {
	//  Detect template	
	RECT result;
	int result_cols = area.cols - tpl.cols + 1;
	int result_rows = area.rows - tpl.rows + 1;
	// Invalid template size > area size
	if (result_cols < 1 || result_rows < 1)
		return result = RECT{ 0 };
	Mat match_roi = Mat(result_rows, result_cols, CV_32FC1);
	// Do the Matching and Normalize
	matchTemplate(area, tpl, match_roi, match_mode);
	//normalize(match_roi, match_roi, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double matchVal, minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;

	minMaxLoc(match_roi, &minVal, &maxVal, &minLoc, &maxLoc);

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (match_mode == TM_SQDIFF || match_mode == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc;
		matchVal = minVal;
	}
	else
	{
		matchLoc = maxLoc;
		matchVal = maxVal;
	}

	// Exact match found
	if (-0.02 <= matchVal && matchVal <= 0.02 || 0.98 <= matchVal && matchVal <= 1.02) {  // 0.02% tolerance
		result = RECT{ matchLoc.x, matchLoc.y , matchLoc.x + tpl.cols, matchLoc.y + tpl.rows };
	}
	else
		// No match found
		result = RECT{ 0 };

	return result;
}

//////////////////////////////////////////////////////////////////////////


void CDlgTableMap::update_ocr_r$_display(void) {
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text, selected_transform, separation;
	CDC* pDC;
	HDC					hdcControl, hdcScreen, hdc_bitmap_orig, hdc_bitmap_transform_ocr;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform_ocr;
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (sel_region != p_tablemap->r$()->end()) {
		// Transform
		if (sel_region->second.transform == "I")		selected_transform = "Image";
		else if (sel_region->second.transform == "A0")		selected_transform = "AutoOcr0";
		else if (sel_region->second.transform == "A1")		selected_transform = "AutoOcr1";
	}

	// Auto-Ocr processing
	if (sel_template != p_tablemap->tpl$()->end())
	{
		m_UseDefault.SetCheck(sel_template->second.use_default);
		if (m_UseDefault.GetCheck()) {
			m_MatchMode.SetCurSel(kDefaultMatchMode);
		}
		else {
			m_MatchMode.SetCurSel(sel_template->second.match_mode);
		}
		if (sel_template->second.created) {
			m_Result.SetWindowText("Created");
			m_CreateFont.EnableWindow(true);
		}
		else {
			m_Result.SetWindowText("Not Created");
			m_CreateFont.EnableWindow(false);
		}
	}

	if (selected_transform == "Image") {
		m_UseDefault.SetCheck(sel_region->second.use_default);
		if (m_UseDefault.GetCheck()) {
			text.Format("%d", kDefaultInbuiltThreshold);
			m_Threshold.SetWindowText(text);
		}
		else {
			text.Format("%d", sel_region->second.threshold);
			m_Threshold.SetWindowText(text);
		}
	}

	if (selected_transform.Find("AutoOcr") != -1) {
		m_UseDefault.SetCheck(sel_region->second.use_default);
		m_UseCrop.SetCheck(sel_region->second.use_cropping);
		if (m_UseDefault.GetCheck()) {
			text.Format("%d", kDefaultAutoOcrThreshold);
			m_Threshold.SetWindowText(text);
		}
		else {
			text.Format("%d", sel_region->second.threshold);
			m_Threshold.SetWindowText(text);
		}

		if (!m_UseCrop.GetCheck()) {
			text.Format("%d", kDefaultCropSize);
			m_CropSize.SetWindowText(text);
			m_BoxColor.SetCurSel(kDefaultBoxColor);
		}
		else {
			text.Format("%d", sel_region->second.crop_size);
			m_CropSize.SetWindowText(text);
			m_BoxColor.SetCurSel(sel_region->second.box_color);
		}
	}

	if (m_UseCrop.GetCheck()) {
		m_CropSize.EnableWindow(true);
		m_CropSpin.EnableWindow(true);
		m_BoxColor.EnableWindow(true);
	}
	else {
		m_CropSize.EnableWindow(false);
		m_CropSpin.EnableWindow(false);
		m_BoxColor.EnableWindow(false);
	}

	if (!m_UseDefault.GetCheck()) {
		if (type_text == "Templates") {
			m_MatchMode.EnableWindow(true);
		}
		if (selected_transform == "Image" ||
			selected_transform.Find("AutoOcr") != -1) {
			m_Threshold.EnableWindow(true);
			m_ThresholdSpin.EnableWindow(true);
		}
	}
	else {
		m_Threshold.EnableWindow(false);
		m_ThresholdSpin.EnableWindow(false);
		m_MatchMode.EnableWindow(false);
	}


	if (sel_region != p_tablemap->r$()->end()) {
		// Go calc the result and display it
		pDC = m_BitmapFrame.GetDC();
		hdcControl = *pDC;
		hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

		hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);
		old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, pDoc->attached_bitmap);

		int w = sel_region->second.right - sel_region->second.left + 1;
		int h = sel_region->second.bottom - sel_region->second.top + 1;

		hdc_bitmap_transform_ocr = CreateCompatibleDC(hdcScreen);
		bitmap_transform_ocr = CreateCompatibleBitmap(hdcScreen, w, h);
		old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform_ocr, bitmap_transform_ocr);

		BitBlt(hdc_bitmap_transform_ocr, 0, 0, w, h,
			hdc_bitmap_orig,
			sel_region->second.left, sel_region->second.top,
			SRCCOPY);

		// result field
		if (selected_transform.Find("AutoOcr") != -1) {
			if (w > 0 && h > 0) {
				Mat input(h, w, CV_8UC4);
				BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
				GetDIBits(hdc_bitmap_transform_ocr, bitmap_transform_ocr, 0, h, input.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
				text = get_ocr_result(input, selected_transform);
			}
			else {
				text = "";
			}
		}

		m_Result.SetWindowText(text);

		// Clean up
		SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
		DeleteObject(bitmap_transform_ocr);
		DeleteDC(hdc_bitmap_transform_ocr);

		SelectObject(hdc_bitmap_orig, old_bitmap_orig);
		DeleteDC(hdc_bitmap_orig);

		DeleteDC(hdcScreen);
		ReleaseDC(pDC);
	}
}


void CDlgTableMap::update_r$_display(bool dont_update_spinners)
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				text = "", selected_transform, separation;
	CDC* pDC;
	HDC					hdcControl, hdcScreen, hdc_bitmap_orig, hdc_bitmap_transform, hdc_bitmap_transform_ocr;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform, bitmap_transform_ocr;
	COLORREF			cr_avg = 0;
	CTransform			trans;
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	// Transform
	if (sel_region != p_tablemap->r$()->end()) {
		if (sel_region->second.transform == "C")			selected_transform = "Color";
		else if (sel_region->second.transform == "I")		selected_transform = "Image";
		else if (sel_region->second.transform == "A0")		selected_transform = "AutoOcr0";
		else if (sel_region->second.transform == "A1")		selected_transform = "AutoOcr1";
		else if (sel_region->second.transform == "T0")		selected_transform = "Text0";
		else if (sel_region->second.transform == "T1")		selected_transform = "Text1";
		else if (sel_region->second.transform == "T2")		selected_transform = "Text2";
		else if (sel_region->second.transform == "T3")		selected_transform = "Text3";
		else if (sel_region->second.transform == "T4")		selected_transform = "Text4";
		else if (sel_region->second.transform == "T5")		selected_transform = "Text5";
		else if (sel_region->second.transform == "T6")		selected_transform = "Text6";
		else if (sel_region->second.transform == "T7")		selected_transform = "Text7";
		else if (sel_region->second.transform == "T8")		selected_transform = "Text8";
		else if (sel_region->second.transform == "T9")		selected_transform = "Text9";
		else if (sel_region->second.transform == "H0")		selected_transform = "Hash0";
		else if (sel_region->second.transform == "H1")		selected_transform = "Hash1";
		else if (sel_region->second.transform == "H2")		selected_transform = "Hash2";
		else if (sel_region->second.transform == "H3")		selected_transform = "Hash3";
		else if (sel_region->second.transform == "WC")		selected_transform = "WebColour";
		else if (sel_region->second.transform == "N")		selected_transform = "None";
		m_Transform.SelectString(-1, selected_transform);
	}

	// Left/top/right/bottom edits/spinners
	if (!dont_update_spinners && sel_template != p_tablemap->tpl$()->end())
	{
		text.Format("%d", sel_template->second.left);
		m_Left.SetWindowText(text);
		m_LeftSpin.SetPos(sel_template->second.left);

		text.Format("%d", sel_template->second.top);
		m_Top.SetWindowText(text);
		m_TopSpin.SetPos(sel_template->second.top);

		text.Format("%d", sel_template->second.right);
		m_Right.SetWindowText(text);
		m_RightSpin.SetPos(sel_template->second.right);

		text.Format("%d", sel_template->second.bottom);
		m_Bottom.SetWindowText(text);
		m_BottomSpin.SetPos(sel_template->second.bottom);
	}

	// Auto-Ocr processing
	if (sel_template != p_tablemap->tpl$()->end())
	{
		m_UseDefault.SetCheck(sel_template->second.use_default);
		if (m_UseDefault.GetCheck()) {
			m_MatchMode.SetCurSel(kDefaultMatchMode);
		}
		else {
			m_MatchMode.SetCurSel(sel_template->second.match_mode);
		}
		if (sel_template->second.created) {
			m_Result.SetWindowText("Created");
			m_CreateFont.EnableWindow(true);
		}
		else {
			m_Result.SetWindowText("Not Created");
			m_CreateFont.EnableWindow(false);
		}
	}

	if (selected_transform == "Image") {
		m_UseDefault.SetCheck(sel_region->second.use_default);
		if (m_UseDefault.GetCheck()) {
			text.Format("%d", kDefaultInbuiltThreshold);
			m_Threshold.SetWindowText(text);
		}
		else {
			text.Format("%d", sel_region->second.threshold);
			m_Threshold.SetWindowText(text);
		}
	}

	if (selected_transform.Find("AutoOcr") != -1) {
		m_UseDefault.SetCheck(sel_region->second.use_default);
		m_UseCrop.SetCheck(sel_region->second.use_cropping);
		if (m_UseDefault.GetCheck()) {
			text.Format("%d", kDefaultAutoOcrThreshold);
			m_Threshold.SetWindowText(text);
		}
		else {
			text.Format("%d", sel_region->second.threshold);
			m_Threshold.SetWindowText(text);
		}

		if (!m_UseCrop.GetCheck()) {
			text.Format("%d", kDefaultCropSize);
			m_CropSize.SetWindowText(text);
			m_BoxColor.SetCurSel(kDefaultBoxColor);
		}
		else {
			text.Format("%d", sel_region->second.crop_size);
			m_CropSize.SetWindowText(text);
			m_BoxColor.SetCurSel(sel_region->second.box_color);
		}
	}

	if (m_UseCrop.GetCheck()) {
		m_CropSize.EnableWindow(true);
		m_CropSpin.EnableWindow(true);
		m_BoxColor.EnableWindow(true);
	}
	else {
		m_UseCrop.SetCheck(false);
		m_CropSize.EnableWindow(false);
		m_CropSpin.EnableWindow(false);
		m_BoxColor.EnableWindow(false);
	}

	if (!m_UseDefault.GetCheck()) {
		if (type_text == "Templates") {
			m_MatchMode.EnableWindow(true);
		}
		if (selected_transform == "Image" ||
			selected_transform.Find("AutoOcr") != -1) {
			m_Threshold.EnableWindow(true);
			m_ThresholdSpin.EnableWindow(true);
		}
	}
	else {
		m_Threshold.EnableWindow(false);
		m_ThresholdSpin.EnableWindow(false);
		m_MatchMode.EnableWindow(false);
	}

	if (sel_template != p_tablemap->tpl$()->end()) return;


	if (!dont_update_spinners && sel_region != p_tablemap->r$()->end())
	{
		text.Format("%d", sel_region->second.left);
		m_Left.SetWindowText(text);
		m_LeftSpin.SetPos(sel_region->second.left);

		text.Format("%d", sel_region->second.top);
		m_Top.SetWindowText(text);
		m_TopSpin.SetPos(sel_region->second.top);

		text.Format("%d", sel_region->second.right);
		m_Right.SetWindowText(text);
		m_RightSpin.SetPos(sel_region->second.right);

		text.Format("%d", sel_region->second.bottom);
		m_Bottom.SetWindowText(text);
		m_BottomSpin.SetPos(sel_region->second.bottom);
	}

	text.Format("%d x %d", sel_region->second.right - sel_region->second.left + 1, sel_region->second.bottom - sel_region->second.top + 1);
	m_xy.SetWindowText(text);

	// Color/radius  (color is stored internally and in the .tm file in ABGR (COLORREF) format)
	if (selected_transform.Find("AutoOcr") != -1
		|| selected_transform == "Hash0"
		|| selected_transform == "Hash1"
		|| selected_transform == "Hash2"
		|| selected_transform == "Hash3"
		|| selected_transform == "None"
		|| selected_transform == "Image"
		|| selected_transform == "WebColour") {
		m_Alpha.EnableWindow(false);
		m_Red.EnableWindow(false);
		m_Green.EnableWindow(false);
		m_Blue.EnableWindow(false);
		m_Picker.EnableWindow(false);
		m_Radius.EnableWindow(false);
	}
	else
	{
		m_Alpha.EnableWindow(true);
		m_Red.EnableWindow(true);
		m_Green.EnableWindow(true);
		m_Blue.EnableWindow(true);
		m_Picker.EnableWindow(true);
		m_Radius.EnableWindow(true);
	}

	text.Format("%02x", (sel_region->second.color >> 24) & 0xff);
	m_Alpha.SetWindowText(text);
	text.Format("%02x", (sel_region->second.color >> 0) & 0xff);
	m_Red.SetWindowText(text);
	text.Format("%02x", (sel_region->second.color >> 8) & 0xff);
	m_Green.SetWindowText(text);
	text.Format("%02x", (sel_region->second.color >> 16) & 0xff);
	m_Blue.SetWindowText(text);
	text.Format("%d", sel_region->second.radius);
	m_Radius.SetWindowText(text);

	// avg color fields
	if (selected_transform == "Color")
	{
		text.Format("%02x", GetRValue(cr_avg));
		m_RedAvg.SetWindowText(text);
		text.Format("%02x", GetGValue(cr_avg));
		m_GreenAvg.SetWindowText(text);
		text.Format("%02x", GetBValue(cr_avg));
		m_BlueAvg.SetWindowText(text);
		text.Format("%d", sel_region->second.radius);

		m_RedAvg.EnableWindow(false);
		m_GreenAvg.EnableWindow(false);
		m_BlueAvg.EnableWindow(false);
	}
	else
	{
		m_RedAvg.EnableWindow(true);
		m_GreenAvg.EnableWindow(true);
		m_BlueAvg.EnableWindow(true);
	}


	// Go calc the result and display it
	if (sel_text.Find("area") != -1) {
		// Templates detection in target area (ROI)
		int _width = sel_region->second.right - sel_region->second.left;
		int _height = sel_region->second.bottom - sel_region->second.top;
		if (_width > 0 && _height > 0)
			text = GetDetectTemplatesResult(sel_region->second.name);
	}
	else {
		pDC = m_BitmapFrame.GetDC();
		hdcControl = *pDC;
		hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

		hdc_bitmap_orig = CreateCompatibleDC(hdcScreen);
		old_bitmap_orig = (HBITMAP)SelectObject(hdc_bitmap_orig, pDoc->attached_bitmap);

		int w = sel_region->second.right - sel_region->second.left + 1;
		int h = sel_region->second.bottom - sel_region->second.top + 1;

		hdc_bitmap_transform = CreateCompatibleDC(hdcScreen);
		bitmap_transform = CreateCompatibleBitmap(hdcScreen, w, h);
		old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform, bitmap_transform);

		BitBlt(hdc_bitmap_transform, 0, 0, w, h,
			hdc_bitmap_orig,
			sel_region->second.left, sel_region->second.top,
			SRCCOPY);

		//w = w + 6;
		//h = h + 6;

		hdc_bitmap_transform_ocr = CreateCompatibleDC(hdcScreen);
		bitmap_transform_ocr = CreateCompatibleBitmap(hdcScreen, w, h);
		old_bitmap_transform = (HBITMAP)SelectObject(hdc_bitmap_transform_ocr, bitmap_transform_ocr);

		BitBlt(hdc_bitmap_transform_ocr, 0, 0, w, h,
			hdc_bitmap_orig,
			sel_region->second.left, sel_region->second.top,
			SRCCOPY);

		// result field
		if (selected_transform.Find("AutoOcr") != -1) {
			if (w > 0 && h > 0) {
				Mat input(h, w, CV_8UC4);
				BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
				GetDIBits(hdc_bitmap_transform_ocr, bitmap_transform_ocr, 0, h, input.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
				text = get_ocr_result(input, selected_transform);
			}
			else {
				text = "";
			}
		}
		else {
			int ret = trans.DoTransform(sel_region, hdc_bitmap_transform, &text, &separation, &cr_avg);
			if (ret != ERR_GOOD_SCRAPE_GENERAL)
			{
				switch (ret)
				{
				case ERR_FIELD_TOO_LARGE:
					text.Format("Err: Field too large");
					break;
				case ERR_NO_HASH_MATCH:
					text.Format("Err: No hash match");
					break;
				case ERR_TEXT_SCRAPE_NOMATCH:
					if (separation.Find("X") == -1)
						text.Format("Err: No foreground pixels");
					else
						text.Format("Err: No text match");
					break;
				case ERR_NO_IMAGE_MATCH:
					text.Format("Err: No image match");
					break;
				default:
					text.Format("Err: %d", ret);
					break;
				}
			}
		}

		// Clean up
		SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
		DeleteObject(bitmap_transform_ocr);
		DeleteDC(hdc_bitmap_transform_ocr);

		SelectObject(hdc_bitmap_transform, old_bitmap_transform);
		DeleteObject(bitmap_transform);
		DeleteDC(hdc_bitmap_transform);

		SelectObject(hdc_bitmap_orig, old_bitmap_orig);
		DeleteDC(hdc_bitmap_orig);

		DeleteDC(hdcScreen);
		ReleaseDC(pDC);
	}

	// Display result
	m_Result.SetWindowText(text);

	// pixel separation field
	if (selected_transform.Find("Text") == -1)  separation = "";
	m_PixelSeparation.SetWindowText(separation);
}

void CDlgTableMap::update_t$_display(void)
{
	int					x, bit, top;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString				separation, text, charstr;
	bool				character[MAX_CHAR_WIDTH][MAX_CHAR_HEIGHT] = { false };
	TMapCI				sel_font = p_tablemap->t$(0)->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get record lookup info from name
	int font_type = GetType(sel_text);
	if (font_type<0 || font_type>k_max_number_of_font_groups_in_tablemap - 1)
		return;

	CString hexmash = sel_text.Mid(sel_text.Find("[") + 1, sel_text.Find("]") - sel_text.Find("[") - 1);

	// Get iterator for selected font
	sel_font = p_tablemap->t$(font_type)->find(hexmash.GetString());

	// Exit if we can't find the font record
	if (sel_font == p_tablemap->t$(font_type)->end())
		return;

	// Get set bits
	bit = 0;
	for (int j = 0; j < sel_font->second.x_count; j++)
		for (bit = 0; bit < MAX_CHAR_HEIGHT; bit++)
			character[j][bit] = (sel_font->second.x[j] & (int)pow((double)2, (double)bit)) != 0;

	// Find topmost line with a set pixel
	for (int j = MAX_CHAR_HEIGHT - 1; j >= 0; j--)
	{
		for (x = 0; x < sel_font->second.x_count; x++)
		{
			if (character[x][j])
			{
				top = j;
				x = sel_font->second.x_count + 1;
				j = -1;
			}
		}
	}

	// Create string of set pixels
	separation = "";
	for (int j = top; j >= 0; j--)
	{
		for (x = 0; x < sel_font->second.x_count; x++)
		{
			if (character[x][j])
				separation.Append("X");
			else
				separation.Append(" ");
		}
		separation.Append("\r\n");
	}

	// Update pixel separation control
	m_PixelSeparation.SetWindowText(separation);
}

void CDlgTableMap::OnDeltaposLeftSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_region != p_tablemap->r$()->end())
		sel_region->second.left = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (sel_template != p_tablemap->tpl$()->end())
		sel_template->second.left = pNMUpDown->iPos + pNMUpDown->iDelta;

	update_r$_display(true);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposTopSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_region != p_tablemap->r$()->end())
		sel_region->second.top = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (sel_template != p_tablemap->tpl$()->end())
		sel_template->second.top = pNMUpDown->iPos + pNMUpDown->iDelta;

	update_r$_display(true);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposBottomSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_region != p_tablemap->r$()->end())
		sel_region->second.bottom = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (sel_template != p_tablemap->tpl$()->end())
		sel_template->second.bottom = pNMUpDown->iPos + pNMUpDown->iDelta;

	update_r$_display(true);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposRightSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	if (ignore_changes)  return;

	if (sel_region != p_tablemap->r$()->end())
		sel_region->second.right = pNMUpDown->iPos + pNMUpDown->iDelta;
	if (sel_template != p_tablemap->tpl$()->end())
		sel_template->second.right = pNMUpDown->iPos + pNMUpDown->iDelta;

	update_r$_display(true);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposRadiusSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	// Exit if we can't find the region record
	if (sel_region == p_tablemap->r$()->end())
		return;

	if (ignore_changes)  return;

	sel_region->second.radius = pNMUpDown->iPos + pNMUpDown->iDelta;

	update_r$_display(true);
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposThresholdSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	// Exit if we can't find the region record
	if (sel_region == p_tablemap->r$()->end())
		return;

	if (ignore_changes)  return;

	sel_region->second.threshold = pNMUpDown->iPos + pNMUpDown->iDelta;

	//update_ocr_display();
	update_ocr_r$_display();
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnDeltaposCropSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMUPDOWN			pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	// Exit if we can't find the region record
	if (sel_region == p_tablemap->r$()->end())
		return;

	if (ignore_changes)  return;

	sel_region->second.crop_size = pNMUpDown->iPos + pNMUpDown->iDelta;

	//update_ocr_display();
	update_ocr_r$_display();
	Invalidate(false);
	theApp.m_pMainWnd->Invalidate(false);

	pDoc->SetModifiedFlag(true);

	*pResult = 0;
}

void CDlgTableMap::OnBnClickedNew() {
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CString	sel_text, type_text;
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);
	if (type_text == "Sizes") {
		// Prep dialog
		CDlgEditSizes dlgsizes;
		dlgsizes.titletext = "New Size record";
		dlgsizes.name = "";
		dlgsizes.width = 0;
		dlgsizes.height = 0;
		dlgsizes.strings.RemoveAll();

		ZMap::const_iterator z_iter;
		for (int i = 0; i < list_of_sizes.size(); i++) {
			bool used_string = false;
			for (z_iter = p_tablemap->z$()->begin(); z_iter != p_tablemap->z$()->end(); z_iter++) {
				if (z_iter->second.name == list_of_sizes[i]) {
					used_string = true;
				}
			}
			if (!used_string) {
				dlgsizes.strings.Add(list_of_sizes[i]);
			}
		}
		// Show dialog if there are any strings left to add
		if (dlgsizes.strings.GetSize() == 0) {
			MessageBox("All Size records are already present.");
		}
		else {
			if (dlgsizes.DoModal() == IDOK && dlgsizes.name != "") {
				// Add new record to internal structure
				STablemapSize new_size;
				new_size.name = dlgsizes.name;
				new_size.width = dlgsizes.width;
				new_size.height = dlgsizes.height;
				// Insert the new record in the existing array of z$ records
				if (!p_tablemap->z$_insert(new_size)) {
					MessageBox("Failed to create size record.", "Size creation error", MB_OK);
				}
				else {
					// Add new record to tree
					HTREEITEM new_hti = m_TableMapTree.InsertItem(dlgsizes.name, type_node ? type_node : m_TableMapTree.GetSelectedItem());
					m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
					m_TableMapTree.SelectItem(new_hti);

					pDoc->SetModifiedFlag(true);
					Invalidate(false);
				}
			}
		}
	}
	else if (type_text == "Symbols") {
		// Prep dialog
		CDlgEditSymbols dlgsymbols;
		dlgsymbols.titletext = "New Symbol record";
		dlgsymbols.name = "";
		dlgsymbols.value = "";
		char title[200];
		::GetWindowText(pDoc->attached_hwnd, title, 200);
		dlgsymbols.titlebartext = title;
		dlgsymbols.strings.RemoveAll();

		SMap::const_iterator s_iter;
		for (int i = 0; i < list_of_symbols.size(); i++) {
			bool used_string = false;
			for (s_iter = p_tablemap->s$()->begin(); s_iter != p_tablemap->s$()->end(); s_iter++) {
				if (list_of_symbols[i] == "" || s_iter->second.name == list_of_symbols[i]) {
					used_string = true;
				}
			}
			if (!used_string) {
				dlgsymbols.strings.Add(list_of_symbols[i]);
			}
		}
		// Show dialog if there are any strings left to add
		if (dlgsymbols.strings.GetSize() == 0)
		{
			MessageBox("All Symbol records are already present.");
		}
		else
		{
			if (dlgsymbols.DoModal() == IDOK && dlgsymbols.name != "")
			{
				// Add new record to internal structure
				STablemapSymbol new_symbol;
				new_symbol.name = dlgsymbols.name;
				new_symbol.text = dlgsymbols.value;

				// Insert the new record in the existing array of z$ records
				if (!p_tablemap->s$_insert(new_symbol))
				{
					MessageBox("Failed to create symbol record.", "Symbol creation error", MB_OK);
				}
				else
				{
					// Add new record to tree
					HTREEITEM new_hti = m_TableMapTree.InsertItem(new_symbol.name, type_node ? type_node : m_TableMapTree.GetSelectedItem());
					m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
					m_TableMapTree.SelectItem(new_hti);

					pDoc->SetModifiedFlag(true);
					Invalidate(false);
				}
			}
		}
	}
	else if (type_text == "Regions") {
		// Prep dialog
		CDlgEditRegion dlgregions;
		dlgregions.titletext = "New Region record";
		dlgregions.labeltext = "Record name:";
		dlgregions.name = "";
		dlgregions.strings.RemoveAll();
		assert(list_of_regions[list_of_regions.size() - 1] != "");
		for (int i = 0; i < list_of_regions.size(); i++) {
			// Ignore empty strings
			// This should no longer happen with the new way we build lists
			// but we keep this checl, as the consequences of empty symbols were nasty.
			if (list_of_regions[i] == "") continue;
			bool used_string = false;
			for (RMapCI r_iter = p_tablemap->r$()->begin(); r_iter != p_tablemap->r$()->end(); r_iter++) {
				CString tablemap_string = r_iter->second.name;
				CString allowed_string = list_of_regions[i];
				assert(tablemap_string != "");
				assert(allowed_string != "");
				if (tablemap_string == "") continue;
				if (tablemap_string == allowed_string) {
					used_string = true;
					break;
				}
			}

			if (!used_string)
				dlgregions.strings.Add(list_of_regions[i]);
		}

		// Show dialog if there are any strings left to add
		if (dlgregions.strings.GetSize() == 0)
		{
			MessageBox("All Region records are already present.");
		}
		else
		{
			if (dlgregions.DoModal() == IDOK && dlgregions.name != "")
			{
				// Add new record to internal structure
				STablemapRegion new_region;
				new_region.name = dlgregions.name;
				new_region.left = 0;
				new_region.top = 0;
				new_region.right = 0;
				new_region.bottom = 0;
				new_region.color = 0;
				new_region.radius = 0;
				new_region.transform = "N";
				new_region.use_default = true;
				new_region.threshold = 0;
				new_region.use_cropping = false;
				new_region.crop_size = 0;
				new_region.box_color = -1;

				// Insert the new record in the existing array of z$ records
				if (!p_tablemap->r$_insert(new_region))
				{
					MessageBox("Failed to create region record.", "Region creation error", MB_OK);
				}
				else
				{

					// Add new record to tree
					HTREEITEM new_hti = NULL;
					new_hti = InsertGroupedRegion(new_region.name);
					m_TableMapTree.SelectItem(new_hti);
					pDoc->SetModifiedFlag(true);
					Invalidate(false);
				}
			}
		}
	}

	else if (type_text == "Fonts")
	{
		// Not valid - add font character using "Create Font" button
	}

	else if (type_text == "Hash Points")
	{
		// Prep dialog
		CDlgEditHashPoint dlghashpoint;
		dlghashpoint.titletext = "New Hash Point record";
		dlghashpoint.type = "Type 0";
		dlghashpoint.x = 0;
		dlghashpoint.y = 0;

		// Show dialog
		if (dlghashpoint.DoModal() == IDOK)
		{
			// Add new record to internal structure
			int type = atoi(dlghashpoint.type.Mid(5, 1).GetString());
			STablemapHashPoint new_hashpoint;
			new_hashpoint.x = dlghashpoint.x;
			new_hashpoint.y = dlghashpoint.y;

			// Insert the new record in the existing array of p$ records
			if (!p_tablemap->p$_insert(type, new_hashpoint))
			{
				MessageBox("Failed to create hash point record.", "Hash point creation error", MB_OK);
			}
			else
			{
				// Add new record to tree
				CString text;
				text.Format("%d (%d, %d)", type, new_hashpoint.x, new_hashpoint.y);
				HTREEITEM new_hti = m_TableMapTree.InsertItem(text.GetString(), type_node ? type_node : m_TableMapTree.GetSelectedItem());
				m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
				m_TableMapTree.SelectItem(new_hti);

				pDoc->SetModifiedFlag(true);
				Invalidate(false);
			}
		}
	}

	else if (type_text == "Hashes")
	{
		// Not valid - add hash using "Create Hash" button
	}

	else if (type_text == "Images")
	{
		// Not valid - add image using "Create Image" button
	}

	else if (type_text == "Templates")
	{
		// Prep dialog
		CDlgEditRegion dlgtemplates;
		dlgtemplates.titletext = "New Template record";
		dlgtemplates.labeltext = "Template name:";
		dlgtemplates.name = "";
		//dlgtemplates.value = "";
		char title[200];
		::GetWindowText(pDoc->attached_hwnd, title, 200);
		//dlgtemplates.titlebartext = title;
		dlgtemplates.strings.RemoveAll();

		TPLMap::const_iterator tpl_iter;
		for (int i = 0; i < list_of_templates.size(); i++) {
			bool used_string = false;
			for (tpl_iter = p_tablemap->tpl$()->begin(); tpl_iter != p_tablemap->tpl$()->end(); tpl_iter++) {
				if (list_of_templates[i] == "" || tpl_iter->second.name == list_of_templates[i]) {
					used_string = true;
				}
			}
			if (!used_string) {
				dlgtemplates.strings.Add(list_of_templates[i]);
			}
		}
		/* Show dialog if there are any strings left to add
		if (dlgtemplates.strings.GetSize() == 0)
		{
			MessageBox("All Template records are already present.");
		}
		else
		{*/
		if (dlgtemplates.DoModal() == IDOK && dlgtemplates.name != "")
		{
			/*
			if (dlgtemplates.name.Find("card_", 0) == -1 && dlgtemplates.name.Find("button_", 0) == -1) {
				MessageBox("Template name is invalid.\nPlease choose one of the list.");
				return;
			}
			*/
			// Add new record to internal structure
			STablemapTemplate new_template;
			new_template.name = dlgtemplates.name;
			new_template.left = 0;
			new_template.top = 0;
			new_template.right = 0;
			new_template.bottom = 0;
			new_template.width = 0;
			new_template.height = 0;
			new_template.use_default = true;
			new_template.match_mode = -1;
			new_template.created = false;

			// Insert the new record in the existing array of z$ records
			if (!p_tablemap->tpl$_insert(new_template))
			{
				MessageBox("Failed to create template record.", "Template creation error", MB_OK);
			}
			else
			{
				// Add new record to tree
				HTREEITEM new_hti = m_TableMapTree.InsertItem(new_template.name, type_node ? type_node : m_TableMapTree.GetSelectedItem());
				m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
				m_TableMapTree.SelectItem(new_hti);

				pDoc->SetModifiedFlag(true);
				Invalidate(false);
			}
		}
		//}
	}
}

int CDlgTableMap::GetType(CString selected_text)
{
	int type = 0;
	// Special case: The character "(", as here the string starts with "( (",
	// e.g. "( (0), whereas the normal case is e..g. "x (0)".
	CString LeftSide = selected_text.Left(3);
	if (LeftSide == "( (")
	{
		type = strtoul(selected_text.Mid(selected_text.Find("( (") + 3, 1).GetString(), NULL, 10);
	}
	else
	{
		// Normal case:
		type = strtoul(selected_text.Mid(selected_text.Find("(") + 1, 1).GetString(), NULL, 10);
	}
	return type;
}

void CDlgTableMap::OnBnClickedDelete()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	bool				item_deleted = false;

	CString	sel_text, type_text;
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// If this is a parent/group item, return immediately
	if (type_node == NULL || m_TableMapTree.ItemHasChildren(m_TableMapTree.GetSelectedItem()))
		return;

	// If this is not a valid record type (like a region group), then return
	CString		valid_types("Sizes|Symbols|Regions|Fonts|Hash Points|Hashes|Images|Templates");
	if (valid_types.Find(type_text.GetString()) == -1)
		return;

	CString region_text = type_text.Left(type_text.GetLength() - 1);
	CString caption_text, text;
	caption_text.Format("Delete %s record?", region_text);
	text.Format("Really delete %s record: %s", region_text, sel_text);

	if (MessageBox(text.GetString(), caption_text.GetString(), MB_YESNO) == IDNO)
		return;

	if (type_text == "Sizes")
	{
		if (p_tablemap->z$_erase(sel_text))
			item_deleted = true;
	}

	else if (type_text == "Symbols")
	{
		if (p_tablemap->s$_erase(sel_text))
			item_deleted = true;
	}

	else if (type_text == "Regions") {
		if (p_tablemap->r$_erase(sel_text)) {
			item_deleted = true;
		}
	}

	else if (type_text == "Fonts")
	{
		int font_type = GetType(sel_text);
		if (font_type<0 || font_type>k_max_number_of_font_groups_in_tablemap - 1)
			return;

		CString hexmash = sel_text.Mid(sel_text.Find("[") + 1, sel_text.Find("]") - sel_text.Find("[") - 1);

		if (p_tablemap->t$_erase(font_type, hexmash))
			item_deleted = true;
	}

	else if (type_text == "Hash Points")
	{
		int hashpoint_type = strtoul(sel_text.Mid(0, 1).GetString(), NULL, 10);
		int x = strtoul(sel_text.Mid(3, sel_text.Find(",") - 3).GetString(), NULL, 10);
		int y = strtoul(sel_text.Mid(sel_text.Find(",") + 2, sel_text.Find(")") - sel_text.Find(",") + 2).GetString(), NULL, 10);

		if (p_tablemap->p$_erase(hashpoint_type, ((x & 0xffff) << 16) | (y & 0xffff)))
			item_deleted = true;
	}

	else if (type_text == "Hashes")
	{
		int hash_type = GetType(sel_text);
		for (HMapCI h_iter = p_tablemap->h$(hash_type)->begin(); h_iter != p_tablemap->h$(hash_type)->end(); h_iter++)
		{
			int start = 0;
			if (h_iter->second.name == sel_text.Tokenize(" ", start))
			{

				if (p_tablemap->h$_erase(hash_type, h_iter->second.hash))
					item_deleted = true;
				break;
			}
		}
	}

	else if (type_text == "Images")
	{
		// Get index into array for selected record
		int index = (int)m_TableMapTree.GetItemData(m_TableMapTree.GetSelectedItem());

		if (p_tablemap->i$_erase(index))
			item_deleted = true;
	}

	else if (type_text == "Templates")
	{
		if (p_tablemap->tpl$_erase(sel_text))
			item_deleted = true;
		//deleteTemplate(sel_text.GetString());
		//pDoc->DoFileSave();
	}

	if (!item_deleted)
	{
		CString text;
		text.Format("Error deleting %s record.", region_text);
		MessageBox(text.GetString(), "ERROR", MB_OK | MB_TOPMOST);
		return;
	}
	else
	{
		HTREEITEM prev = m_TableMapTree.GetPrevSiblingItem(m_TableMapTree.GetSelectedItem());
		HTREEITEM prev_prev = NULL;

		if (prev == NULL)
		{
			prev = m_TableMapTree.GetParentItem(m_TableMapTree.GetSelectedItem());

			if (m_TableMapTree.GetParentItem(prev) != NULL)
				prev_prev = m_TableMapTree.GetParentItem(prev);
		}

		HTREEITEM item = m_TableMapTree.GetSelectedItem();
		m_TableMapTree.SelectItem(prev);
		m_TableMapTree.DeleteItem(item);

		if (prev_prev != NULL && !m_TableMapTree.ItemHasChildren(prev))
		{
			m_TableMapTree.DeleteItem(prev);
			m_TableMapTree.SelectItem(prev_prev);
		}

		Invalidate(false);
		pDoc->SetModifiedFlag(true);
	}
}

void CDlgTableMap::SaveNodeExpansionState(CArray <bool, bool>* node_state)
{
	node_state->RemoveAll();
	HTREEITEM node = m_TableMapTree.GetChildItem(NULL);
	while (node != NULL)
	{
		node_state->Add((m_TableMapTree.GetItemState(node, TVIS_EXPANDED) & TVIS_EXPANDED) != 0);
		node = m_TableMapTree.GetNextItem(node, TVGN_NEXT);
	}
}

void CDlgTableMap::RestoreNodeExpansionState(CArray <bool, bool>* node_state)
{
	int i = 0;

	HTREEITEM node = m_TableMapTree.GetChildItem(NULL);
	while (node != NULL && i < node_state->GetCount())
	{
		m_TableMapTree.Expand(node, node_state->GetAt(i) ? TVE_EXPAND : TVE_COLLAPSE);
		i++;
		node = m_TableMapTree.GetNextItem(node, TVGN_NEXT);
	}
}

HTREEITEM CDlgTableMap::update_tree(CString node_text)
{
	CArray <bool, bool> node_state;
	HTREEITEM		node;
	CString			text;

	// Save expanded state of each node
	SaveNodeExpansionState(&node_state);

	// Recreate the tree
	create_tree();

	// Restore expanded state of each node
	RestoreNodeExpansionState(&node_state);

	// Find root node and return it
	node = m_TableMapTree.GetChildItem(NULL);
	text = m_TableMapTree.GetItemText(node);

	while (text != node_text && node != NULL)
	{
		node = m_TableMapTree.GetNextItem(node, TVGN_NEXT);
		text = m_TableMapTree.GetItemText(node);
	}
	return node;
}

void CDlgTableMap::OnBnClickedEdit()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	if (type_text == "Sizes")
	{
		// Get selected size record
		ZMapCI z_iter = p_tablemap->z$()->find(sel_text.GetString());

		if (z_iter == p_tablemap->z$()->end())
		{
			MessageBox("Error editing record - not found.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Prep dialog
		CDlgEditSizes dlgsizes;
		dlgsizes.titletext = "Edit Size record";
		dlgsizes.name = z_iter->second.name;
		dlgsizes.width = z_iter->second.width;
		dlgsizes.height = z_iter->second.height;

		dlgsizes.strings.RemoveAll();
		for (int i = 0; i < list_of_sizes.size(); i++)
			dlgsizes.strings.Add(list_of_sizes[i]);

		// Show dialog
		if (dlgsizes.DoModal() != IDOK)
			return;

		// If key is changed, search for new key, and error out if found
		if (dlgsizes.name != z_iter->second.name &&
			p_tablemap->z$()->find(dlgsizes.name) != p_tablemap->z$()->end())
		{
			MessageBox("Error editing record - name already exists.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Delete original record in internal structure
		if (!p_tablemap->z$_erase(sel_text))
		{
			MessageBox("Error applying changes.", "ERROR (1)", MB_OK | MB_TOPMOST);
			return;
		}

		// Add new record to internal structure
		STablemapSize new_size;
		new_size.name = dlgsizes.name;
		new_size.width = dlgsizes.width;
		new_size.height = dlgsizes.height;

		if (!p_tablemap->z$_insert(new_size))
		{
			MessageBox("Error applying changes.", "ERROR (2)", MB_OK | MB_TOPMOST);
			return;
		}

		// Update record in tree
		m_TableMapTree.SetItemText(m_TableMapTree.GetSelectedItem(), new_size.name.GetString());
		m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
		m_TableMapTree.SelectItem(m_TableMapTree.GetSelectedItem());

		update_display();
		Invalidate(false);
		pDoc->SetModifiedFlag(true);
	}

	else if (type_text == "Symbols")
	{
		// Get selected symbol record
		SMapCI s_iter = p_tablemap->s$()->find(sel_text.GetString());

		if (s_iter == p_tablemap->s$()->end())
		{
			MessageBox("Error editing record - not found.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Prep dialog
		CDlgEditSymbols dlgsymbols;
		dlgsymbols.titletext = "Edit Symbol record";
		dlgsymbols.name = s_iter->second.name;
		dlgsymbols.value = s_iter->second.text;
		char title[200];
		::GetWindowText(pDoc->attached_hwnd, title, 200);
		dlgsymbols.titlebartext = title;

		dlgsymbols.strings.RemoveAll();
		for (int i = 0; i < list_of_symbols.size(); i++)
			dlgsymbols.strings.Add(list_of_symbols[i]);

		// Show dialog
		if (dlgsymbols.DoModal() != IDOK)
			return;

		// If key is changed, search for new key, and error out if found
		CString old_name = s_iter->second.name;
		if (dlgsymbols.name != old_name
			&& p_tablemap->s$()->find(dlgsymbols.name) != p_tablemap->s$()->end())
		{
			MessageBox("Error editing record - name already exists.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Delete original record in internal structure
		if (!p_tablemap->s$_erase(sel_text))
		{
			MessageBox("Error applying changes.", "ERROR (1)", MB_OK | MB_TOPMOST);
			return;
		}

		// Add new record to internal structure
		STablemapSymbol new_symbol;
		new_symbol.name = dlgsymbols.name;
		new_symbol.text = dlgsymbols.value;

		if (!p_tablemap->s$_insert(new_symbol))
		{
			MessageBox("Error applying changes.", "ERROR (2)", MB_OK | MB_TOPMOST);
			return;
		}

		// Update record in tree
		m_TableMapTree.SetItemText(m_TableMapTree.GetSelectedItem(), new_symbol.name.GetString());
		m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
		m_TableMapTree.SelectItem(m_TableMapTree.GetSelectedItem());

		update_display();
		Invalidate(false);
		pDoc->SetModifiedFlag(true);
	}

	else if (type_text == "Regions")
	{
		// Get selected region record
		RMapCI r_iter = p_tablemap->r$()->find(sel_text.GetString());

		if (r_iter == p_tablemap->r$()->end())
		{
			MessageBox("Error editing record - not found.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Prep dialog
		CDlgEditRegion dlgregion;
		dlgregion.titletext = "Edit Region record";
		dlgregion.labeltext = "Record name:";
		dlgregion.name = r_iter->second.name;

		dlgregion.strings.RemoveAll();
		for (int i = 0; i < list_of_regions.size(); i++)
			dlgregion.strings.Add(list_of_regions[i]);

		// Show dialog
		if (dlgregion.DoModal() != IDOK)
			return;

		// If key is changed, search for new key, and error out if found
		if (dlgregion.name != r_iter->second.name &&
			p_tablemap->r$()->find(dlgregion.name) != p_tablemap->r$()->end())
		{
			MessageBox("Error editing record - name already exists.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Delete original record in internal structure
		if (!p_tablemap->r$_erase(sel_text))
		{
			MessageBox("Error applying changes.", "ERROR (1)", MB_OK | MB_TOPMOST);
			return;
		}

		// Add new record to internal structure
		STablemapRegion new_region;
		new_region.name = dlgregion.name;
		new_region.left = r_iter->second.left;
		new_region.top = r_iter->second.top;
		new_region.right = r_iter->second.right;
		new_region.bottom = r_iter->second.bottom;
		new_region.color = r_iter->second.color;
		new_region.radius = r_iter->second.radius;
		new_region.transform = r_iter->second.transform;
		new_region.cur_bmp = r_iter->second.cur_bmp;
		new_region.last_bmp = r_iter->second.last_bmp;
		new_region.use_default = r_iter->second.use_default;
		new_region.threshold = r_iter->second.threshold;
		new_region.use_cropping = r_iter->second.use_cropping;
		new_region.crop_size = r_iter->second.crop_size;
		new_region.box_color = r_iter->second.box_color;

		if (!p_tablemap->r$_insert(new_region))
		{
			MessageBox("Error applying changes.", "ERROR (2)", MB_OK | MB_TOPMOST);
			return;
		}

		// Update record in tree
		m_TableMapTree.SetItemText(m_TableMapTree.GetSelectedItem(), new_region.name.GetString());
		m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
		m_TableMapTree.SelectItem(m_TableMapTree.GetSelectedItem());

		update_display();
		Invalidate(false);
		pDoc->SetModifiedFlag(true);

	}

	else if (type_text == "Fonts")
	{
		// Get selected font record
		int font_group = GetType(sel_text);
		if (font_group<0 || font_group>k_max_number_of_font_groups_in_tablemap - 1)
			return;

		CString hexmash = sel_text.Mid(sel_text.Find("[") + 1, sel_text.Find("]") - sel_text.Find("[") - 1);

		TMapCI t_iter = p_tablemap->t$(font_group)->find(hexmash);

		if (t_iter == p_tablemap->t$(font_group)->end())
		{
			MessageBox("Error editing record - not found.", "ERROR", MB_OK | MB_TOPMOST);
			return;
		}

		// Prep dialog
		STablemapFont edit_font;
		edit_font.ch = t_iter->second.ch;
		edit_font.x_count = t_iter->second.x_count;
		for (int i = 0; i < edit_font.x_count; i++)
			edit_font.x[i] = t_iter->second.x[i];
		edit_font.hexmash = t_iter->second.hexmash;

		CArray <STablemapFont, STablemapFont> edit_font_array;
		edit_font_array.Add(edit_font);

		CDlgEditFont dlg_editfont;
		dlg_editfont.titletext = "Edit font character";
		dlg_editfont.character = t_iter->second.ch;
		dlg_editfont.group = font_group;
		dlg_editfont.delete_sort_enabled = false;
		dlg_editfont.new_t$_recs[font_group] = &edit_font_array;


		// Show dialog
		if (dlg_editfont.DoModal() != IDOK)
			return;

		// Delete original record in internal structure
		if (!p_tablemap->t$_erase(font_group, hexmash))
		{
			MessageBox("Error applying changes.", "ERROR (1)", MB_OK | MB_TOPMOST);
			return;
		}

		// Add new record to internal structure
		edit_font.ch = dlg_editfont.character.GetAt(0);

		if (!p_tablemap->t$_insert(font_group, edit_font))
		{
			MessageBox("Error applying changes.", "ERROR (2)", MB_OK | MB_TOPMOST);
			return;
		}

		// Update record in tree
		CString text;
		text.Format("%c (%d) [%s]", edit_font.ch, font_group, edit_font.hexmash);
		m_TableMapTree.SetItemText(m_TableMapTree.GetSelectedItem(), text.GetString());
		m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
		m_TableMapTree.SelectItem(m_TableMapTree.GetSelectedItem());

		update_display();
		Invalidate(false);
		pDoc->SetModifiedFlag(true);
	}

	else if (type_text == "Hash Points" ||
		type_node == NULL && sel_text == "Hash Points")
	{
		// If parent==NULL, then the root of hash points is selected and we should bring up the graphical hash point editor
		if (type_node == NULL)
		{
			// Prep dialog
			CDlgEditGrHashPoints dlggrhashpoints;
			for (int i = 0; i <= 3; i++)
			{
				for (PMapCI p_iter = p_tablemap->p$(i)->begin(); p_iter != p_tablemap->p$(i)->end(); p_iter++)
				{
					STablemapHashPoint hash_point;
					hash_point.x = p_iter->second.x;
					hash_point.y = p_iter->second.y;
					dlggrhashpoints.working_hash_points[i].Add(hash_point);
				}
			}

			// Show dialog
			if (dlggrhashpoints.DoModal() != IDOK)
				return;

			// Clear all existing hash points
			for (int i = 0; i <= 3; i++)
				p_tablemap->p$_clear(i);

			// Clear Hash Points branch of tree
			HTREEITEM hHashPointNode = GetTypeNode("Hash Points");
			HTREEITEM hChild = m_TableMapTree.GetChildItem(hHashPointNode);

			while (hChild != NULL)
			{
				m_TableMapTree.DeleteItem(hChild);
				hChild = m_TableMapTree.GetChildItem(hHashPointNode);
			}

			// Load new hash points from dialog into internal structure and tree
			for (int i = 0; i <= 3; i++)
			{
				for (int j = 0; j < dlggrhashpoints.working_hash_points[i].GetSize(); j++)
				{
					// Internal structure
					STablemapHashPoint hash_point;
					hash_point.x = dlggrhashpoints.working_hash_points[i].GetAt(j).x;
					hash_point.y = dlggrhashpoints.working_hash_points[i].GetAt(j).y;
					p_tablemap->p$_insert(i, hash_point);

					// Tree
					CString text;
					text.Format("%d (%d, %d)", i, hash_point.x, hash_point.y);
					m_TableMapTree.InsertItem(text, m_TableMapTree.GetSelectedItem());
				}
			}

			m_TableMapTree.SortChildren(m_TableMapTree.GetSelectedItem());

			update_display();
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}

		// else bring up the individual hash point editor
		else
		{
			// Get selected hash point record
			int hashpoint_type = strtoul(sel_text.Mid(0, 1).GetString(), NULL, 10);
			int x = strtoul(sel_text.Mid(3, sel_text.Find(",") - 3).GetString(), NULL, 10);
			int y = strtoul(sel_text.Mid(sel_text.Find(",") + 2, sel_text.Find(")") - sel_text.Find(",") + 2).GetString(), NULL, 10);

			PMapCI sel_hash_point = p_tablemap->p$(hashpoint_type)->find(((x & 0xffff) << 16) | (y & 0xffff));

			if (sel_hash_point == p_tablemap->p$(hashpoint_type)->end())
				return;

			// Prep dialog
			CDlgEditHashPoint dlghashpoint;
			dlghashpoint.titletext = "Edit Hash Point record";
			CString text;
			text.Format("Type %d", hashpoint_type);
			dlghashpoint.type = text;
			dlghashpoint.x = sel_hash_point->second.x;
			dlghashpoint.y = sel_hash_point->second.y;


			// Show dialog
			if (dlghashpoint.DoModal() != IDOK)
				return;

			// Delete original record in internal structure
			if (!p_tablemap->p$_erase(hashpoint_type, ((sel_hash_point->second.x & 0xffff) << 16) | (sel_hash_point->second.y & 0xffff)))
			{
				MessageBox("Error applying changes.", "ERROR (1)", MB_OK | MB_TOPMOST);
				return;
			}

			// Add new record to internal structure
			int new_hashpoint_type = atoi(dlghashpoint.type.Mid(5, 1).GetString());
			STablemapHashPoint hash_point;
			hash_point.x = dlghashpoint.x;
			hash_point.y = dlghashpoint.y;

			if (!p_tablemap->p$_insert(new_hashpoint_type, hash_point))
			{
				MessageBox("Error applying changes.", "ERROR (2)", MB_OK | MB_TOPMOST);
				return;
			}

			// Edit record in tree
			text.Format("%d (%d, %d)", new_hashpoint_type, hash_point.x, hash_point.y);
			m_TableMapTree.SetItemText(m_TableMapTree.GetSelectedItem(), text.GetString());
			m_TableMapTree.SortChildren(type_node ? type_node : m_TableMapTree.GetSelectedItem());
			m_TableMapTree.SelectItem(m_TableMapTree.GetSelectedItem());

			update_display();
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}
	}

	else if (type_text == "Hashes")
	{
		// Not valid - should delete and add a new one using "Create Hash" button
	}

	else if (type_text == "Images")
	{
		// Not valid - should delete and add a new one using "Create Image" button
	}

	else if (type_text == "Templates")
	{
		// Not valid - should delete and add a new one using "Create Template" button
	}
}

void CDlgTableMap::OnBnClickedCreateImage()
{
	CDlgEdit			edit;
	STablemapImage		new_image;
	int					x, y, width, height, pix_cnt;
	BYTE				alpha, red, green, blue;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	HTREEITEM			new_hti;
	CString				text, node_text, sel_region_name;
	HTREEITEM			image_node, region_node, child_node;
	RMapCI				sel_region = p_tablemap->r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region or template
	sel_region = p_tablemap->r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	// Case of create card template instead of create image
	if (type_text == "Templates") {
		// Make sure that we are attached to a window before we do this
		if (!pDoc->attached_hwnd)
		{
			MessageBox("To create a template, you must first capture\na window, using the 'Green Circle' toolbar button.",
				"No window captured", MB_OK);
			return;
		}

		// Get bitmap size
		width = pDoc->attached_rect.right - pDoc->attached_rect.left;
		height = pDoc->attached_rect.bottom - pDoc->attached_rect.top;

		// Populate new image record			
		//sel_template->second.name = edit.m_result;
		if (sel_template->second.right < sel_template->second.left)
		{
			MessageBox("Template has negative width!", "ERROR", 0);
			return;
		}
		else
		{
			sel_template->second.width = sel_template->second.right - sel_template->second.left + 1;
		}
		if (sel_template->second.bottom < sel_template->second.top)
		{
			MessageBox("Template has negative height!", "ERROR", 0);
			return;
		}
		else
		{
			sel_template->second.height = sel_template->second.bottom - sel_template->second.top + 1;
		}

		// Allocate space for "RGBAImage"
		text = sel_template->second.name + ".ppm";
		sel_template->second.image = new RGBAImage(sel_template->second.width, sel_template->second.height, text.GetString());

		// Populate pixel elements of struct
		pix_cnt = 0;
		for (y = (int)sel_template->second.top; y <= (int)sel_template->second.bottom; y++)
		{
			for (x = (int)sel_template->second.left; x <= (int)sel_template->second.right; x++)
			{
				alpha = pDoc->attached_pBits[y * width * 4 + x * 4 + 3];
				red = pDoc->attached_pBits[y * width * 4 + x * 4 + 2];
				green = pDoc->attached_pBits[y * width * 4 + x * 4 + 1];
				blue = pDoc->attached_pBits[y * width * 4 + x * 4 + 0];

				// image record is stored internally in ABGR format
				sel_template->second.pixel[pix_cnt] = (alpha << 24) + (blue << 16) + (green << 8) + red;
				sel_template->second.image->Set(red, green, blue, alpha, pix_cnt);

				pix_cnt++;
			}
		}

		sel_template->second.created = true;
		MessageBox("Card template created", "Info");

		//Invalidate(false);
		//pDoc->DoFileSave();
		//update_r$_display(true);
		//Invalidate(false);
		//theApp.m_pMainWnd->Invalidate(false);
		OnRegionChange();
		pDoc->SetModifiedFlag(true);
		return;
	}

	// Make sure that we are attached to a window before we do this
	if (!pDoc->attached_hwnd)
	{
		MessageBox("To create an image, you must first capture\na window, using the 'Green Circle' toolbar button.",
			"No window captured", MB_OK);
		return;
	}

	edit.m_titletext = "Name of new image";
	edit.m_result = "";
	if (edit.DoModal() == IDOK)
	{
		// Get bitmap size
		width = pDoc->attached_rect.right - pDoc->attached_rect.left;
		height = pDoc->attached_rect.bottom - pDoc->attached_rect.top;

		// Populate new image record			
		new_image.name = edit.m_result;
		if (sel_region->second.right < sel_region->second.left)
		{
			MessageBox("Image has negative width!", "ERROR", 0);
			return;
		}
		else
		{
			new_image.width = sel_region->second.right - sel_region->second.left + 1;
		}
		if (sel_region->second.bottom < sel_region->second.top)
		{
			MessageBox("Image has negative height!", "ERROR", 0);
			return;
		}
		else
		{
			new_image.height = sel_region->second.bottom - sel_region->second.top + 1;
		}

		// Allocate space for "RGBAImage"
		text = new_image.name + ".ppm";
		new_image.image = new RGBAImage(new_image.width, new_image.height, text.GetString());

		// Populate pixel elements of struct
		pix_cnt = 0;
		for (y = (int)sel_region->second.top; y <= (int)sel_region->second.bottom; y++)
		{
			for (x = (int)sel_region->second.left; x <= (int)sel_region->second.right; x++)
			{
				alpha = pDoc->attached_pBits[y * width * 4 + x * 4 + 3];
				red = pDoc->attached_pBits[y * width * 4 + x * 4 + 2];
				green = pDoc->attached_pBits[y * width * 4 + x * 4 + 1];
				blue = pDoc->attached_pBits[y * width * 4 + x * 4 + 0];

				// image record is stored internally in ABGR format
				new_image.pixel[pix_cnt] = (alpha << 24) + (blue << 16) + (green << 8) + red;
				new_image.image->Set(red, green, blue, alpha, pix_cnt);

				pix_cnt++;
			}
		}

		// Insert the new record in the existing array of i$ records, exit on failure
		if (!p_tablemap->i$_insert(new_image))
		{
			MessageBox("Failed to create image record, possible duplicate?", "Image creation error", MB_OK);
			return;
		}

		// Find root of the Images node
		image_node = m_TableMapTree.GetChildItem(NULL);
		node_text = m_TableMapTree.GetItemText(image_node);

		while (node_text != "Images" && image_node != NULL)
		{
			image_node = m_TableMapTree.GetNextItem(image_node, TVGN_NEXT);
			node_text = m_TableMapTree.GetItemText(image_node);
		}

		// Insert the new record into the tree
		if (image_node != NULL)
		{
			text.Format("%s (%dx%d)", new_image.name, new_image.width, new_image.height);
			new_hti = m_TableMapTree.InsertItem(text.GetString(), image_node);
			m_TableMapTree.SetItemData(new_hti,
				(DWORD_PTR)p_tablemap->CreateI$Index(new_image.name, new_image.width, new_image.height, new_image.pixel));
			m_TableMapTree.SortChildren(image_node);
		}

		region_node = update_tree("Regions");

		// Re-select previously selected region
		child_node = m_TableMapTree.GetChildItem(region_node);
		node_text = m_TableMapTree.GetItemText(child_node);
		while (node_text != sel_text && child_node != NULL)
		{
			child_node = m_TableMapTree.GetNextItem(child_node, TVGN_NEXT);
			node_text = m_TableMapTree.GetItemText(child_node);
		}

		if (child_node)
			m_TableMapTree.SelectItem(child_node);

		//Invalidate(false);
		pDoc->SetModifiedFlag(true);
	}
}

void CDlgTableMap::OnBnClickedCreateFont()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CDlgEditFont		dlg_editfont;
	STablemapFont		new_font;
	CString				text = "", separation = "", num = "";
	int					width = 0, height = 0, pos = 0, x_cnt = 0, scan_pos = 0;
	HDC					hdcScreen = NULL, hdc = NULL, hdc_region = NULL;
	HBITMAP				old_bitmap = NULL, old_bitmap2 = NULL, bitmap_region = NULL;
	bool				character[MAX_CHAR_WIDTH][MAX_CHAR_HEIGHT] = { false };
	bool				background[MAX_CHAR_WIDTH] = { false };
	CString				hexmash = "";
	int					char_field_x_begin = 0, char_field_x_end = 0, char_field_y_begin = 0, char_field_y_end = 0;
	int					i = 0, j = 0, insert_point = 0, new_index = 0;
	HTREEITEM			new_hti = NULL, font_node = NULL, region_node = NULL, child_node = NULL;
	CString				node_text = "";
	HTREEITEM			parent = GetRecordTypeNode(m_TableMapTree.GetSelectedItem());
	CArray <STablemapFont, STablemapFont>		new_t$_recs[k_max_number_of_font_groups_in_tablemap];
	CTransform			trans;
	RMapCI				sel_region = p_tablemap->r$()->end();
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();
	int					font_group;

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region
	sel_region = p_tablemap->r$()->find(sel_text.GetString());
	sel_template = p_tablemap->tpl$()->find(sel_text.GetString());

	// Exit if we can't find the region or template record
	if (sel_region == p_tablemap->r$()->end() && sel_template == p_tablemap->tpl$()->end())
		return;

	// Case of detect card template instead of create font
	if (type_text == "Templates") {
		DetectAndShowTemplate(sel_text.GetString());
		return;
	}

	// Get the text group we are dealing with
	font_group = atoi(sel_region->second.transform.Right(1));

	// Initialize arrays
	for (int i = 0; i < MAX_CHAR_WIDTH; i++)
		background[i] = true;

	// Get bitmap size
	width = sel_region->second.right - sel_region->second.left + 1;
	height = sel_region->second.bottom - sel_region->second.top + 1;

	// Select saved bitmap into memory DC
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	hdc = CreateCompatibleDC(hdcScreen);
	old_bitmap = (HBITMAP)SelectObject(hdc, pDoc->attached_bitmap);

	// Create new memory DC, new bitmap, and copy our region into the new dc/bitmap
	hdc_region = CreateCompatibleDC(hdcScreen);
	bitmap_region = CreateCompatibleBitmap(hdcScreen, width, height);
	old_bitmap2 = (HBITMAP)SelectObject(hdc_region, bitmap_region);
	BitBlt(hdc_region, 0, 0, width, height, hdc, sel_region->second.left, sel_region->second.top, SRCCOPY);

	// Get the pixels
	trans.TTypeTransform(sel_region, hdc_region, &text, &separation, background, character);

	// Clean up
	SelectObject(hdc_region, old_bitmap2);
	DeleteObject(bitmap_region);
	DeleteDC(hdc_region);

	SelectObject(hdc, old_bitmap);
	DeleteDC(hdc);
	DeleteDC(hdcScreen);

	// Scan through background, separate characters by looking for background bands
	for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; i++)
		new_t$_recs[i].RemoveAll();

	int start = 0;

	// skip initial background bands
	scan_pos = 0;
	while (background[scan_pos] && scan_pos < width)
		scan_pos++;

	while (scan_pos < width)
	{
		start = scan_pos;

		// scan for next background band
		while (!background[scan_pos] && scan_pos < width)
			scan_pos++;

		// We got a background bar, add element to array
		trans.GetShiftLeftDownIndexes(start, scan_pos - start, height, background, character,
			&char_field_x_begin, &char_field_x_end, &char_field_y_begin, &char_field_y_end);

		// Get individual hex values
		trans.CalcHexmash(char_field_x_begin, char_field_x_end, char_field_y_begin, char_field_y_end,
			character, &hexmash, true);

		pos = x_cnt = 0;
		num = hexmash.Tokenize(" ", pos);
		while (pos != -1 && x_cnt < MAX_SINGLE_CHAR_WIDTH)
		{
			new_font.x[x_cnt++] = strtoul(num.GetString(), NULL, 16);
			num = hexmash.Tokenize(" ", pos);
		}
		new_font.x_count = x_cnt;

		// Get whole hexmash
		trans.CalcHexmash(char_field_x_begin, char_field_x_end, char_field_y_begin, char_field_y_end,
			character, &new_font.hexmash, false);

		// Populate new font record array, if this character does not already exist in this font group
		if (p_tablemap->t$(font_group)->find(new_font.hexmash) == p_tablemap->t$(font_group)->end())
		{
			CString text;
			m_Transform.GetLBText(m_Transform.GetCurSel(), text);
			new_font.ch = '?';

			// Insert the new record in the existing array of i$ records
			new_t$_recs[font_group].Add(new_font);
		}

		// skip background bands
		while (background[scan_pos] && scan_pos < width)
			scan_pos++;

	}

	if (new_t$_recs[font_group].GetCount() == 0)
	{
		MessageBox("No foreground pixels, or no unknown characters found.", "Font creation error");
	}
	else
	{
		// Prepare dialog box
		dlg_editfont.titletext = "Add font characters";
		dlg_editfont.character = "";
		dlg_editfont.delete_sort_enabled = true;
		dlg_editfont.group = font_group;

		for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; i++)
			dlg_editfont.new_t$_recs[i] = &new_t$_recs[i];

		if (dlg_editfont.DoModal() == IDOK)
		{
			// Find root of the Fonts node
			font_node = m_TableMapTree.GetChildItem(NULL);
			node_text = m_TableMapTree.GetItemText(font_node);
			while (node_text != "Fonts" && font_node != NULL)
			{
				font_node = m_TableMapTree.GetNextItem(font_node, TVGN_NEXT);
				node_text = m_TableMapTree.GetItemText(font_node);
			}

			for (int i = 0; i < new_t$_recs[font_group].GetCount(); i++)
			{
				// Populate temp structure
				new_font.x_count = new_t$_recs[font_group].GetAt(i).x_count;
				for (int j = 0; j < new_font.x_count; j++)
					new_font.x[j] = new_t$_recs[font_group].GetAt(i).x[j];
				new_font.hexmash = new_t$_recs[font_group].GetAt(i).hexmash;
				new_font.ch = new_t$_recs[font_group].GetAt(i).ch;

				// Insert into internal structure
				p_tablemap->t$_insert(font_group, new_font);

				// Insert the new record into the tree
				if (font_node != NULL)
				{
					// Insert the new records into the tree
					text.Format("%c (%d) [%s]", new_font.ch, font_group, new_font.hexmash);
					new_hti = m_TableMapTree.InsertItem(text.GetString(), font_node);
					m_TableMapTree.SetItemData(new_hti, (DWORD_PTR)new_index);
				}
			}

			// Update tree
			region_node = update_tree("Regions");

			// Re-select previously selected region
			child_node = m_TableMapTree.GetChildItem(region_node);
			node_text = m_TableMapTree.GetItemText(child_node);
			while (node_text != sel_text && child_node != NULL)
			{
				child_node = m_TableMapTree.GetNextItem(child_node, TVGN_NEXT);
				node_text = m_TableMapTree.GetItemText(child_node);
			}

			if (child_node)
				m_TableMapTree.SelectItem(child_node);

			// Update display and set dirty bit 
			update_display();
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}
	}
}


void CDlgTableMap::OnBnClickedFontplus()
{
	separation_font_size++;
	lf_fixed.lfHeight = separation_font_size;
	separation_font.DeleteObject();
	separation_font.CreateFontIndirect(&lf_fixed);
	m_PixelSeparation.SetFont(&separation_font);
	Invalidate(false);
}

void CDlgTableMap::OnBnClickedFontminus()
{
	separation_font_size--;
	lf_fixed.lfHeight = separation_font_size;
	separation_font.DeleteObject();
	separation_font.CreateFontIndirect(&lf_fixed);
	m_PixelSeparation.SetFont(&separation_font);
	Invalidate(false);
}

BOOL CDlgTableMap::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	RECT			bmp_rect;
	POINT			point;

	m_BitmapFrame.GetClientRect(&bmp_rect);
	GetCursorPos(&point);
	m_BitmapFrame.ScreenToClient(&point);

	if (picker_cursor &&
		point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
		point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
	{
		SetCursor(hCurPicker);
		return TRUE;
	}

	return CDialog::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CDlgTableMap::OnStickyButtonDown(WPARAM wp, LPARAM lp)
{
	COpenScrapeView* pView = COpenScrapeView::GetView();

	if ((HWND)wp == m_DrawRect.GetSafeHwnd())
	{
		pView->drawing_rect = true;
	}

	else if ((HWND)wp == m_Picker.GetSafeHwnd())
	{
		picker_cursor = true;
		SetCursor(hCurPicker);
	}

	return false;
}

LRESULT CDlgTableMap::OnStickyButtonUp(WPARAM wp, LPARAM lp)
{
	COpenScrapeView* pView = COpenScrapeView::GetView();

	if ((HWND)wp == m_DrawRect.GetSafeHwnd())
	{
		pView->drawing_rect = false;
	}

	else if ((HWND)wp == m_Picker.GetSafeHwnd())
	{
		picker_cursor = false;
		SetCursor(hCurStandard);
		update_display();
		Invalidate(false);
	}

	return false;
}

void CDlgTableMap::OnLButtonDown(UINT nFlags, CPoint point)
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RECT				bmp_rect;
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		m_BitmapFrame.GetWindowRect(&bmp_rect);
		ScreenToClient(&bmp_rect);

		if (picker_cursor &&
			point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
			point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
		{
			sel_region->second.color = get_color_under_mouse(&nFlags, &point);

			update_display();
			Invalidate(false);
			pDoc->SetModifiedFlag(true);

			m_Picker.OnBnClicked();
		}
	}

	CDialog::OnLButtonDown(nFlags, point);
}

void CDlgTableMap::OnMouseMove(UINT nFlags, CPoint point)
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	RECT				bmp_rect;
	RMapI				sel_region = p_tablemap->set_r$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		m_BitmapFrame.GetWindowRect(&bmp_rect);
		ScreenToClient(&bmp_rect);

		if (picker_cursor &&
			point.x >= bmp_rect.left && point.x <= bmp_rect.right &&
			point.y >= bmp_rect.top && point.y <= bmp_rect.bottom)
		{
			sel_region->second.color = get_color_under_mouse(&nFlags, &point);

			update_display();
			Invalidate(false);
			pDoc->SetModifiedFlag(true);
		}
	}

	CDialog::OnMouseMove(nFlags, point);
}

COLORREF CDlgTableMap::get_color_under_mouse(UINT* nFlags, CPoint* point)
{
	CDC* pDC = GetDC();
	HDC				hdc = *pDC;
	HDC				hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC				hdcCompatible = CreateCompatibleDC(hdcScreen);
	COLORREF		cr;
	int				width, height;
	CString			sel = m_TableMapTree.GetItemText(m_TableMapTree.GetSelectedItem());
	RECT			crect;
	HBITMAP			hbm, old_bitmap;
	BYTE* pBits, alpha, red, green, blue;

	// Load TableMap dialog into memory DC
	GetClientRect(&crect);
	width = crect.right - crect.left + 1;
	height = crect.bottom - crect.top + 1;
	hbm = CreateCompatibleBitmap(hdcScreen, width, height);
	old_bitmap = (HBITMAP)SelectObject(hdcCompatible, hbm);
	BitBlt(hdcCompatible, 0, 0, width, height, hdc, 0, 0, SRCCOPY);
	SelectObject(hdcCompatible, old_bitmap);

	// Allocate heap space for BITMAPINFO
	BITMAPINFO* bmi;
	int			info_len = sizeof(BITMAPINFOHEADER) + 1024;
	bmi = (BITMAPINFO*) ::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, info_len);

	// Populate BITMAPINFOHEADER
	bmi->bmiHeader.biSize = sizeof(bmi->bmiHeader);
	bmi->bmiHeader.biBitCount = 0;
	GetDIBits(hdcCompatible, hbm, 0, 0, NULL, bmi, DIB_RGB_COLORS);

	// Get the actual BGRA bit information
	bmi->bmiHeader.biHeight = -bmi->bmiHeader.biHeight;
	pBits = new BYTE[bmi->bmiHeader.biSizeImage];
	GetDIBits(hdc, hbm, 0, height, pBits, bmi, DIB_RGB_COLORS);

	// Get pixel color under mouse click spot - GetDIBits format is BGRA, COLORREF format is ABGR
	alpha = pBits[point->y * width * 4 + point->x * 4 + 3];
	red = pBits[point->y * width * 4 + point->x * 4 + 2];
	green = pBits[point->y * width * 4 + point->x * 4 + 1];
	blue = pBits[point->y * width * 4 + point->x * 4 + 0];

	cr = (alpha << 24) + (blue << 16) + (green << 8) + (red);

	// Clean up
	delete[]pBits;
	HeapFree(GetProcessHeap(), NULL, bmi);

	// Clean up
	DeleteObject(hbm);
	DeleteDC(hdcCompatible);
	DeleteDC(hdcScreen);
	ReleaseDC(pDC);

	return cr;
}

void CDlgTableMap::create_tree(void)
{
	HTREEITEM					parent, newitem;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	CArray<CString, CString>	strings;
	CString						text;

	m_TableMapTree.DeleteAllItems();

	// z$ records
	parent = m_TableMapTree.InsertItem("Sizes");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (ZMapCI z_iter = p_tablemap->z$()->begin(); z_iter != p_tablemap->z$()->end(); z_iter++)
		m_TableMapTree.InsertItem(z_iter->second.name, parent);

	m_TableMapTree.SortChildren(parent);

	// s$ records
	parent = m_TableMapTree.InsertItem("Symbols");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (SMapCI s_iter = p_tablemap->s$()->begin(); s_iter != p_tablemap->s$()->end(); s_iter++)
		m_TableMapTree.InsertItem(s_iter->second.name, parent);

	m_TableMapTree.SortChildren(parent);

	// r$ records
	parent = m_TableMapTree.InsertItem("Regions");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (RMapCI r_iter = p_tablemap->r$()->begin(); r_iter != p_tablemap->r$()->end(); r_iter++) {
		m_TableMapTree.InsertItem(r_iter->second.name, parent);
	}
	GroupRegions();

	m_TableMapTree.SortChildren(parent);

	// t$ records
	parent = m_TableMapTree.InsertItem("Fonts");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (int i = 0; i < k_max_number_of_font_groups_in_tablemap; i++)
	{
		for (TMapCI t_iter = p_tablemap->t$(i)->begin(); t_iter != p_tablemap->t$(i)->end(); t_iter++)
		{
			text.Format("%c (%d) [%s]", t_iter->second.ch, i, t_iter->second.hexmash);
			newitem = m_TableMapTree.InsertItem(text, parent);
		}
	}
	m_TableMapTree.SortChildren(parent);

	// p$ records
	parent = m_TableMapTree.InsertItem("Hash Points");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (int i = 0; i < 3; i++)
	{
		for (PMapCI p_iter = p_tablemap->p$(i)->begin(); p_iter != p_tablemap->p$(i)->end(); p_iter++)
		{
			text.Format("%d (%d, %d)", i, p_iter->second.x, p_iter->second.y);
			newitem = m_TableMapTree.InsertItem(text, parent);
		}
	}

	m_TableMapTree.SortChildren(parent);

	// h$ records
	parent = m_TableMapTree.InsertItem("Hashes");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (int i = 0; i <= 3; i++)
	{
		for (HMapCI h_iter = p_tablemap->h$(i)->begin(); h_iter != p_tablemap->h$(i)->end(); h_iter++)
		{
			text.Format("%s (%d)", h_iter->second.name, i);
			newitem = m_TableMapTree.InsertItem(text, parent);
		}
	}

	m_TableMapTree.SortChildren(parent);

	// i$ records
	parent = m_TableMapTree.InsertItem("Images");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (IMapCI i_iter = p_tablemap->i$()->begin(); i_iter != p_tablemap->i$()->end(); i_iter++)
	{
		text.Format("%s (%dx%d)", i_iter->second.name, i_iter->second.width, i_iter->second.height);
		newitem = m_TableMapTree.InsertItem(text, parent);
		m_TableMapTree.SetItemData(newitem,
			(DWORD_PTR)p_tablemap->CreateI$Index(i_iter->second.name, i_iter->second.width, i_iter->second.height, i_iter->second.pixel));
	}

	m_TableMapTree.SortChildren(parent);

	// tpl$ records
	parent = m_TableMapTree.InsertItem("Templates");
	m_TableMapTree.SetItemState(parent, TVIS_BOLD, TVIS_BOLD);

	for (TPLMapCI tpl_iter = p_tablemap->tpl$()->begin(); tpl_iter != p_tablemap->tpl$()->end(); tpl_iter++)
		m_TableMapTree.InsertItem(tpl_iter->second.name, parent);

	m_TableMapTree.SortChildren(parent);

	UpdateStatus();
}

void CDlgTableMap::OnBnClickedNudgeTaller()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.top = sel_region->second.top - 1;
		sel_region->second.bottom = sel_region->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.top = sel_template->second.top - 1;
		sel_template->second.bottom = sel_template->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeShorter()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.top = sel_region->second.top + 1;
		sel_region->second.bottom = sel_region->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.top = sel_template->second.top + 1;
		sel_template->second.bottom = sel_template->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeWider()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left - 1;
		sel_region->second.right = sel_region->second.right + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left - 1;
		sel_template->second.right = sel_template->second.right + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeNarrower()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left + 1;
		sel_region->second.right = sel_region->second.right - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left + 1;
		sel_template->second.right = sel_template->second.right - 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeBigger()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left - 1;
		sel_region->second.top = sel_region->second.top - 1;
		sel_region->second.right = sel_region->second.right + 1;
		sel_region->second.bottom = sel_region->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left - 1;
		sel_template->second.top = sel_template->second.top - 1;
		sel_template->second.right = sel_template->second.right + 1;
		sel_template->second.bottom = sel_template->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeSmaller()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		if (sel_region->second.left + 1 <= sel_region->second.right)
			sel_region->second.left = sel_region->second.left + 1;

		if (sel_region->second.top + 1 <= sel_region->second.bottom)
			sel_region->second.top = sel_region->second.top + 1;

		if (sel_region->second.right - 1 >= sel_region->second.left)
			sel_region->second.right = sel_region->second.right - 1;

		if (sel_region->second.bottom - 1 >= sel_region->second.top)
			sel_region->second.bottom = sel_region->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		if (sel_template->second.left + 1 <= sel_template->second.right)
			sel_template->second.left = sel_template->second.left + 1;

		if (sel_template->second.top + 1 <= sel_template->second.bottom)
			sel_template->second.top = sel_template->second.top + 1;

		if (sel_template->second.right - 1 >= sel_template->second.left)
			sel_template->second.right = sel_template->second.right - 1;

		if (sel_template->second.bottom - 1 >= sel_template->second.top)
			sel_template->second.bottom = sel_template->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeUpleft()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left - 1;
		sel_region->second.top = sel_region->second.top - 1;
		sel_region->second.right = sel_region->second.right - 1;
		sel_region->second.bottom = sel_region->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left - 1;
		sel_template->second.top = sel_template->second.top - 1;
		sel_template->second.right = sel_template->second.right - 1;
		sel_template->second.bottom = sel_template->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}
}



void CDlgTableMap::OnBnClickedNudgeUp()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.top = sel_region->second.top - 1;
		sel_region->second.bottom = sel_region->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.top = sel_template->second.top - 1;
		sel_template->second.bottom = sel_template->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeUpright()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left + 1;
		sel_region->second.top = sel_region->second.top - 1;
		sel_region->second.right = sel_region->second.right + 1;
		sel_region->second.bottom = sel_region->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left + 1;
		sel_template->second.top = sel_template->second.top - 1;
		sel_template->second.right = sel_template->second.right + 1;
		sel_template->second.bottom = sel_template->second.bottom - 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeRight()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left + 1;
		sel_region->second.right = sel_region->second.right + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left + 1;
		sel_template->second.right = sel_template->second.right + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeDownright()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left + 1;
		sel_region->second.top = sel_region->second.top + 1;
		sel_region->second.right = sel_region->second.right + 1;
		sel_region->second.bottom = sel_region->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left + 1;
		sel_template->second.top = sel_template->second.top + 1;
		sel_template->second.right = sel_template->second.right + 1;
		sel_template->second.bottom = sel_template->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeDown()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.top = sel_region->second.top + 1;
		sel_region->second.bottom = sel_region->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.top = sel_template->second.top + 1;
		sel_template->second.bottom = sel_template->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeDownleft()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left - 1;
		sel_region->second.top = sel_region->second.top + 1;
		sel_region->second.right = sel_region->second.right - 1;
		sel_region->second.bottom = sel_region->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left - 1;
		sel_template->second.top = sel_template->second.top + 1;
		sel_template->second.right = sel_template->second.right - 1;
		sel_template->second.bottom = sel_template->second.bottom + 1;

		UpdateDisplayOfAllRegions();
	}
}

void CDlgTableMap::OnBnClickedNudgeLeft()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	RMapI				sel_region = p_tablemap->set_r$()->end();
	TPLMapI				sel_template = p_tablemap->set_tpl$()->end();

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get iterator for selected region and template
	sel_region = p_tablemap->set_r$()->find(sel_text.GetString());
	sel_template = p_tablemap->set_tpl$()->find(sel_text.GetString());

	if (sel_region != p_tablemap->r$()->end())
	{
		sel_region->second.left = sel_region->second.left - 1;
		sel_region->second.right = sel_region->second.right - 1;

		UpdateDisplayOfAllRegions();
	}

	if (sel_template != p_tablemap->tpl$()->end())
	{
		sel_template->second.left = sel_template->second.left - 1;
		sel_template->second.right = sel_template->second.right - 1;

		UpdateDisplayOfAllRegions();
	}
}

BOOL CDlgTableMap::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return false;

	TOOLTIPTEXT* pTTT = (TOOLTIPTEXT*)pNMHDR;
	UINT_PTR nID = pNMHDR->idFrom;
	if (pTTT->uFlags & TTF_IDISHWND)
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
		if (nID)
		{
			pTTT->lpszText = MAKEINTRESOURCE(nID);
			pTTT->hinst = AfxGetResourceHandle();
			return(TRUE);
		}
	}

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE |
		SWP_NOSIZE | SWP_NOMOVE | SWP_NOOWNERZORDER);

	return true;    // message was handled
}

void CDlgTableMap::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	if (proceed_scroll == true)
		m_scrollHelper->OnSize(nType, cx, cy);
}


void CDlgTableMap::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (pScrollBar == NULL)
		m_scrollHelper->OnVScroll(nSBCode,
			nPos, pScrollBar);
}

BOOL CDlgTableMap::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	BOOL wasScrolled;
	if (proceed_scroll == true)
		wasScrolled = m_scrollHelper->OnMouseWheel(nFlags,
			zDelta, pt);
	else
		wasScrolled = FALSE;
	return wasScrolled;
}

int CDlgTableMap::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

void CDlgTableMap::UpdateStatus(void)
{
	int k, fontNum, cardNum;
	CString		statusFonts, statusCards, node_text, fontSet;
	HTREEITEM	node;

	statusFonts = "";
	statusCards = "";

	fontSet.Format("%d", m_TrackerFontSet.GetCurSel());
	fontNum = m_TrackerFontNum.GetCurSel() + 1;
	cardNum = m_TrackerCardNum.GetCurSel() + 1;


	//fonts
	for (int i = 0; fontsList[i] != '\0'; i++)
	{
		k = fontNum;
		// Find root of the Fonts node
		node = m_TableMapTree.GetChildItem(NULL);
		node_text = m_TableMapTree.GetItemText(node);
		while (node_text != "Fonts" && node != NULL)
		{
			node = m_TableMapTree.GetNextItem(node, TVGN_NEXT);
			node_text = m_TableMapTree.GetItemText(node);
		}
		// If we have fonts created...
		if (m_TableMapTree.ItemHasChildren(node))
		{
			node = m_TableMapTree.GetChildItem(node);
			node_text = m_TableMapTree.GetItemText(node);
			// ...and the first node is a font we need in the proper set...
			if ((strncmp(node_text, &fontsList[i], 1) == 0) && (node_text[3] == fontSet))
			{
				// ...decrement the amount missing for this font
				k--;
			}
			// Parse the rest of the nodes looking for matches and decrementing
			while (node != NULL)
			{
				node = m_TableMapTree.GetNextSiblingItem(node);
				node_text = m_TableMapTree.GetItemText(node);
				if ((strncmp(node_text, &fontsList[i], 1) == 0) && node_text[3] == fontSet)
				{
					k--;
				}
			}
		}
		// ...add missing characters to a missing fonts list
		while (k > 0)
		{
			statusFonts = statusFonts + fontsList[i];
			k--;
		}
	}
	//  Display missing fonts 
	m_status_fonts.SetWindowTextA(_T(statusFonts));

	//card hashes
	for (int i = 0; i < 52; i++)
	{
		k = cardNum;
		// Find root of the Hashes node
		node = m_TableMapTree.GetChildItem(NULL);
		node_text = m_TableMapTree.GetItemText(node);
		while (node_text != "Hashes" && node != NULL)
		{
			node = m_TableMapTree.GetNextItem(node, TVGN_NEXT);
			node_text = m_TableMapTree.GetItemText(node);
		}
		// If we have hashes created...
		if (m_TableMapTree.ItemHasChildren(node))
		{
			node = m_TableMapTree.GetChildItem(node);
			node_text = m_TableMapTree.GetItemText(node);
			// ...and the first node is a card in the proper set...
			if (strncmp(node_text, cardsList[i], 2) == 0)
			{
				// ...decrement the amount missing for this card
				k--;
			}
			// Parse the rest of the nodes looking for matches and decrementing
			while (node != NULL)
			{
				node = m_TableMapTree.GetNextSiblingItem(node);
				node_text = m_TableMapTree.GetItemText(node);
				if (strncmp(node_text, cardsList[i], 2) == 0)
				{
					k--;
				}
			}
		}
		// ...add missing cards to a missing cards list
		while (k > 0)
		{
			statusCards = statusCards + cardsList[i];
			k--;
		}
	}
	//  Display missing cards
	m_status_cards.SetWindowTextA(_T(statusCards));
}

CString CDlgTableMap::GetGroupName(CString regionName)
{
	CString groupName = "";

	switch (region_grouping)
	{
	case BY_TYPE:

		if (regionName.Mid(0, 1) == "p" || regionName.Mid(0, 1) == "u" || regionName.Mid(0, 1) == "c")
			groupName = regionName.Mid(0, 2);

		else if (regionName.Mid(0, 1) == "i")
			groupName = regionName.Mid(0, 2);

		else
			groupName.Empty();

		break;

	case BY_NAME:
	default:
		if (regionName.Find("cardface") != -1 && regionName.Find("rank") != -1)
			groupName = "cardface-rank";

		else if (regionName.Find("cardface") != -1 && regionName.Find("suit") != -1)
			groupName = "cardface-suit";

		else if (regionName.Find("cardface") != -1)
			groupName = "cardface";

		else if (regionName.Find("handnumber") != -1)
			groupName = "handnumber";

		else if (regionName.Find("pot") != -1 && regionName.Find("chip") != -1)
			groupName = "pot-chip";

		else if (regionName.Find("pot") != -1)
			groupName = "pot";

		else if (regionName.Find("limits") != -1)
			groupName = "limits";

		else if (regionName.Find("button") != -1)
			groupName = "button";

		else if (regionName.Find("label") != -1)
			groupName = "label";

		else if (regionName.Find("state") != -1)
			groupName = "state";

		else if (regionName.Find("active") != -1)
			groupName = "active";

		else if (regionName.Find("balance") != -1)
			groupName = "balance";

		else if (regionName.Find("bet") != -1)
			groupName = "bet";

		else if (regionName.Find("cardback") != -1)
			groupName = "cardback";

		else if (regionName.Find("dealer") != -1)
			groupName = "dealer";

		else if (regionName.Find("name") != -1 && regionName.Find("tourn") == -1)
			groupName = "name";

		else if (regionName.Find("seated") != -1)
			groupName = "seated";

		else if (regionName.Find("chip") != -1)
			groupName = "chip";

		else if (regionName.Find("c0isfinaltable") != -1 || regionName.Find("c0smallblind") != -1 ||
			regionName.Find("c0bigblind") != -1 || regionName.Find("c0bigbet") != -1 ||
			regionName.Find("c0ante") != -1)
			groupName = "c0misc";

		else
			groupName.Empty();
		break;
	}

	return groupName;
}

HTREEITEM CDlgTableMap::FindRegionGroupItem(HTREEITEM hRegionNode, CString groupName)
{
	HTREEITEM searchItem = m_TableMapTree.GetChildItem(hRegionNode);

	while (searchItem)
	{
		if (m_TableMapTree.ItemHasChildren(searchItem))
		{
			CString tempString = m_TableMapTree.GetItemText(searchItem);
			if (!tempString.Compare(groupName))
				return searchItem;
		}
		searchItem = m_TableMapTree.GetNextSiblingItem(searchItem);
	}

	return NULL;
}

HTREEITEM CDlgTableMap::MoveTreeItem(HTREEITEM hItem, HTREEITEM hNewParent, CString name, bool bSelect)
{
	HTREEITEM hMovedItem = NULL;

	if (!name.IsEmpty())
	{
		hMovedItem = m_TableMapTree.InsertItem(name, hNewParent);
	}
	else
	{
		CString itemName;
		itemName = m_TableMapTree.GetItemText(hItem);
		hMovedItem = m_TableMapTree.InsertItem(itemName, hNewParent);
	}

	m_TableMapTree.SetItemData(hMovedItem, m_TableMapTree.GetItemData(hItem));
	m_TableMapTree.DeleteItem(hItem);

	if (bSelect)
		m_TableMapTree.SelectItem(hMovedItem);

	return hMovedItem;
}

void CDlgTableMap::RemoveSingleItemGroups()
{
	// Find region node
	HTREEITEM hRegionNode = GetTypeNode("Regions");

	if (hRegionNode == NULL)
		return;

	if (m_TableMapTree.GetItemText(hRegionNode) != "Regions")
		return;

	HTREEITEM hRegionChildItem = m_TableMapTree.GetChildItem(hRegionNode);
	HTREEITEM hNextLevelItem = NULL, hNextItem = NULL;

	while (hRegionChildItem != NULL)
	{
		// Skip, if this item is not a group
		if (!m_TableMapTree.ItemHasChildren(hRegionChildItem))
		{
			hRegionChildItem = m_TableMapTree.GetNextSiblingItem(hRegionChildItem);
		}

		// Move single item out of group
		else
		{

			hNextItem = m_TableMapTree.GetNextSiblingItem(hRegionChildItem);
			hNextLevelItem = m_TableMapTree.GetChildItem(hRegionChildItem);

			if (hNextLevelItem != NULL && m_TableMapTree.GetNextSiblingItem(hNextLevelItem) == NULL)
			{
				MoveTreeItem(hNextLevelItem, hRegionNode, NULL, false);
				m_TableMapTree.DeleteItem(hRegionChildItem);
			}

			hRegionChildItem = hNextItem;
		}
	}
}

HTREEITEM CDlgTableMap::GetTypeNode(CString type)
{
	HTREEITEM hNode = m_TableMapTree.GetNextItem(m_TableMapTree.GetRootItem(), TVGN_NEXT);

	while (hNode != NULL)
	{
		if (m_TableMapTree.GetItemText(hNode) == type)
			break;

		hNode = m_TableMapTree.GetNextItem(hNode, TVGN_NEXT);
	}

	return hNode;
}

void CDlgTableMap::GroupRegions()
{
	if (!m_TableMapTree.ItemHasChildren(m_TableMapTree.GetRootItem()))
		return;

	// Find region node
	HTREEITEM hRegionNode = GetTypeNode("Regions");

	if (hRegionNode == NULL)
		return;

	if (m_TableMapTree.GetItemText(hRegionNode) != "Regions")
		return;

	// Loop through each item in the region node and move into groups
	HTREEITEM hRegionChildItem = m_TableMapTree.GetChildItem(hRegionNode);
	HTREEITEM hNextItem = NULL;

	while (hRegionChildItem != NULL)
	{
		hNextItem = m_TableMapTree.GetNextSiblingItem(hRegionChildItem);

		// Skip, if this item is a group
		if (!m_TableMapTree.ItemHasChildren(hRegionChildItem))
		{
			CString itemText = m_TableMapTree.GetItemText(hRegionChildItem);
			CString groupName = GetGroupName(itemText);

			// Skip, if this is not a group-able region
			if (!groupName.IsEmpty())
			{

				// Find or create the appropriate group node
				HTREEITEM hGroup = FindRegionGroupItem(hRegionNode, groupName);

				if (!hGroup)
					hGroup = m_TableMapTree.InsertItem(groupName, hRegionNode);

				// Move the item into the new group
				MoveTreeItem(hRegionChildItem, hGroup, itemText, false);
			}
		}

		hRegionChildItem = hNextItem;
	}

	//if (region_grouping==BY_NAME)
	//	RemoveSingleItemGroups();
}

void CDlgTableMap::UngroupRegions()
{
	if (!m_TableMapTree.ItemHasChildren(m_TableMapTree.GetRootItem()))
		return;

	// Find region node
	HTREEITEM hRegionNode = GetTypeNode("Regions");

	if (hRegionNode == NULL)
		return;

	if (m_TableMapTree.GetItemText(hRegionNode) != "Regions")
		return;

	// Loop through each item in the region node and move into groups
	HTREEITEM hRegionChildItem = m_TableMapTree.GetChildItem(hRegionNode);
	HTREEITEM hGroupedRegionItem = NULL, hNextItem = NULL;

	while (hRegionChildItem != NULL)
	{
		// Skip if this item is not a group
		if (!m_TableMapTree.ItemHasChildren(hRegionChildItem))
		{
			hRegionChildItem = m_TableMapTree.GetNextSiblingItem(hRegionChildItem);
		}

		// Move out of the group
		else
		{
			hGroupedRegionItem = m_TableMapTree.GetChildItem(hRegionChildItem);
			while (hGroupedRegionItem)
			{
				hNextItem = m_TableMapTree.GetNextSiblingItem(hGroupedRegionItem);
				MoveTreeItem(hGroupedRegionItem, hRegionNode, NULL, false);
				hGroupedRegionItem = hNextItem;
			}

			hNextItem = m_TableMapTree.GetNextSiblingItem(hRegionChildItem);
			m_TableMapTree.DeleteItem(hRegionChildItem);
			hRegionChildItem = hNextItem;
		}
	}
}

HTREEITEM CDlgTableMap::InsertGroupedRegion(CString itemText)
{
	// Find region node
	HTREEITEM hRegionNode = GetTypeNode("Regions");

	if (hRegionNode == NULL)
		return NULL;

	if (m_TableMapTree.GetItemText(hRegionNode) != "Regions")
		return NULL;

	// Figure out the group name for this item
	CString groupName = GetGroupName(itemText);

	// Find or create the appropriate group node
	HTREEITEM hGroup = FindRegionGroupItem(hRegionNode, groupName);

	if (!hGroup)
		hGroup = m_TableMapTree.InsertItem(groupName, hRegionNode);

	// Insert it
	HTREEITEM hNewItem = m_TableMapTree.InsertItem(itemText, hGroup);
	m_TableMapTree.SortChildren(hGroup);

	// And clean up if needed
	//if (region_grouping==BY_NAME)
	//	RemoveSingleItemGroups();


	return hNewItem;
}

HTREEITEM CDlgTableMap::FindItem(CString s, HTREEITEM start)
{
	HTREEITEM item = m_TableMapTree.GetNextItem(start, TVGN_CHILD);

	while (item)
	{
		CString t = m_TableMapTree.GetItemText(item);
		if (m_TableMapTree.ItemHasChildren(item))
		{
			HTREEITEM next = FindItem(s, item);
			if (next)
				return next;
		}

		else if (s == m_TableMapTree.GetItemText(item))
			return item;

		item = m_TableMapTree.GetNextItem(item, TVGN_NEXT);
	}

	return NULL;
}

void CDlgTableMap::OnBnClickedUseDefault()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	update_ocr_display();

	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);

	pDoc->SetModifiedFlag(true);
}

void CDlgTableMap::OnBnClickedUseCrop()
{
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();

	update_ocr_display();

	theApp.m_pMainWnd->Invalidate(false);
	Invalidate(false);

	pDoc->SetModifiedFlag(true);
}

void CDlgTableMap::OnBnClickedCreateHash0()
{
	CreateHash(0);
}

void CDlgTableMap::OnBnClickedCreateHash1()
{
	CreateHash(1);
}

void CDlgTableMap::OnBnClickedCreateHash2()
{
	CreateHash(2);
}

void CDlgTableMap::OnBnClickedCreateHash3()
{
	CreateHash(3);
}

void CDlgTableMap::CreateHash(int hash_type)
{
	STablemapHashValue	new_hash;
	COpenScrapeDoc* pDoc = COpenScrapeDoc::GetDocument();
	HTREEITEM			new_hti, child_node;
	CString				text;

	CString				sel_text = "", type_text = "";
	HTREEITEM type_node = GetTextSelItemAndRecordType(&sel_text, &type_text);

	// Get image name
	int start = 0;
	sel_text = sel_text.Tokenize(" ", start);

	// Calculate hash for selected image
	int index = (int)m_TableMapTree.GetItemData(m_TableMapTree.GetSelectedItem());
	IMapCI i_iter = p_tablemap->i$()->find(index);
	new_hash.hash = p_tablemap->CalculateHashValue(i_iter, hash_type);

	// Add new record to internal structure
	new_hash.name = sel_text;
	if (!p_tablemap->h$_insert(hash_type, new_hash))
	{
		MessageBox("Error adding hash record: " + new_hash.name, "Hash record add error", MB_OK);
	}
	else
	{
		// Insert the new record into the tree
		HTREEITEM node = GetTypeNode("Hashes");
		if (node != NULL)
		{
			text.Format("%s (%d)", new_hash.name, hash_type);
			new_hti = m_TableMapTree.InsertItem(text.GetString(), node);
			m_TableMapTree.SortChildren(node);
		}

		node = update_tree("Images");

		// Re-select previously selected image
		child_node = m_TableMapTree.GetChildItem(node);
		CString node_text = m_TableMapTree.GetItemText(child_node);
		while (node_text != sel_text && child_node != NULL)
		{
			child_node = m_TableMapTree.GetNextItem(child_node, TVGN_NEXT);
			node_text = m_TableMapTree.GetItemText(child_node);
		}

		if (child_node)
			m_TableMapTree.SelectItem(child_node);

		//Invalidate(false);
		pDoc->SetModifiedFlag(true);

		text.Format("Added hash group %d record: %s", hash_type, new_hash.name);
		MessageBox(text, "Hash record add success", MB_OK);
	}

	update_display();
}

void CDlgTableMap::OnTvnKeydownTablemapTree(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMTVKEYDOWN pTVKeyDown = reinterpret_cast<LPNMTVKEYDOWN>(pNMHDR);
	switch (pTVKeyDown->wVKey)
	{
	case VK_DELETE:
		CDlgTableMap::OnBnClickedDelete();
		break;
	case VK_INSERT:
		CDlgTableMap::OnBnClickedNew();
		break;
	default:
		break;
	}
	*pResult = 0;
}
