//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Reading the poker-table.
//  State-less class for future multi-table support.
//  All data is now in the CTable'state container.
//
//******************************************************************************

#include "StdAfx.h"
#include "CScraper.h"

#include "Bitmaps.h" 
#include "CardFunctions.h"
#include "CAutoconnector.h"
#include "CCasinoInterface.h"
#include "CEngineContainer.h"

#include "CStringMatch.h"
#include "CSymbolEngineActiveDealtPlaying.h"
#include "CSymbolEngineAutoplayer.h"
#include "CSymbolEngineCasino.h"
#include "CSymbolEngineDebug.h"
#include "CSymbolEngineHistory.h"
#include "CSymbolEngineIsOmaha.h"
#include "CSymbolEngineMTTInfo.h"
#include "CSymbolEngineUserchair.h"
#include "CSymbolEngineTableLimits.h"
#include "CTableState.h"
#include "CTableTitle.h"
#include "CTitleEvaluator.h"
#include "..\CTransform\CTransform.h"
#include "..\CTransform\hash\lookup3.h"

#include "MainFrm.h"
#include "OpenHoldem.h"

CScraper *p_scraper = NULL;

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
TessBaseAPI api;

////  Automatic Text Detection and Recognition functions  ///////////

void CScraper::detectMotion(void) {
	/*
	//Subtract consecutive frames
	cv::Mat matN = cv::imread("frame1.bmp");
	cv::Mat matM = cv::imread("frame2.bmp");
	cv::Mat matDiff2 = abs(matM - matN);
	Mat matDiff;
	cvtColor(matDiff2, matDiff, COLOR_BGR2GRAY);
	//imshow("test", matDiff);
	//waitKey();

	//Set region of interest (subframe)
	int x(0), y(0), width(57), height(67);
	cv::Rect myRegionOfInterest(x, y, width, height);

	//Define motion
	double Threshold = 0.1;
	double nze = cv::countNonZero(matDiff(myRegionOfInterest));
	double motionFactor = nze / (width*height*1); //C=255 for uchar, C=1 for binary, etc.
	if (motionFactor>Threshold)
	cout << "Motion detected at specific ROI";
	*/
}

void CScraper::detectBlobs(void) {
	/*
	imshow("AutoOcr view", img_resized);
	imshow("AutoOcr view 2", img_resized2);
	waitKey();

	// Set up the detector with default parameters.
	Ptr<SimpleBlobDetector> detector;
	detector = SimpleBlobDetector::create();

	// Detect blobs.
	vector<KeyPoint> keypoints;
	detector->detect(img_resized, keypoints);

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of blob
	Mat im_with_keypoints, im_with_keypoints2;
	drawKeypoints(img_resized, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);


	detector->detect(img_resized2, keypoints);
	drawKeypoints(img_resized2, keypoints, im_with_keypoints2, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	// Show blobs
	imshow("AutoOcr view", im_with_keypoints);
	imshow("AutoOcr view 2", im_with_keypoints2);
	waitKey();
	*/
}

void CScraper::fourPointsTransform(const Mat& frame, const Point2f vertices[], Mat& result)
{
	const Size outputSize = Size(100, 32);
	Point2f targetVertices[4] = {
		Point(0, outputSize.height - 1),
		Point(0, 0), Point(outputSize.width - 1, 0),
		Point(outputSize.width - 1, outputSize.height - 1)
	};
	Mat rotationMatrix = getPerspectiveTransform(vertices, targetVertices);
	warpPerspective(frame, result, rotationMatrix, outputSize);
}

void CScraper::textDetectAndRecognize(void) {
	float confThreshold = 0.5;
	float nmsThreshold = 0.4;
	int width = 320;
	int height = 320;
	int imreadRGB = 0;
	String detModelPath = "frozen_east_text_detection.pb";
	String recModelPath = "CRNN_VGG_BiLSTM_CTC.onnx";
	String vocPath = "alphabet_94.txt";
	// Load networks.
	CV_Assert(!detModelPath.empty() && !recModelPath.empty());
	TextDetectionModel_EAST detector(detModelPath);
	detector.setConfidenceThreshold(confThreshold)
		.setNMSThreshold(nmsThreshold);
	TextRecognitionModel recognizer(recModelPath);
	// Load vocabulary
	CV_Assert(!vocPath.empty());
	ifstream vocFile;
	//vocFile.open(samples::findFile(vocPath));
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
	// Parameters for Detection
	double detScale = 1.0;
	Size detInputSize = Size(width, height);
	Scalar detMean = Scalar(123.68, 116.78, 103.94);
	bool swapRB = true;
	detector.setInputParams(detScale, detInputSize, detMean, swapRB);
	// Detection
	//Mat frame = img_resized.clone();
	//Mat frame = imread("frame000001.bmp");
	Mat frame;
	vector< vector<Point> > detResults;
	detector.detect(frame, detResults);
	Mat frame2 = frame.clone();
	if (detResults.size() > 0) {
		// Text Recognition
		Mat recInput;
		if (!imreadRGB) {
			cvtColor(frame, recInput, cv::COLOR_BGR2GRAY);
		}
		else {
			recInput = frame;
		}
		vector< vector<Point> > contours;
		for (uint i = 0; i < detResults.size(); i++)
		{
			const auto& quadrangle = detResults[i];
			CV_CheckEQ(quadrangle.size(), (size_t)4, "");
			contours.emplace_back(quadrangle);
			vector<Point2f> quadrangle_2f;
			for (int j = 0; j < 4; j++)
				quadrangle_2f.emplace_back(quadrangle[j]);
			Mat cropped;
			fourPointsTransform(recInput, &quadrangle_2f[0], cropped);
			std::string recognitionResult = recognizer.recognize(cropped);
			cout << i << ": '" << recognitionResult << "'" << endl;
			putText(frame2, recognitionResult, quadrangle[3], FONT_HERSHEY_SIMPLEX, 1.5, Scalar(0, 0, 255), 2);
		}
		polylines(frame2, contours, true, Scalar(255, 255, 255), 1);
	}
	imshow("Output", frame2);
	waitKey();
}

Mat CScraper::binarize_array_opencv(Mat image, int threshold) {
	// Binarize image from gray channel with 76 as threshold
	Mat img;
	cvtColor(image, img, COLOR_BGR2RGB);
	cvtColor(img, img, COLOR_BGR2GRAY);
	Mat thresh;
	cv::threshold(img, thresh, threshold, 255, THRESH_BINARY_INV);
	return thresh;
}

void CScraper::detectText(void) {
	// Load model weights
	TextDetectionModel_DB model("DB_TD500_resnet18.onnx");
	// Post-processing parameters
	float binThresh = 0.1;
	float polyThresh = 0.1;
	uint maxCandidates = 20000;
	double unclipRatio = 2.5;
	model.setBinaryThreshold(binThresh)
		.setPolygonThreshold(polyThresh)
		.setMaxCandidates(maxCandidates)
		.setUnclipRatio(unclipRatio)
		;
	//Mat input = imread("frame000001.bmp");
	Mat input;
	/*
	cvtColor(input, input, COLOR_BGR2GRAY);
	input.convertTo(input, -1, 2, 0);	// changing contrast
	imshow("Contrasted", input);
	waitKey();
	*/
	// Normalization parameters
	double scale = 1.0 / 255.0;
	Scalar mean = Scalar(122.67891434, 116.66876762, 104.00698793);
	// The input shape
	Size inputSize = Size(1280, 736);
	model.setInputParams(scale, inputSize, mean);
	vector<vector<Point>> detResults;
	vector<RotatedRect> rectDetResults;
	//model.detect(input, detResults);
	model.detectTextRectangles(input, rectDetResults);
	// Visualization
	//polylines(input, detResults, true, Scalar(255, 255, 255), 1);
	for (size_t i = 0; i < rectDetResults.size(); i++)
		rectangle(input, rectDetResults[i].boundingRect(), Scalar(255, 0, 0), 1);
	imshow("Text Detection", input);
	waitKey();
}

Mat CScraper::prepareImage(Mat img_orig, Mat* img_cropped, RMapCI region, bool binarize, int threshold) {
	// Prepare image for OCR
	//  !!  Do not change those settings and values !!   //
	//  Or display on OCR view control will be distorded //
	//imshow("Output", img_orig);
	//waitKey();
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
	resize(img_orig, *img_cropped, Size(basewidth, hsize), INTER_LANCZOS4);

	if (binarize) {
		img_resized = binarize_array_opencv(img_resized, threshold);
	}
	Mat img_bounded = img_resized.clone();

	// Cropping management ///////////////////////////
	if (region->second.use_cropping == true) {
		// downsample and use it for processing
		//pyrDown(*img_cropped, *img_cropped);
		int crop_size = region->second.crop_size;
		if (crop_size < 2)
			return *img_cropped;
		Mat gray;
		cvtColor(*img_cropped, gray, COLOR_BGR2GRAY);
		// morphological gradient
		Mat grad;
		Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(crop_size, crop_size));
		morphologyEx(gray, grad, MORPH_GRADIENT, morphKernel);
		// binarize
		Mat bw;
		cv::threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
		// connect horizontally oriented regions
		Mat connected;
		morphKernel = getStructuringElement(MORPH_RECT, Size(1, 1));
		morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
		// find contours
		Mat mask = Mat::zeros(bw.size(), CV_8UC1);
		vector<vector<Point>> contours;
		vector<Vec4i> hierarchy;
		findContours(connected, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE, Point(0, 0));
		// filter contours
		vector<Rect> boundRect, boundRect2;
		img_bounded.convertTo(img_bounded, CV_8UC3);
		cvtColor(img_bounded, img_bounded, COLOR_GRAY2BGR);
		int m_crColor = region->second.box_color;
		for (size_t idx = 0; idx < contours.size(); idx++)
		{
			Rect rect = boundingRect(contours[idx]);
			if (contours[idx].size() > 50) {
				boundRect.push_back(rect);
				rectangle(*img_cropped, rect, Scalar(GetBValue(m_crColor), GetGValue(m_crColor), GetRValue(m_crColor)), 1);
				rectangle(img_bounded, rect, Scalar(GetBValue(m_crColor), GetGValue(m_crColor), GetRValue(m_crColor)), 1);
			}
		}

		vector<double> boxArea, boxDist;
		double wCenter = img_cropped->cols / 2;
		double hCenter = img_cropped->rows / 2;
		Point pCenter(wCenter, wCenter);
		Rect bestRect;
		////  Find best box match for recognition
		// First find biggest box(es)
		for (size_t i = 0; i < boundRect.size(); i++) {
			boxArea.push_back(boundRect[i].width * boundRect[i].height);
		}
		// Select the element with the maximum value
		auto ita = max_element(boxArea.begin(), boxArea.end());
		int maxArea = *ita;
		vector<int> maxIndex;
		for (size_t i = 0; i < boxArea.size(); i++) {
			if (boxArea[i] * 1.2 > maxArea) {
				maxIndex.push_back(i);
			}
		}
		for (size_t i = 0; i < maxIndex.size(); i++) {
			int j = maxIndex[i];
			boundRect[j].x = boundRect[j].x + 2;
			boundRect[j].width = boundRect[j].width - 4;
			boundRect[j].y = boundRect[j].y + 2;
			boundRect[j].height = boundRect[j].height - 4;
			bestRect = boundRect[j];
			boundRect2.push_back(bestRect);
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
					boundRect2[i].x = boundRect2[i].x + 2;
					boundRect2[i].width = boundRect2[i].width - 4;
					boundRect2[i].y = boundRect2[i].y + 2;
					boundRect2[i].height = boundRect2[i].height - 4;
					bestRect = boundRect2[i];
					break;
				}
			}
		}
		// Draw best box
		rectangle(img_bounded, bestRect, Scalar(GetBValue(m_crColor), GetGValue(m_crColor), GetRValue(m_crColor)), 2);

		//if (!img_bounded.empty())
		//	imshow("Output 1", img_bounded);
		//waitKey();

		*img_cropped = img_resized(bestRect);

		// Add border to bestRect ROI + resize it to fit 30-33px height for capital letter
		// for optimal 0% error rate on Tesseract recognition: https://groups.google.com/g/tesseract-ocr/c/Wdh_JJwnw94/m/24JHDYQbBQAJ
		const int border = 25; // 10
		//const int borderType = BORDER_CONSTANT | BORDER_ISOLATED;
		const Scalar value(255, 255, 255);
		const Mat roi(img_cropped->rows + 2 * border, img_cropped->cols + 2 * border, CV_8UC1, value);
		img_cropped->copyTo(roi(Rect(border, border, img_cropped->cols, img_cropped->rows)));
		*img_cropped = roi;
		int height = 70; // 29 with 10 border
		float pct = (height / static_cast<float>(img_cropped->rows));
		int width = static_cast<int>(static_cast<float>(img_cropped->cols) * pct);
		resize(*img_cropped, *img_cropped, Size(width, height), INTER_LANCZOS4);

		//if (!img_cropped->empty())
		//	imshow("Output 2", *img_cropped);
		//waitKey();
		return img_bounded;
	}
	///////////////////////////////////////////

	if (binarize) {
		img_resized = binarize_array_opencv(img_resized, threshold);
	}

	*img_cropped = img_resized;
	return img_resized;
}

CString CScraper::process_ocr(Mat img_orig, RMapCI region, bool fast) {
	/*
	CString sel_region = region->second.name;
	if (sel_region.Find("handnumber") != -1 || sel_region.Find("limits") != -1 || sel_region.Find("stack") != -1 || sel_region.Find("mtt") != -1 || sel_region.Find("prize") != -1 ||
		sel_region.Find("c0pot") != -1 || sel_region.Find("bet", 2) != -1 || sel_region.Find("balance") != -1 || sel_region.Find("chip") != -1 ||
		sel_region.Find("ante") != -1 || sel_region.Find("smallblind") != -1 || sel_region.Find("bigblind") != -1 || sel_region.Find("bigbet") != -1)

		api.SetVariable("tessedit_char_whitelist", "0123456789.,$¢");
	*/
	//else
	//	api.SetVariable("tessedit_char_blacklist", "®©℗ⓒ™!%&*+/;=?@²^æÆÇçÉéèêëïîíìÄÅàáâäåúùûüôöòñÑÿÖÜ€$¢£¥₧ƒ~ªº¿⌐¬½¼¡«»\"'`#<{([])}>|│░▒▓┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■");

	/*
	PIX* pix = pixCreate(img_orig.cols, img_orig.rows, 8);
	for (int i = 0; i<img_orig.rows; i++)
		for (int j = 0; j<img_orig.cols; j++)
			pixSetPixel(pix, j, i, (l_uint32)img_orig.at<uchar>(i, j));
	api.SetImage(pix);
	*/

	api.SetImage(img_orig.data, img_orig.cols, img_orig.rows, img_orig.channels(), img_orig.step);
	//imshow("AutoOcr view", img_orig);
	//waitKey();
	//int ret = api.Recognize(0);
	CString result = trim(api.GetUTF8Text()).c_str();
	
	const char* blacklist = "®©℗ⓒ™!%&*+;=?@²^æÆÇçÉéèêëïîíìÄÅàáâäåúùûüôöòñÑÿÖÜ€£¥₧ƒ~ªº¿⌐¬½¼¡«»\"`#<{([])}>|│░▒▓┤╡╢╖╕╣║╗╝╜╛┐└┴┬├─┼╞╟╚╔╩╦╠═╬╧╨╤╥╙╘╒╓╫╪┘┌█▄▌▐▀αßΓπΣσµτΦΘΩδ∞φε∩≡±≥≤⌠⌡÷≈°∙·√ⁿ²■";
	for (size_t i = 0; i < strlen(blacklist); i++)
		if (result.Find(blacklist[i]) != -1)
			result.Replace(blacklist[i], '\0');
	
	return result;
}

CString CScraper::get_ocr_result(Mat img_orig, RMapCI region, bool fast) {
	// Return string value from image. "" when OCR failed
	Mat img_resized, img_resized2;
	Mat* img_cropped = new Mat();
	Mat* img_cropped2 = new Mat();

	if (force_auto_ocr) {
		img_resized = prepareImage(img_orig, img_cropped, region, true);
		img_resized2 = prepareImage(img_orig, img_cropped2, region, true, 125);
	}

	if (region->second.transform == "A0") {
		img_resized = prepareImage(img_orig, img_cropped, region, true);
		img_resized2 = prepareImage(img_orig, img_cropped2, region, true, region->second.threshold);
	}
	if (region->second.transform == "A1") {
		img_resized = prepareImage(img_orig, img_cropped, region, true, region->second.threshold);
		img_resized2 = prepareImage(img_orig, img_cropped2, region, true);
	}

	if (region->second.use_cropping == false) {
		img_resized.convertTo(img_resized, CV_8UC3);
		cvtColor(img_resized, img_resized, COLOR_GRAY2BGR);
		img_resized2.convertTo(img_resized2, CV_8UC3);
		cvtColor(img_resized2, img_resized2, COLOR_GRAY2BGR);
	}

	vector<CString> lst;
	CString ocr_result = process_ocr(*img_cropped, region);
	CString ocr_result2 = process_ocr(*img_cropped2, region);

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
				CString ocr_str = process_ocr(images[j], region);
				if (ocr_str != "")
					lst.push_back(ocr_str);
				j += 1;
			}
			i += 1;
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

//////////////////////////////////////////////////////////////////////////




////  Automatic Template Detection functions  ////////////////////////////

Mat CScraper::loadTemplate(std::string name) {
	// Load template
	FileStorage fs;
	Mat mat;
	try
	{
		fs.open("Templates.xml", FileStorage::READ);
	}
	catch (cv::Exception& e)
	{
		CString err_msg = "Failed to open Template file.\nIt seems that your Templates.xml file is invalid or corrupted.\nOCR engine returned error:\n" + (CString)e.what();
		MessageBox(NULL, err_msg, "AutoOcr error", MB_OK);
		return mat;
	}
	//read(fs, "Input", Matrix);
	fs[name] >> mat;
	fs.release();

	//imshow("Template", mat);
	//waitKey();
	return mat;
}

void CScraper::deleteTemplate(std::string name) {
	// Delete template
	FileStorage fs;
	fstream tplFile("Templates.xml");
	string content((istreambuf_iterator<char>(tplFile)),
		(istreambuf_iterator<char>()));
	unsigned first, last;

	if (content.find(" <!-- resumed -->\n\n") != -1) {
		first = content.find(" <!-- resumed -->\n\n");
		last = first + 19;
		content.replace(first, last - first, "");
	}
	first = content.find("<" + name);
	last = content.find("</" + name + ">\n") + name.length() + 4;
	content.replace(first, last - first, "");
	tplFile.close();
	tplFile.open("Templates.xml", ios::out | ios::trunc);
	tplFile << content;
	tplFile.close();
	fs.release();
}

void CScraper::createTemplate(Mat input, string name) {
	// Create template
	FileStorage fs;
	FileNode node;

	try
	{
		fs.open("Templates.xml", FileStorage::READ);
		node = fs[name]; // Read string sequence - Get node
	}
	catch (cv::Exception& e)
	{
		CString err_msg = "Failed to open Template file.\nIt seems that your Templates.xml file is invalid or corrupted.\nOCR engine returned error:\n" + (CString)e.what();
		MessageBox(NULL, err_msg, "AutoOcr error", MB_OK);
		return;
	}

	if (!node.isNone())
		deleteTemplate(name);

	fs.release();
	fs.open("Templates.xml", FileStorage::APPEND);
	fs << name << input;
	fs.release();
}
RECT CScraper::detectTemplate(Mat area, Mat tpl, int match_mode) {
	//  Detect template	
	RECT result;
	Mat resultImg;
	int result_cols = area.cols - area.cols + 1;
	int result_rows = area.rows - area.rows + 1;
	resultImg.create(result_rows, result_cols, CV_32FC1);
	// Do the Matching and Normalize
	matchTemplate(area, tpl, resultImg, match_mode);
	//normalize(resultImg, resultImg, 0, 1, NORM_MINMAX, -1, Mat());

	/// Localizing the best match with minMaxLoc
	double matchVal, minVal, maxVal;
	Point minLoc, maxLoc, matchLoc;

	minMaxLoc(resultImg, &minVal, &maxVal, &minLoc, &maxLoc);

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
	if (-0.08 <= matchVal && matchVal <= 0.08 || 0.92 <= matchVal && matchVal <= 1.08) {  // 0.08% tolerance
		result = RECT{ matchLoc.x, matchLoc.y , matchLoc.x + tpl.cols, matchLoc.y + tpl.rows };

		// Show me what you got
		//rectangle(area, matchLoc, Point(matchLoc.x + tpl.cols, matchLoc.y + tpl.rows), Scalar(0, 255, 0), 2, 8, 0);
		//rectangle(resultImg, matchLoc, Point(matchLoc.x + tpl.cols, matchLoc.y + tpl.rows), Scalar(0, 255, 0), 2, 8, 0);

		//imshow("Result view", area);
		//imshow("Result transform view", resultImg);
		//waitKey();
	}
	else
		// No match found
		result = RECT{ 0 };

	return result;
}
//////////////////////////////////////////////////////////////////////////



CScraper::CScraper(void) {
	p_table_state->Reset();
  _leaking_GDI_objects = 0;
  total_region_counter = 0;
  identical_region_counter = 0;

  force_auto_ocr = false;

  // New automatic OCR based on tesseract-ocr
  // Load Tesseract text recognition network
  if (api.Init("tessdata", "eng") == -1) {		// OEM_LSTM_ONLY
	  MessageBox(NULL, "Failed to load tessdata files.\nMake sure tessdata folder is present and/or datas are not corrupted.", "AutoOcr error", MB_OK);
	  return;
  }
  //api.SetPageSegMode(PSM_SINGLE_LINE);
  //api.SetVariable("user_defined_dpi", "300");

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
}

CScraper::~CScraper(void) {
	p_table_state->Reset();
  if (_leaking_GDI_objects != 0 ) {
    write_log(k_always_log_errors, "[CScraper] ERROR! Leaking GDI objects: %i\n",
      _leaking_GDI_objects);
    write_log(k_always_log_errors, "[CScraper] Please get in contact with the development team\n");
  }
  assert(_leaking_GDI_objects == 0);
  write_log(true, "[CScraper] Total regions scraped %i\n",
    total_region_counter);
  write_log(true, "[CScraper] Identical regions scraped %i\n",
    identical_region_counter);

  // Unload network
  api.End();	   
}

bool CScraper::ProcessRegion(RMapCI r_iter) {
  write_log(Preferences()->debug_scraper(),
    "[CScraper] ProcessRegion %s (%i, %i, %i, %i)\n",
    r_iter->first, r_iter->second.left, r_iter->second.top,
    r_iter->second.right, r_iter->second.bottom);
  write_log(Preferences()->debug_scraper(),
    "[CScraper] ProcessRegion color %i radius %i transform %s\n",
    r_iter->second.color, r_iter->second.radius, r_iter->second.transform);
	__HDC_HEADER
	// Get "current" bitmap
	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
	if (r_iter->second.transform[0] == 'A') {
		BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 7,
			r_iter->second.bottom - r_iter->second.top + 7,
			hdc, r_iter->second.left - 3, r_iter->second.top - 3, SRCCOPY);
	}
	else {
		BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 1, 
									r_iter->second.bottom - r_iter->second.top + 1, 
									hdc, r_iter->second.left, r_iter->second.top, SRCCOPY);
	}
	SelectObject(hdcCompatible, old_bitmap);
	//SaveHBITMAPToFile(r_iter->second.cur_bmp, "output.bmp");

	// If the bitmaps are different, then continue on
	if (!BitmapsAreEqual(r_iter->second.last_bmp, r_iter->second.cur_bmp)) {
    // Copy into "last" bitmap
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.last_bmp);
		if (r_iter->second.transform[0] == 'A') {
			BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 7,
				r_iter->second.bottom - r_iter->second.top + 7,
				hdc, r_iter->second.left - 3, r_iter->second.top - 3, SRCCOPY);
		}
		else {
			BitBlt(hdcCompatible, 0, 0, r_iter->second.right - r_iter->second.left + 1,
				r_iter->second.bottom - r_iter->second.top + 1,
				hdc, r_iter->second.left, r_iter->second.top, SRCCOPY);
		}
		SelectObject(hdcCompatible, old_bitmap);  
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	return false;
}

CString CScraper::RecognizeText(Mat input) {
	float confThreshold = 0.5;
	float nmsThreshold = 0.4;
	int width = 736;
	int height = 736;
	int imreadRGB = 1;
	String detModelPath = "DB_TD500_resnet50.onnx";
	String recModelPath = "crnn_cs.onnx";
	String vocPath = "alphabet_94.txt";
	// Load networks.
	CV_Assert(!detModelPath.empty() && !recModelPath.empty());
	//TextDetectionModel_DB detector(detModelPath);
	//detector.setConfidenceThreshold(confThreshold)
	//	.setNMSThreshold(nmsThreshold);
	TextRecognitionModel recognizer(recModelPath);
	// Load vocabulary
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
	// Text Recognition
	Mat recInput;
	if (!imreadRGB) {
		cvtColor(input, recInput, cv::COLOR_BGR2GRAY);
	}
	else
		recInput = input;
	cvtColor(input, recInput, cv::COLOR_BGR2RGB);

	//resize(recInput, recInput, Size(recInput.cols*1.5, recInput.rows*1.5), INTER_LANCZOS4);
	//imshow("Output", recInput);
	//waitKey();
	//recInput = imread("5228_8.png", IMREAD_COLOR);
	//input.convertTo(input, CV_8UC4);
	CString recognitionResult = recognizer.recognize(recInput).c_str();
	return recognitionResult;
}
bool CScraper::EvaluateRegion(CString name, CString *result) {
  __HDC_HEADER
  write_log(Preferences()->debug_scraper(),
    "[CScraper] EvaluateRegion %s\n", name);
	CTransform	trans;
	RMapCI		r_iter = p_tablemap->r$()->find(name.GetString());
	if (r_iter != p_tablemap->r$()->end()) {
    // Potential for optimization here
    ++total_region_counter;
		if (ProcessRegion(r_iter)) {
      ++identical_region_counter;
      write_log(Preferences()->debug_scraper(),
        "[CScraper] Region %s identical\n", name);
    } else {
      write_log(Preferences()->debug_scraper(),
        "[CScraper] Region %s NOT identical\n", name);
    }
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
		if (r_iter->second.transform[0] == 'A'  || force_auto_ocr) {
			int w = r_iter->second.right - r_iter->second.left + 7;
			int h = r_iter->second.bottom - r_iter->second.top + 7;
			Mat input(h, w, CV_8UC4);
			BITMAPINFOHEADER bi = { sizeof(bi), w, -h, 1, 32, BI_RGB };
			GetDIBits(hdc, r_iter->second.cur_bmp, 0, h, input.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
			//imshow("Output", input);
			//waitKey();
			*result = get_ocr_result(input, r_iter).GetString();
			// OpenCV DNN text recognition
			/*
			bool imreadRGB = false;
			if (!imreadRGB) {
				cvtColor(input, input, cv::COLOR_BGR2GRAY);
			}
			//imshow("Output", input);
			//waitKey();
			*result = RecognizeText(input);
			*/
		}
		else
			trans.DoTransform(r_iter, hdcCompatible, result);
		SelectObject(hdcCompatible, old_bitmap);
		write_log(Preferences()->debug_scraper(), "[CScraper] EvaluateRegion(), [%s] -> [%s]\n", 
			name, *result);
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	// Region does not exist
  *result = "";
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	return false;
}

// Result will be changed to true, if "true" or something similar is found
// Result will be changed to falsee, if "false" or something similar is found
// Otherwise unchanged (keep default / allow multiple evaluations)
void CScraper::EvaluateTrueFalseRegion(bool *result, const CString name) {
  CString text_result;
	if (EvaluateRegion(name, &text_result))	{
    write_log(Preferences()->debug_scraper(), "[CScraper] %s result %s\n", 
      name, text_result.GetString());
    if (text_result == "true") {
      *result = true;
    } else if (text_result == "false") {
      *result = false;
    }
	}
}

void CScraper::ScrapeInterfaceButtons() {
	CString result;
	// i86X-buttons
	CString button_name;
	for (int i=0; i<k_max_number_of_i86X_buttons; i++) {
		button_name.Format("i86%dstate", i);
		if (EvaluateRegion(button_name, &result))	{
      p_casino_interface->_technical_i86X_spam_buttons[i].SetState(result);
		}
	}
}

void CScraper::ScrapeActionButtons() {
	CString button_name;
	CString result;
	for (int i=0; i<k_max_number_of_buttons; ++i)	{
		button_name.Format("i%cstate", HexadecimalChar(i)); 
		if (EvaluateRegion(button_name, &result)) {
      p_casino_interface->_technical_autoplayer_buttons[i].SetState(result);
		}
    if ((i == 5) && p_engine_container->symbol_engine_casino()->ConnectedToManualMode()) {
      // Ugly WinHoldem convention
      // When using ManualMode, grab i5state for PT network
      p_tablemap->set_network(result);
    }
	}
}

void CScraper::ScrapeActionButtonLabels() {
	CString label;
	CString result;
  // Every button needs a label
  // No longer using any WinHoldem defaults
	for (int i=0; i<k_max_number_of_buttons; ++i)	{
    p_casino_interface->_technical_autoplayer_buttons[i].SetLabel("");
		label.Format("i%clabel", HexadecimalChar(i));
		if (EvaluateRegion(label, &result))	{
      p_casino_interface->_technical_autoplayer_buttons[i].SetLabel(result);
		}
	}
}

void CScraper::ScrapeBetpotButtons() {
	CString button_name;
	CString result;
	for (int i=0; i<k_max_betpot_buttons; i++) {
    button_name.Format("%sstate", k_betpot_button_name[i]);
		if (EvaluateRegion(button_name, &result))	{
      p_casino_interface->_technical_betpot_buttons[i].SetState(result);
		}
	}
}

void CScraper::ScrapeSeatedActive() {
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
    p_table_state->Player(i)->set_active(false);
    // Me must NOT set_seated(false) here,
    // as that would reset all player data.
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20567
		ScrapeSeated(i);
		if (p_table_state->Player(i)->seated()) {
			ScrapeActive(i);
		}
	}
}

void CScraper::ScrapeBetsAndBalances() {
	for (int i=0; i<p_tablemap->nchairs(); i++)
	{
		// We have to scrape "every" player,
    //   * as people might bet-fold-standup.
    //   * as people might be missing in tournament, but we use ICM
		// Improvement: 
		//   * scrape everybody up to my first action (then we know who was dealt)
		//   * after that we scrape only dealt players
		//   * and also players who have cards (fresh sitdown and hand-reset, former playersdealt is wrong)
		if ((!p_engine_container->symbol_engine_history()->DidActThisHand())
			|| IsBitSet(p_engine_container->symbol_engine_active_dealt_playing()->playersdealtbits(), i)
      || p_table_state->Player(i)->HasAnyCards())
		{
			ScrapeBet(i);
			ScrapeBalance(i);
		}
	}
}

void CScraper::ScrapeSeated(int chair) {
	CString seated;
	CString result;
  // Me must NOT set_seated(false) here,
  // as that would reset all player data.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=156&t=20567
	seated.Format("p%dseated", chair);
	if (EvaluateRegion(seated, &result)) {
		if (result != "")	{
			p_table_state->Player(chair)->set_seated(p_string_match->IsStringSeated(result));
		}
	}
	if (p_table_state->Player(chair)->seated()) {
		return;
	}
	// try u region next uXseated,
	// but only if we didn't get a positive result from the p region
	seated.Format("u%dseated", chair);
	if (EvaluateRegion(seated, &result)) {
		if (result != "")	{
			p_table_state->Player(chair)->set_seated(p_string_match->IsStringSeated(result));
      return;
		}
	}
  // Failed. Not seated
  p_table_state->Player(chair)->set_seated(false);
}

void CScraper::ScrapeDealer() {
	// The dealer might sit in any chair, even empty ones in some cases
	// That's why we scrape all chairs
	CString dealer;
	CString result;
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		p_table_state->Player(i)->set_dealer(false);
	}
	for (int i=0; i<p_tablemap->nchairs(); i++)	{
		dealer.Format("p%ddealer", i);
		if (EvaluateRegion(dealer, &result)) {
			if (p_string_match->IsStringDealer(result))	{
				p_table_state->Player(i)->set_dealer(true);
				return;
			}
		}
		// Now search for uXdealer
		dealer.Format("u%ddealer", i);
		if (EvaluateRegion(dealer, &result)) {
			if (p_string_match->IsStringDealer(result))	{
				p_table_state->Player(i)->set_dealer(true);
				return;
			}
		}
	}
}

void CScraper::ScrapeActive(int chair) {
	CString active;
	CString result;
	p_table_state->Player(chair)->set_active(false);
  // try p region first pXactive
	active.Format("p%dactive", chair);
	if (EvaluateRegion(active, &result)) {
		p_table_state->Player(chair)->set_active(p_string_match->IsStringActive(result));
	}
	if (p_table_state->Player(chair)->active()) {
		return;
	}
	active.Format("u%dactive", chair);
	if (EvaluateRegion(active, &result)) {
		p_table_state->Player(chair)->set_active(p_string_match->IsStringActive(result));
	}
}

void CScraper::ScrapeColourCodes() {
  CString result;
  CString region;
  for (int i=0; i<p_tablemap->nchairs(); i++) {
    region.Format("p%icolourcode", i);
    if (EvaluateRegion(region, &result)) {
      p_table_state->Player(i)->set_colourcode(atoi(result));
    } else {
      p_table_state->Player(i)->set_colourcode(kUndefinedZero);
    }
  }
}

void CScraper::ScrapeSlider() {
	__HDC_HEADER
	RMapCI handleCI, slider;
	RMapI handleI;
	CString text;
	POINT handle_xy;
  // find handle
	handleCI = p_tablemap->r$()->find("i3handle");
	slider = p_tablemap->r$()->find("i3slider");
  if (handleCI!=p_tablemap->r$()->end() 
      && slider!=p_tablemap->r$()->end() 
      && p_casino_interface->BetsizeConfirmationButton()->IsClickable())	{
		int j = slider->second.right - handleCI->second.left;
		text = "";
    p_casino_interface->_allin_slider.ResetHandlePosition();
		for (int k=0; k<=j; ++k) {
			handleI = p_tablemap->set_r$()->find("i3handle");
			handleI->second.left  += k;
			handleI->second.right += k;
      EvaluateRegion("i3handle", &text);
			handleI->second.left  -= k;
			handleI->second.right -= k;
			if (text == "handle" || text == "true") {
				handleCI = p_tablemap->r$()->find("i3handle");
				handle_xy.x = handleCI->second.left + k;
				handle_xy.y = handleCI->second.top;
        p_casino_interface->_allin_slider.SetHandlePosition(handle_xy);
				write_log(Preferences()->debug_scraper(), "[CScraper] i3handle, result %d,%d\n", handle_xy.x, handle_xy.y);
        __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
				return;
			}
		}
		write_log(Preferences()->debug_scraper(), "[CScraper] i3handle, cannot find handle in the slider region...\n");
	}
  __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

int CScraper::CardString2CardNumber(CString card) {
	int result;
	if (StdDeck_stringToCard((char*) card.GetString(), &result)) {
    AssertRange(result, 0, 255);
	  return result;
  } else {
    return CARD_UNDEFINED;
  }
}

int CScraper::ScrapeCardface(CString base_name) {
  CString card_str;
  // Here name = base_name
	if (EvaluateRegion(base_name, &card_str)) {
		if (card_str != "") {
			return CardString2CardNumber(card_str);
		}
	}
  return CARD_UNDEFINED;
}

int CScraper::ScrapeCardByRankAndSuit(CString base_name) {
  CString rank = base_name + "rank";
	CString suit = base_name + "suit";
	CString rank_result, suit_result;
	// Scrape suit first (usually very fast colour-transform)
	if (EvaluateRegion(suit, &suit_result))	{
		// If a suit could not be recognized we don't need to scrape the rank at all
		// which is often an expensive fuzzy font in this case.
		if (IsSuitString(suit_result)) {
			EvaluateRegion(rank, &rank_result);
			if (IsRankString(rank_result))
			{
        if (rank_result == "10") {
          rank_result = "T";
        }
				CString card_str = rank_result + suit_result;
				return CardString2CardNumber(card_str);
			}
		}
	}
  return CARD_UNDEFINED;
}

int CScraper::ScrapeCardback(CString base_name) {
  if (base_name[0] == 'p')	{
	  CString cardback = base_name.Left(2) + "cardback";
	  CString cardback_result;
	  if (EvaluateRegion(cardback, &cardback_result)) {
	    if ((cardback_result == "cardback")
	        || (cardback_result == "true")) {
		    return CARD_BACK;
	    }
	  }
  }
  return CARD_UNDEFINED;
}

int CScraper::ScrapeNoCard(CString base_name){
  CString card_no_card = base_name + "nocard";
  CString no_card_result;
  if (EvaluateRegion(card_no_card, &no_card_result) 
		  && (no_card_result == "true"))	{
    write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeNoCard(%s) -> true\n",
      card_no_card);
    return CARD_NOCARD;
  }
  write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeNoCard(%s) -> false\n",
      card_no_card);
  return CARD_UNDEFINED;
}

// Cares about "everything"
//   * cardfaces
//   * ranks and suits
//   * cardbacks
int CScraper::ScrapeCard(CString name) {
  int result = CARD_UNDEFINED;
  if (p_tablemap->cardscrapemethod() == 1) {
    // Some casinos display additional cardbacks, 
    // even if a player has card-faces
    // For these casinos we have to scrape the faces first
    // http://www.maxinmontreal.com/forums/viewtopic.php?f=111&t=18539
    // This order of scraping (faces, backs, nocard)
    // always works, but has suboptimal performance
    result = ScrapeCardface(name);
    if (result != CARD_UNDEFINED) {
	  return result;
	}
	// Nextz: try to scrape suits and ranks individually
    result = ScrapeCardByRankAndSuit(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // First: in case of player cards try to scrape card-backs
  // This has to be the very first one,
  // because some casinos use different locations for cardbacks and cards
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=17960
  result = ScrapeCardback(name);
  if (result == CARD_BACK) {
    return CARD_BACK;
  }
  // Then try to scrape "no card"
  result = ScrapeNoCard(name);
  if (result == CARD_NOCARD) {
    return CARD_NOCARD;
  }
  if (p_tablemap->cardscrapemethod() != 1) {
    // If not already done so scrape card-faces
    // This order of scraping (backs, nocard, faces)
    // works for most casinos and has a very good performance
    result = ScrapeCardface(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
	// Again: try to scrape suits and ranks individually
    result = ScrapeCardByRankAndSuit(name);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // Otherwise: in case of playercards try to scrape uXcardfaceY
  CString uname = name;
  if (name[0] == 'p')	{
    uname.SetAt(0, 'u');
	result = ScrapeCardface(uname);
    if (result != CARD_UNDEFINED) {
      return result;
    }
  }
  // Nothing found
  write_log(k_always_log_errors, 
    "[CScraper] WARNING ScrapeCard(%s) found nothing\n", name);
  write_log(k_always_log_errors, 
    "[CScraper] Not nocard, no cards and no cardbacks.\n");
  write_log(k_always_log_errors,
    "[CScraper] Defaulting to nocard\n");
  write_log(k_always_log_errors,
     "[CScraper] Please revisit your cards, especially nocard-regions.\n");
  // For some time we tried to be smart and returned
  //   * CARD_BACK for players
  //   * CARD_NOCARD for board-cards
  // in case of scraping-errors, as card-backs are more easy 
  // to get wrong than nocard (usually a simple colour-transform 
  // with background-colour and negative radius), 
  // but it looks as if this "smart" error-handling 
  // was the reason for wrong deal-positions for some beginners 
  // with bad tablemaps.
  // So we are back to simplicity.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=120&t=19109
  return CARD_NOCARD;
}

void CScraper::ScrapePlayerCards(int chair) {
	CString card_name;
	int card = CARD_UNDEFINED;
  int number_of_cards_to_be_scraped = kNumberOfCardsPerPlayerHoldEm;
  if (p_tablemap->SupportsOmaha()) {
    number_of_cards_to_be_scraped = kNumberOfCardsPerPlayerOmaha;
  }
  CString s_chair = to_string(chair).c_str();
  RMapCI		r_iter = p_tablemap->r$()->find("area_player_cards" + s_chair);
  if (r_iter != p_tablemap->r$()->end()) {
	  vector<CString> result = GetDetectTemplatesResult(r_iter->second.name);
	  int i = 0;
	  for (auto & element : result) {
		  int card = CardString2CardNumber(element);
		  p_table_state->Player(chair)->hole_cards(i)->SetValue(card);
		  i++;
	  }
  }
  else {
	  for (int i = 0; i < number_of_cards_to_be_scraped; i++) {
		  card_name.Format("p%dcardface%d", chair, i);
		  if ((i > 0)
			  && ((card == CARD_UNDEFINED) || (card == CARD_BACK) || (card == CARD_NOCARD))) {
			  // Stop scraping if we find missing cards or cardbacks
		  }
		  else {
			  card = ScrapeCard(card_name);
		  }
		  p_table_state->Player(chair)->hole_cards(i)->SetValue(card);
	  }
  }
  p_table_state->Player(chair)->CheckPlayerCardsForConsistency();
}

void CScraper::ScrapeCommonCards() {
	RMapCI		r_iter = p_tablemap->r$()->find("area_common_cards");
	if (r_iter != p_tablemap->r$()->end()) {
		vector<CString> result = GetDetectTemplatesResult(r_iter->second.name);
		int i = 0;
		for (auto & element : result) {
			int card = CardString2CardNumber(element);
			p_table_state->CommonCards(i)->SetValue(card);
			i++;
		}
	}
	else {
		CString card_name;
		for (int i = 0; i < kNumberOfCommunityCards; i++) {
			card_name.Format("c0cardface%d", i);
			int card = ScrapeCard(card_name);
			p_table_state->CommonCards(i)->SetValue(card);
		}
	}
}

vector<CString> CScraper::GetDetectTemplatesResult(CString area_name) {
	//  Detect template
	HDC					hdc = GetDC(p_autoconnector->attached_hwnd());
	HDC					hdcScreen, hdcCompat, hdc_bitmap_orig, hdc_bitmap_transform_ocr;
	CString				text, selected_transform, separation;
	HBITMAP				old_bitmap_orig, old_bitmap_transform, bitmap_transform_ocr;
	RMapCI				r_iter = p_tablemap->r$()->find(area_name.GetString());
	TPLMapCI			sel_template = p_tablemap->tpl$()->end();

	// Exit because the area doesn't exist
	if (r_iter == p_tablemap->r$()->end())
		return{ "" };


	// Bitblt the attached windows bitmap into a HDC
	hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	hdcCompat = CreateCompatibleDC(hdcScreen);
	RECT rect;
	GetClientRect(p_autoconnector->attached_hwnd(), &rect);
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
	//SaveHBITMAPToFile(bitmap_transform_ocr, "output.bmp");

	//imshow("Area Image", area_mat);
	//waitKey();

	int					x, y, width, height, match_mode;
	HBITMAP				bitmap_image, old_bitmap_image;
	BYTE				*pBits, alpha, red, green, blue;
	RECT roi, zero = RECT{ 0 };
	vector<pair<int, CString>> listROI;

	// Get search templates in area
	for (sel_template = p_tablemap->tpl$()->begin(); sel_template != p_tablemap->tpl$()->end(); sel_template++) {
		if (area_name == "area_common_cards" && sel_template->second.name.Find("card_common_") == -1)
			continue;
		if (area_name.Find("area_player_cards") != -1 && sel_template->second.name.Find("card_player_") == -1)
			continue;
		// Get template size
		width = sel_template->second.width;
		height = sel_template->second.height;
		match_mode = sel_template->second.match_mode;

		// Copy saved template into a memory dc so we can get the bmi
		hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

		// Create new memory DC and new bitmap
		//hdc_image = CreateCompatibleDC(hdcScreen);
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
		::SetDIBits(hdc_bitmap_orig, bitmap_image, 0, height, pBits, &bmi, DIB_RGB_COLORS);

		Mat template_mat(height, width, CV_8UC4);
		template_mat.data = pBits;
		//imshow("Template Image", template_mat);
		//waitKey();

		roi = detectTemplate(area_mat, template_mat, match_mode);
		if (!EqualRect(&roi, &zero)) {
			if (area_name == "area_common_cards") {
				CString value = sel_template->second.name;
				value.Replace("card_common_", "");
				pair<int, CString> matchPair(roi.left, value);
				listROI.push_back(matchPair);
			}
			if (area_name.Find("area_player_cards") != -1) {
				CString value = sel_template->second.name;
				value.Replace("card_player_", "");
				pair<int, CString> matchPair(roi.left, value);
				listROI.push_back(matchPair);
			}
		}
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
	SelectObject(hdc_bitmap_transform_ocr, old_bitmap_transform);
	DeleteObject(bitmap_transform_ocr);
	DeleteDC(hdc_bitmap_transform_ocr);

	SelectObject(hdc_bitmap_orig, old_bitmap_orig);
	DeleteDC(hdc_bitmap_orig);

	DeleteDC(hdcScreen);

	return result;
}
	
// returns true if common cards are in the middle of an animation
bool CScraper::IsCommonAnimation(void) {
	int	flop_card_count = 0;

	// Count all the flop cards
	for (int i=0; i<kNumberOfFlopCards; i++) {
    if (p_table_state->CommonCards(i)->IsKnownCard()) {
			flop_card_count++;
		}
	}

	// If there are some flop cards present,
	// but not all 3 then there is an animation going on
	if (flop_card_count > 0 && flop_card_count < kNumberOfFlopCards) {
		return true;
	}
	// If the turn card is present,
	// but not all 3 flop cards are present then there is an animation going on
	else if (p_table_state->TurnCard()->IsKnownCard()
      && flop_card_count != kNumberOfFlopCards) {
		return true;
	}
	// If the river card is present,
	// but the turn card isn't
	// OR not all 3 flop cards are present then there is an animation going on
	else if (p_table_state->RiverCard()->IsKnownCard() 
      && (!p_table_state->TurnCard()->IsKnownCard() || flop_card_count != kNumberOfFlopCards)) {
		return true;
	}
	return false;
}

void CScraper::ClearAllPlayerNames() {
	for (int i=0; i<kMaxNumberOfPlayers; i++) {
    p_table_state->Player(i)->set_name("");
	}
}

void CScraper::ScrapeName(int chair) {
	RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())

	CString				result;
	CString				s = "";

	// Player name uXname
	s.Format("u%dname", chair);
	EvaluateRegion(s, &result);	
	write_log(Preferences()->debug_scraper(), "[CScraper] u%dname, result %s\n", chair, result.GetString());
	if (result != "")	{
    p_table_state->Player(chair)->set_name(result);
		return;
	}
	// Player name pXname
	s.Format("p%dname", chair);
	EvaluateRegion(s, &result);
	write_log(Preferences()->debug_scraper(), "[CScraper] p%dname, result %s\n", chair, result.GetString());
	if (result != "") {
		p_table_state->Player(chair)->set_name(result);
    return;
	}
}

CString CScraper::CheckEnteredBetsize() {
	CString				result;
	CString				s = "i3edit";

	force_auto_ocr = true;
	EvaluateRegion(s, &result);
	force_auto_ocr = false;
	if (result != "") {
		return result.GetString();
	}
	return"";
}

CString CScraper::ScrapeUPBalance(int chair, char scrape_u_else_p) {
  CString	name;
  CString text;
  assert((scrape_u_else_p == 'u') || (scrape_u_else_p == 'p'));
  name.Format("%c%dbalance", scrape_u_else_p, chair);
  if (EvaluateRegion(name, &text)) {
		if (p_string_match->IsStringAllin(text)) { 
      write_log(Preferences()->debug_scraper(), "[CScraper] %s, result ALLIN", name);
       return Number2CString(0.0);
		}	else if (	text.MakeLower().Find("out")!=-1
				||	text.MakeLower().Find("inactive")!=-1
				||	text.MakeLower().Find("away")!=-1 ) {
			p_table_state->Player(chair)->set_active(false);
			write_log(Preferences()->debug_scraper(), "[CScraper] %s, result OUT/INACTIVE/AWAY\n", name);
      return Number2CString(kUndefinedZero);
		}	else {
      return text;
		}
	}
  // Number2CString(kUndefined) returns "-1",
  // which probably got converted to 1 by StringToMoney.
  // That's why we return an empty string "" again
  // to support all the people who don't scrape "nothing" as 0 or allin.
  // http://www.maxinmontreal.com/forums/viewtopic.php?f=124&t=20277
  return "";
}

void CScraper::ScrapeBalance(int chair) {
	RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())
  // Scrape uXbalance and pXbalance
  CString balance = ScrapeUPBalance(chair, 'p');
  if (p_table_state->Player(chair)->_balance.SetValue(balance)) {
    return;
  }
  balance = ScrapeUPBalance(chair, 'u');
  if (p_table_state->Player(chair)->_balance.SetValue(balance)) {
    return;
  }
}

void CScraper::ScrapeBet(int chair) {
  RETURN_IF_OUT_OF_RANGE (chair, p_tablemap->LastChair())

	__HDC_HEADER;
	CString				text = "";
	CString				s = "", t="";

	p_table_state->Player(chair)->_bet.Reset();
  // Player bet pXbet
  s.Format("p%dbet", chair);
  CString result;
  EvaluateRegion(s, &result);
	if (p_table_state->Player(chair)->_bet.SetValue(result)) {
		__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return;
	}
  // uXbet
	s.Format("u%dbet", chair);
  result = "";
  EvaluateRegion(s, &result);
  if (p_table_state->Player(chair)->_bet.SetValue(result)) {
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
      return;
  }
	// pXchip00
	s.Format("p%dchip00", chair);
	RMapCI r_iter = p_tablemap->r$()->find(s.GetString());
	if (r_iter != p_tablemap->r$()->end() && p_table_state->Player(chair)->_bet.GetValue() == 0) 	{
		old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
		double chipscrape_res = DoChipScrape(r_iter);
		SelectObject(hdcCompatible, old_bitmap);

		t.Format("%.2f", chipscrape_res);
		p_table_state->Player(chair)->_bet.SetValue(t.GetString());
		write_log(Preferences()->debug_scraper(), "[CScraper] p%dchipXY, result %f\n", 
      chair, p_table_state->Player(chair)->_bet.GetValue());
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

void CScraper::ScrapeAllPlayerCards() {
	for (int i=0; i<kMaxNumberOfPlayers; i++){
		for (int j=0; j<NumberOfCardsPerPlayer(); j++) {
			p_table_state->Player(i)->hole_cards(j)->ClearValue();
		}
	}
	write_log(Preferences()->debug_scraper(), "[CScraper] ScrapeAllPlayerCards()\n");
	for (int i=0; i<p_tablemap->nchairs(); i++) {
		write_log(Preferences()->debug_scraper(), "[CScraper] Calling ScrapePlayerCards, chair %d.\n", i);
		ScrapePlayerCards(i);
	}
}

void CScraper::ScrapePots() {
	__HDC_HEADER
	CString			text = "";
	CTransform	trans;
	CString			s = "", t="";
	RMapCI			r_iter = p_tablemap->r$()->end();

  p_table_state->ResetPots();
	for (int j=0; j<kMaxNumberOfPots; j++) {
		// r$c0potX
		s.Format("c0pot%d", j);
    CString result;
    EvaluateRegion(s, &result);
    if (p_table_state->set_pot(j, result)) {
      continue;
    }
		// r$c0potXchip00_index
		s.Format("c0pot%dchip00", j);
		r_iter = p_tablemap->r$()->find(s.GetString());
		if (r_iter != p_tablemap->r$()->end() && p_table_state->Pot(j) == 0) {
			ProcessRegion(r_iter);
			//old_bitmap = (HBITMAP) SelectObject(hdcCompatible, r_iter->second.cur_bmp);
			//trans.DoTransform(r_iter, hdcCompatible, &text);
			//SelectObject(hdcCompatible, old_bitmap);
      old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
			double chipscrape_res = DoChipScrape(r_iter);
			SelectObject(hdcCompatible, old_bitmap);
			t.Format("%.2f", chipscrape_res);
			p_table_state->set_pot(j, t.GetString());
			write_log(Preferences()->debug_scraper(), 
        "[CScraper] c0pot%dchipXY, result %f\n", j, p_table_state->Pot(j));

			// update the bitmap for second chip position in the first stack
			s.Format("c0pot%dchip01", j);
			r_iter = p_tablemap->r$()->find(s.GetString());
			if (r_iter != p_tablemap->r$()->end()) {
				ProcessRegion(r_iter);
      }
			// update the bitmap for first chip position in the second stack
			s.Format("c0pot%dchip10", j);
			r_iter = p_tablemap->r$()->find(s.GetString());
			if (r_iter != p_tablemap->r$()->end())
				ProcessRegion(r_iter);
		}
	}
	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
}

void CScraper::ScrapeMTTRegions() {
  assert(p_engine_container->symbol_engine_mtt_info() != NULL);
	CString result;
	if (EvaluateRegion("mtt_number_entrants", &result)) {	
		p_engine_container->symbol_engine_mtt_info()->set_mtt_number_entrants(result);
	}
	if (EvaluateRegion("mtt_players_remaining", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_players_remaining(result);
	}
	if (EvaluateRegion("mtt_my_rank", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_my_rank(result);
	}
	if (EvaluateRegion("mtt_paid_places", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_paid_places(result);
	}
	if (EvaluateRegion("mtt_largest_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_largest_stack(result);
	}
	if (EvaluateRegion("mtt_average_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_average_stack(result);
	}
	if (EvaluateRegion("mtt_smallest_stack", &result)) {
		p_engine_container->symbol_engine_mtt_info()->set_mtt_smallest_stack(result);
	}
}

void CScraper::ScrapeLimits() {
  assert(p_title_evaluator != NULL);
  p_title_evaluator->ClearAllDataOncePerHeartbeat();
  p_title_evaluator->EvaluateScrapedHandNumbers();
  p_title_evaluator->EvaluateTitleText();
  p_title_evaluator->EvaluateScrapedTitleTexts();
  p_title_evaluator->EvaluateScrapedGameInfo(); 
}

BOOL CScraper::SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName)
{
	HDC hDC;
	int iBits;
	WORD wBitCount;
	DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
	BITMAP Bitmap0;
	BITMAPFILEHEADER bmfHdr;
	BITMAPINFOHEADER bi;
	LPBITMAPINFOHEADER lpbi;
	HANDLE fh, hDib, hPal, hOldPal2 = NULL;
	hDC = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL);
	iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
	DeleteDC(hDC);
	if (iBits <= 1)
		wBitCount = 1;
	else if (iBits <= 4)
		wBitCount = 4;
	else if (iBits <= 8)
		wBitCount = 8;
	else
		wBitCount = 24;
	GetObject(hBitmap, sizeof(Bitmap0), (LPSTR)&Bitmap0);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = Bitmap0.bmWidth;
	bi.biHeight = -Bitmap0.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = wBitCount;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrImportant = 0;
	bi.biClrUsed = 256;
	dwBmBitsSize = ((Bitmap0.bmWidth * wBitCount + 31) & ~31) / 8
		* Bitmap0.bmHeight;
	hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
	lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	hPal = GetStockObject(DEFAULT_PALETTE);
	if (hPal)
	{
		hDC = GetDC(NULL);
		hOldPal2 = SelectPalette(hDC, (HPALETTE)hPal, FALSE);
		RealizePalette(hDC);
	}


	GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap0.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
		+ dwPaletteSize, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);

	if (hOldPal2)
	{
		SelectPalette(hDC, (HPALETTE)hOldPal2, TRUE);
		RealizePalette(hDC);
		ReleaseDC(NULL, hDC);
	}

	fh = CreateFile(lpszFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (fh == INVALID_HANDLE_VALUE)
		return FALSE;

	bmfHdr.bfType = 0x4D42; // "BM"
	dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
	bmfHdr.bfSize = dwDIBSize;
	bmfHdr.bfReserved1 = 0;
	bmfHdr.bfReserved2 = 0;
	bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;

	WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);

	WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
	GlobalUnlock(hDib);
	GlobalFree(hDib);
	CloseHandle(fh);
	return TRUE;
}

void CScraper::CreateBitmaps(void) {
	HDC				hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);

	// Whole window
	RECT			cr = {0};
	GetClientRect(p_autoconnector->attached_hwnd(), &cr);
	_entire_window_last = CreateCompatibleBitmap(hdcScreen, cr.right, cr.bottom);
	_entire_window_cur = CreateCompatibleBitmap(hdcScreen, cr.right, cr.bottom);

	// r$regions
	for (RMapI r_iter=p_tablemap->set_r$()->begin(); r_iter!=p_tablemap->set_r$()->end(); r_iter++)
	{
		int w = r_iter->second.right - r_iter->second.left + 1;
		int h = r_iter->second.bottom - r_iter->second.top + 1;
		if (r_iter->second.transform[0] != 'A') {
			r_iter->second.last_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
			r_iter->second.cur_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
		}
		else {
			w = w + 6;
			h = h + 6;
			r_iter->second.last_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
			r_iter->second.cur_bmp = CreateCompatibleBitmap(hdcScreen, w, h);
		}
	}

	DeleteDC(hdcScreen);
}

void CScraper::DeleteBitmaps(void) {
	// Whole window
	DeleteObject(_entire_window_last);
  delete_entire_window_cur();

	// Common cards
	for (RMapI r_iter=p_tablemap->set_r$()->begin(); r_iter!=p_tablemap->set_r$()->end(); r_iter++)
	{
		DeleteObject(r_iter->second.last_bmp); r_iter->second.last_bmp=NULL;
		DeleteObject(r_iter->second.cur_bmp); r_iter->second.cur_bmp=NULL;
	}
}

// This is the chip scrape routine
const double CScraper::DoChipScrape(RMapCI r_iter) {
	HDC				hdc = GetDC(p_autoconnector->attached_hwnd());

	int				j = 0, stackindex = 0, chipindex = 0;
	int				hash_type = 0, pixcount = 0, chipwidth = 0, chipheight = 0;
	int				top = 0, bottom = 0, left = 0, right = 0;
	bool			stop_loop = false;
	uint32_t		*uresult = NULL, hash = 0, pix[MAX_HASH_WIDTH*MAX_HASH_HEIGHT] = {0};
	double			result = 0;
	CString			resstring = "";

	CString			type = "";
	int				vertcount = 0, horizcount = 0;
	RMapCI			r_start = p_tablemap->r$()->end();
	RMapCI			r_vert[10];
	RMapCI			r_horiz[10];
	CString			s = "";

	// Initialize arrays
	for (int j=0; j<10; j++)
	{
		r_vert[j] = p_tablemap->r$()->end();
		r_horiz[j] = p_tablemap->r$()->end();
	}

	// Check for bad parameters
	if (r_iter == p_tablemap->r$()->end())
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	// figure out if we are dealing with a pot or playerbet here
	if (r_iter->second.name.Mid(0,5)=="c0pot" && r_iter->second.name.Mid(6,4)=="chip")
		type = r_iter->second.name.Mid(0,10);

	else if (r_iter->second.name.Mid(0,1)=="p" && r_iter->second.name.Mid(2,4)=="chip")
		type = r_iter->second.name.Mid(0,6);

	else
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	// find start, vert stride, and horiz stride regions
	s.Format("%s00", type.GetString());
	r_start = p_tablemap->r$()->find(s.GetString());
	if (r_start == p_tablemap->r$()->end())
	{
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		return 0.;
	}

	for (int j = 1; j<=9; j++)
	{
		s.Format("%s0%d", type.GetString(), j);
		r_vert[j] = p_tablemap->r$()->find(s.GetString());
		if (r_vert[j] != p_tablemap->r$()->end())
			vertcount++;

		s.Format("%s%d0", type.GetString(), j);
		r_horiz[j] = p_tablemap->r$()->find(s.GetString());
		if (r_horiz[j] != p_tablemap->r$()->end())
			horizcount++;
	}

	hash_type = RightDigitCharacterToNumber(r_start->second.transform);

	// Bitblt the attached windows bitmap into a HDC
	HDC hdcScreen = CreateDC("DISPLAY", NULL, NULL, NULL);
	HDC hdcCompat = CreateCompatibleDC(hdcScreen);
	RECT rect;
	GetClientRect(p_autoconnector->attached_hwnd(), &rect);
	HBITMAP attached_bitmap = CreateCompatibleBitmap(hdcScreen, rect.right, rect.bottom);
	HBITMAP	old_bitmap = (HBITMAP) SelectObject(hdcCompat, attached_bitmap);
	BitBlt(hdcCompat, 0, 0, rect.right, rect.bottom, hdc, 0, 0, SRCCOPY);
	
	// Get chipscrapemethod option from tablemap, if specified
	CString res = p_tablemap->chipscrapemethod();
	CString cs_method = res.MakeLower();
	int cs_method_x = 0, cs_method_y = 0;
	if (cs_method!="" && cs_method!="all" && cs_method.Find("x")!=-1)
	{
		cs_method_x = strtoul(cs_method.Left(cs_method.Find("x")), NULL, 10);
		cs_method_y = strtoul(cs_method.Mid(cs_method.Find("x")+1), NULL, 10);
	}

	stop_loop = false;
	// loop through horizontal stacks
	for (stackindex=0; stackindex<MAX_CHIP_STACKS && !stop_loop; stackindex++)
	{
		// loop through vertical chips
		for (chipindex=0; chipindex<MAX_CHIPS_PER_STACK && !stop_loop; chipindex++)
		{

			// figure out left, right, top, bottom of next chip to be scraped
			if (vertcount==1)
			{
				top = r_start->second.top + chipindex*(r_vert[1]->second.top - r_start->second.top);
				bottom = r_start->second.bottom + chipindex*(r_vert[1]->second.bottom - r_start->second.bottom);
			}
			else
			{
				if (r_vert[chipindex+1] == p_tablemap->r$()->end())
				{
					stop_loop = true;
				}
				else
				{
					top = r_vert[chipindex+1]->second.top;
					bottom = r_vert[chipindex+1]->second.bottom;
				}
			}

			if (horizcount==1)
			{
				left = r_start->second.left + stackindex*(r_horiz[1]->second.left - r_start->second.left);
				right = r_start->second.right + stackindex*(r_horiz[1]->second.right - r_start->second.right);
			}
			else
			{
				if (r_horiz[stackindex+1] == p_tablemap->r$()->end())
				{
					stop_loop = true;
				}
				else
				{
					left = r_horiz[stackindex+1]->second.left;
					right = r_horiz[stackindex+1]->second.right;
				}
			}

			if (!stop_loop)
			{
				chipwidth = right - left + 1;
				chipheight = bottom - top + 1;

				// calculate hash
				if (hash_type>=1 && hash_type<=3)
				{
					pixcount = 0;
					for (PMapCI p_iter=p_tablemap->p$(hash_type)->begin(); p_iter!=p_tablemap->p$(hash_type)->end(); p_iter++)
					{
							int x = p_iter->second.x;
							int y = p_iter->second.y;

							if (x<chipwidth && y<chipheight)
								pix[pixcount++] = GetPixel(hdcCompat, left + x, top + y);
					}

					if (hash_type==1) hash = hashword(&pix[0], pixcount, HASH_SEED_1);
					else if (hash_type==2) hash = hashword(&pix[0], pixcount, HASH_SEED_2);
					else if (hash_type==3) hash = hashword(&pix[0], pixcount, HASH_SEED_3);
				}

				// lookup hash in h$ records
				HMapCI h_iter = p_tablemap->h$(hash_type)->find(hash);

				// no hash match
				if (h_iter == p_tablemap->h$(hash_type)->end())
				{
					// See if we should stop horiz or vert loops on a non-match
					if (cs_method == "")
					{
						// Stop horizontal scrape loop if chipindex==0 AND a non-match
						if (chipindex==0)
							stackindex = MAX_CHIP_STACKS+1;

						// stop vertical scrape loop on a non-match
						chipindex = MAX_CHIPS_PER_STACK+1;
					}
				}

				// hash match found
				else
				{
					resstring = h_iter->second.name;
					resstring.Remove(',');
					resstring.Remove('$');
					result += atof(resstring.GetString());
				}
			}

			// See if we should stop chip loop due to chipscrapemethod
			if (cs_method!="" && cs_method!="all" && chipindex>=cs_method_y)
				chipindex = MAX_CHIPS_PER_STACK+1;
		}

		// See if we should stop stack loop due to chipscrapemethod
		if (cs_method!="" && cs_method!="all" && stackindex>=cs_method_x)
			stackindex = MAX_CHIP_STACKS+1;

	}

	SelectObject(hdcCompat, old_bitmap);
	DeleteObject(attached_bitmap);
	DeleteDC(hdcCompat);
	DeleteDC(hdcScreen);

	ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
	return result;
}

bool CScraper::IsExtendedNumberic(CString text) {
  bool currently_unused = false;
  assert(currently_unused);
  return false;
}

bool CScraper::IsIdenticalScrape() {
  __HDC_HEADER

	// Get bitmap of whole window
	RECT		cr = {0};
	GetClientRect(p_autoconnector->attached_hwnd(), &cr);

	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_cur);
	BitBlt(hdcCompatible, 0, 0, cr.right, cr.bottom, hdc, cr.left, cr.top, SRCCOPY);
	SelectObject(hdcCompatible, old_bitmap);

  p_table_state->TableTitle()->UpdateTitle();
	
	// If the bitmaps are the same, then return now
	// !! How often does this happen?
	// !! How costly is the comparison?
	if (BitmapsAreEqual(_entire_window_last, _entire_window_cur) 
      && !p_table_state->TableTitle()->TitleChangedSinceLastHeartbeat()) 	{
		DeleteDC(hdcCompatible);
		DeleteDC(hdcScreen);
		ReleaseDC(p_autoconnector->attached_hwnd(), hdc);
		write_log(Preferences()->debug_scraper(), "[CScraper] IsIdenticalScrape() true\n");
    __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
		return true;
	}
	// Copy into "last" bitmap
	old_bitmap = (HBITMAP) SelectObject(hdcCompatible, _entire_window_last);
	BitBlt(hdcCompatible, 0, 0, cr.right-cr.left+1, cr.bottom-cr.top+1, hdc, cr.left, cr.top, SRCCOPY);
	SelectObject(hdc, old_bitmap);

	__HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK
	write_log(Preferences()->debug_scraper(), "[CScraper] IsIdenticalScrape() false\n");
	return false;
}

#undef __HDC_HEADER 
#undef __HDC_FOOTER_ATTENTION_HAS_TO_BE_CALLED_ON_EVERY_FUNCTION_EXIT_OTHERWISE_MEMORY_LEAK