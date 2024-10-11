#pragma once
//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: AutoOcr-class for OpenHoldem
//
//******************************************************************************

#ifndef INC_CAUTO_OCR_H
#define INC_CAUTO_OCR_H

#include <vector>
#include <regex>

#include "..\CTablemap\CTablemap.h"
#include "CSpaceOptimizedGlobalObject.h"

using namespace cv;
using namespace tesseract;

class CAutoOcr : public CSpaceOptimizedGlobalObject {
	friend class CScraper;
public:
	// public functions
	CAutoOcr();
	~CAutoOcr();
public:
	CString GetDetectTemplateResult(CString area_name, CString tpl_name, RECT* rect_result);
	vector<CString> GetDetectTemplatesResult(CString area_name);
	CString get_ocr_result(Mat img_orig, RMapCI region, bool fast = false);
private:
	RECT detectTemplate(Mat area, Mat tpl, int match_mode);
	void process_ocr(Mat img_orig, RMapCI region, bool fast = false, bool second_pass = false);
	Mat prepareImage(Mat img_orig, RMapCI region, bool binarize = true, int threshold = 100, bool second_pass = false);
	Mat binarize_array_opencv(Mat image, int threshold);

	string trim(string str) {
		return regex_replace(str, regex("\\s"), "");
	}

	float convertTofloat(const string& str) {
		float result;
		istringstream iss(str);
		iss >> result;
		return result;
	}

private:
	// Counter of GDI objects (potential memorz leak)
	// Should be 0 at end of program -- will be checked.
	int         _leaking_GDI_objects;

	vector<pair<Rect, CString>> ResultBoxes, ResultBoxes2;
	CString ResultString, ResultString2;
	Rect	bestRect, bestRect2;

	CCritSec		m_critsec;
};

extern CAutoOcr *p_auto_ocr;

#endif // INC_CAUTO_OCR_H