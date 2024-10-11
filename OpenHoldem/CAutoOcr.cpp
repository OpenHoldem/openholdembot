//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: AutoOcr class for OpenHoldem
//
//******************************************************************************

#include "StdAfx.h"
#include "CAutoOcr.h"

#include "CAutoconnector.h"


CAutoOcr *p_auto_ocr = NULL;

#define __HDC_HEADER 		HBITMAP		old_bitmap = NULL; \
	HDC				hdc = GetDC(p_autoconnector->attached_hwnd()); \
	HDC				hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL); \
	HDC				hdcCompatible = CreateCompatibleDC(hdcScreen); \
  ++_leaking_GDI_objects;

#define __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK \
  DeleteDC(hdcCompatible); \
	DeleteDC(hdcScreen); \
	ReleaseDC(p_autoconnector->attached_hwnd(), hdc); \
  --_leaking_GDI_objects;

// Declare DNN-Recognizer
//TextRecognitionModel recognizer;
TessBaseAPI* api = new TessBaseAPI;
TessBaseAPI* api2 = new TessBaseAPI;

//
// Constructor and destructor
//
CAutoOcr::CAutoOcr() {
	// New automatic OCR based on tesseract-ocr
	// Load Tesseract text recognition network
	if (api->Init("tessdata", "eng") == -1) {		// OEM_LSTM_ONLY
		MessageBox(NULL, "Failed to load tessdata files.\nMake sure tessdata folder is present and/or datas are not corrupted.", "AutoOcr error", MB_OK);
		return;
	}
	if (api2->Init("tessdata", "eng") == -1) {		// OEM_LSTM_ONLY
		MessageBox(NULL, "Failed to load tessdata files.\nMake sure tessdata folder is present and/or datas are not corrupted.", "AutoOcr error", MB_OK);
		return;
	}
}

CAutoOcr::~CAutoOcr() {
	// Unload network
	api->End();
	delete[] api;
	api2->End();
	delete[] api2;
}


////  Automatic Text Detection and Recognition functions  ///////////
Mat CAutoOcr::binarize_array_opencv(Mat image, int threshold) {
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

Mat CAutoOcr::prepareImage(Mat img_orig, RMapCI region, bool binarize, int threshold, bool second_pass) {
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

	// Cropping management ///////////////////////////
	if (region->second.use_cropping == true) {
		double crop_size = (double)region->second.crop_size / 100;
		if (crop_size < 0.01)
			return img_resized;

		process_ocr(img_resized, region, second_pass);
		vector<pair<Rect, CString>> resBoxes;
		if (second_pass)
			resBoxes = ResultBoxes2;
		else
			resBoxes = ResultBoxes;

		// Exit here if no box found (generally an empty region)
		if (resBoxes.empty())
			return img_resized;

		// filter contours
		vector<Rect> boundRect, boundRect2;
		img_bounded.convertTo(img_bounded, CV_8UC3);
		cvtColor(img_bounded, img_bounded, COLOR_GRAY2BGR);
		//m_crColor = m_BoxColor.GetSelectedColorValue();
		for (size_t idx = 0; idx < resBoxes.size(); idx++)
		{
			Rect rect = resBoxes[idx].first;
			if (rect.area() > 50) {
				boundRect.push_back(rect);
			}
		}

		// Exit if no valid box found (generally an empty region)
		if (boundRect.empty())
			return img_resized;

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
			if (boxArea[i] >  maxArea*(1 - crop_size)) {  // default to 40% diff factor for concurrent boxes
				maxIndex.push_back(i);
			}
		}
		for (size_t i = 0; i < maxIndex.size(); i++) {
			int j = maxIndex[i];
			best_rect = boundRect[j];
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

		return img_bounded;
	}
	/////////////////////////////////////////////////

	process_ocr(img_resized, region, second_pass);
	return img_resized;
}

void CAutoOcr::process_ocr(Mat img_orig, RMapCI region, bool fast, bool second_pass) {
	api->SetImage(img_orig.data, img_orig.cols, img_orig.rows, img_orig.channels(), img_orig.step);
	api->Recognize(0);

	if (region->second.use_cropping == true) {
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
				//printf("word: '%s';  \tconf: %.2f; BoundingBox: %d,%d,%d,%d;\n",
				//	word, conf, x1, y1, x2, y2);
			} while (ri->Next(level));
		}
	}
	else {
		if (second_pass)
			ResultString2 = trim(api->GetUTF8Text()).c_str();
		else
			ResultString = trim(api->GetUTF8Text()).c_str();
	}
}

CString CAutoOcr::get_ocr_result(Mat img_orig, RMapCI region, bool fast) {
	// Return string value from image. "" when OCR failed
	Mat img_resized, img_resized2;
	ResultBoxes.clear(); ResultBoxes2.clear();

	if (region->second.transform == "A0") {
		img_resized = prepareImage(img_orig, region, true);
		img_resized2 = prepareImage(img_orig, region, true, region->second.threshold, true);
	}
	if (region->second.transform == "A1") {
		img_resized = prepareImage(img_orig, region, true, region->second.threshold);
		img_resized2 = prepareImage(img_orig, region, true, 76, true);
	}

	vector<CString> lst;
	CString ocr_result, ocr_result2;
	if (region->second.use_cropping == true) {
		for (auto & element : ResultBoxes) {
			if (element.first == bestRect) {
				ocr_result = element.second;
				break;
			}
		}
		for (auto & element : ResultBoxes2) {
			if (element.first == bestRect2) {
				ocr_result2 = element.second;
				break;
			}
		}
	}
	else {
		img_resized.convertTo(img_resized, CV_8UC3);
		cvtColor(img_resized, img_resized, COLOR_GRAY2BGR);
		img_resized2.convertTo(img_resized2, CV_8UC3);
		cvtColor(img_resized2, img_resized2, COLOR_GRAY2BGR);

		ocr_result = ResultString;
		ocr_result2 = ResultString2;
	}

	// Clean results from unwanted chars
	const char* blacklist = "®©℗ⓒ™!%&*+;=?@²^æÆÇçÉéèêëïîíìÄÅÂÀàáâäåúùûüôöòñÑÿÖÜ€£¥₧ƒ~ªº¿⌐¬½¼¡«»\"`#<{([])}>|│░▒▓┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■";
	for (size_t i = 0; i < strlen(blacklist); i++) {
		if (ocr_result.Find(blacklist[i]) != -1)
			ocr_result.Replace(blacklist[i], '\0');
		if (ocr_result2.Find(blacklist[i]) != -1)
			ocr_result2.Replace(blacklist[i], '\0');
	}

	if (ocr_result != "")
		lst.push_back(ocr_result);
	if (ocr_result2 != "")
		lst.push_back(ocr_result2);


	// Display OCR recognition result
	try {
		if (!lst.empty()) {
			return lst.back();
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
				process_ocr(images[j], region);
				for (auto & element : ResultBoxes) {
					if (element.first == bestRect) {
						ocr_str = element.second;
						break;
					}
				}
				if (ocr_str != "")
					lst.push_back(ocr_str);
				j++;
			}
			i++;
		}
	}
#pragma warning(push)
#pragma warning(disable:4101)
	for (const auto& element : lst) {
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

#define ENT CSLock lock(m_critsec);
RECT CAutoOcr::detectTemplate(Mat area, Mat tpl, int match_mode) {
	//  Detect template	
	RECT result;
	int result_cols = area.cols - tpl.cols + 1;
	int result_rows = area.rows - tpl.rows + 1;
	// Invalid template size > area size
	if (result_cols < 1 || result_rows < 1)
		return result = RECT{ 0 };
	Mat match_roi = Mat(result_rows, result_cols, CV_32FC1);
	// Do the Matching and Normalize
	ENT matchTemplate(area, tpl, match_roi, match_mode);
	//normalize(match_roi, match_roi, 0, 1, NORM_MINMAX, -1, Mat());_CrtMemCheckpoint(&sNew); 

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
#undef ENT

CString CAutoOcr::GetDetectTemplateResult(CString area_name, CString tpl_name, RECT* rect_result) {
	//  Detect template
	CString				text, selected_transform, separation;
	RMapCI				r_iter = p_tablemap->r$()->find(area_name.GetString());
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();
	int		 width, height, match_mode;

	// Exit because the area doesn't exist
	if (r_iter == p_tablemap->r$()->end()) {
		return "false";
	}

	__HDC_HEADER
	for (sel_template = p_tablemap->tpl$()->begin(); sel_template != p_tablemap->tpl$()->end(); sel_template++) {
		// Pass if template isn't created
		if (!sel_template->second.created)
			continue;

		// Pass if it isn't the target template
		CString name = sel_template->second.name;
		name.Replace("button_action_", ""); name.Replace("button_", "");
		if (name.Left(tpl_name.GetLength()) != tpl_name)
			continue;

		// Get template size
		width = sel_template->second.width;
		height = sel_template->second.height;
		match_mode = sel_template->second.match_mode;
		if (width < 1 || height < 1 || match_mode < 0)
			continue;

		// Bitblt the area into a HDC
		// Get bitmap size
		int w = r_iter->second.right - r_iter->second.left + 1;
		int h = r_iter->second.bottom - r_iter->second.top + 1;

		old_bitmap = (HBITMAP)SelectObject(hdcCompatible, r_iter->second.cur_bmp);

		BitBlt(hdcCompatible, 0, 0, w, h,
			hdc,
			r_iter->second.left - 1, r_iter->second.top - 1,
			SRCCOPY);
		SelectObject(hdcCompatible, old_bitmap);

		Mat area_mat(h, w, CV_8UC4);
		BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
		GetDIBits(hdc, r_iter->second.cur_bmp, 0, h, area_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
		//SaveHBITMAPToFile(bitmap_transform, "output.bmp");

		//imshow("Area Image", area_mat);
		//waitKey();

		int					x, y;
		HBITMAP				bitmap_image;
		BYTE				*pBits, alpha, red, green, blue;
		RECT roi, zero_rect = RECT{ 0 };

		bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
		old_bitmap = (HBITMAP)SelectObject(hdcCompatible, bitmap_image);

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
				alpha = (sel_template->second.pixel[y*width + x] >> 24) & 0xff;
				red = (sel_template->second.pixel[y*width + x] >> 0) & 0xff;
				green = (sel_template->second.pixel[y*width + x] >> 8) & 0xff;
				blue = (sel_template->second.pixel[y*width + x] >> 16) & 0xff;

				// SetDIBits format is BGRA
				pBits[y*width * 4 + x * 4 + 0] = blue;
				pBits[y*width * 4 + x * 4 + 1] = green;
				pBits[y*width * 4 + x * 4 + 2] = red;
				pBits[y*width * 4 + x * 4 + 3] = alpha;
			}
		}
		::SetDIBits(hdcCompatible, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

		Mat template_mat(height, width, CV_8UC4);
		template_mat.data = pBits;
		//imshow("Template Image", template_mat);
		//waitKey();

		roi = detectTemplate(area_mat, template_mat, match_mode);
		if (!EqualRect(&roi, &zero_rect)) {
			if (area_name.Find("area_buttons_zone") != -1) {
				*rect_result = RECT{ (int)r_iter->second.left + roi.left, (int)r_iter->second.top + roi.top,
					(int)r_iter->second.left + roi.right, (int)r_iter->second.top + roi.bottom };

				// Clean up
				SelectObject(hdcCompatible, old_bitmap);
				DeleteObject(bitmap_image);
				// Free memory
				delete []pBits;
			__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
				return "true";
			}
		}
		// Clean up
		SelectObject(hdcCompatible, old_bitmap);
		DeleteObject(bitmap_image);
		// Free memory
		delete[]pBits;
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	}

	// Clean up
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK

	*rect_result = RECT{ 0 };
	return "false";
}

vector<CString> CAutoOcr::GetDetectTemplatesResult(CString area_name) {
	//  Detect template
	CString				text, selected_transform, separation;
	RMapCI				r_iter = p_tablemap->r$()->find(area_name.GetString());
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();

	// Exit because the area doesn't exist
	if (r_iter == p_tablemap->r$()->end()) {
		return{ "" };
	}

	__HDC_HEADER
	// Bitblt the area bitmap into a HDC
	// Get bitmap size
	int w = r_iter->second.right - r_iter->second.left + 1;
	int h = r_iter->second.bottom - r_iter->second.top + 1;

	old_bitmap = (HBITMAP)SelectObject(hdcCompatible, r_iter->second.cur_bmp);

	BitBlt(hdcCompatible, 0, 0, w, h,
		hdc,
		r_iter->second.left - 1, r_iter->second.top - 1,
		SRCCOPY);
	SelectObject(hdcCompatible, old_bitmap);

	Mat area_mat(h, w, CV_8UC4);
	BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
	GetDIBits(hdc, r_iter->second.cur_bmp, 0, h, area_mat.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	//SaveHBITMAPToFile(bitmap_transform, "output.bmp");

	//imshow("Area Image", area_mat);
	//waitKey();


	vector<pair<int, CString>> listROI;

	// Get search templates in area
	for (sel_template = p_tablemap->tpl$()->begin(); sel_template != p_tablemap->tpl$()->end(); sel_template++) {
		int					x, y, width, height, match_mode;
		HBITMAP				bitmap_image;
		BYTE				*pBits, alpha, red, green, blue;
		RECT roi, zero_rect = RECT{ 0 };
		if (area_name == "area_cards_common" && sel_template->second.name.Find("card_common_") == -1 || !sel_template->second.created)
			continue;
		if (area_name.Find("area_cards_player") != -1 && sel_template->second.name.Find("card_player_") == -1 || !sel_template->second.created) {
			if (area_name.Find("area_cards_player") != -1 && sel_template->second.name.Find("card_common_") == -1 || !sel_template->second.created)
				continue;
		}
		// Get template size
		width = sel_template->second.width;
		height = sel_template->second.height;
		match_mode = sel_template->second.match_mode;
		if (width < 1 || height < 1 || match_mode < 0)
			continue;

		bitmap_image = CreateCompatibleBitmap(hdcScreen, width, height);
		old_bitmap = (HBITMAP)SelectObject(hdcCompatible, bitmap_image);

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
				alpha = (sel_template->second.pixel[y*width + x] >> 24) & 0xff;
				red = (sel_template->second.pixel[y*width + x] >> 0) & 0xff;
				green = (sel_template->second.pixel[y*width + x] >> 8) & 0xff;
				blue = (sel_template->second.pixel[y*width + x] >> 16) & 0xff;

				// SetDIBits format is BGRA
				pBits[y*width * 4 + x * 4 + 0] = blue;
				pBits[y*width * 4 + x * 4 + 1] = green;
				pBits[y*width * 4 + x * 4 + 2] = red;
				pBits[y*width * 4 + x * 4 + 3] = alpha;
			}
		}
		::SetDIBits(hdcCompatible, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

		Mat template_mat(height, width, CV_8UC4);
		template_mat.data = pBits;
		//imshow("Template Image", template_mat);
		//waitKey();

		roi = detectTemplate(area_mat, template_mat, match_mode);
		if (!EqualRect(&roi, &zero_rect)) {
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
		}
		// Clean up
		SelectObject(hdcCompatible, old_bitmap);
		DeleteObject(bitmap_image);
		// Free memory
		delete []pBits;
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	}

	// Order ROI array
	sort(listROI.begin(), listROI.end());

	// Display result
	vector<CString> result;
	for (auto & element : listROI) {
		result.push_back(element.second);
	}
	//m_Result.SetWindowText(result);


	// Clean up
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK

	return result;
}

//////////////////////////////////////////////////////////////////////////