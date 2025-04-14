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

#ifndef INC_CSCRAPER_H
#define INC_CSCRAPER_H

#include <stdint.h>
#include "../CTablemap/CTablemap.h"
#include "CSpaceOptimizedGlobalObject.h"



class CScraper : public CSpaceOptimizedGlobalObject {
  friend class CLazyScraper;
  friend class CAutoConnector;
 public:
	// public functions and accessors
	CScraper(void);
	~CScraper(void);
 public:
  // For replay-frames
	const HBITMAP	entire_window_cur() { return _entire_window_cur; }
 public:
  // For scraping custom regions at the DLL-level
  bool EvaluateRegion(CString name, CString *result);
  void EvaluateTrueFalseRegion(bool *result, const CString name);
 public:
  bool IsCommonAnimation();
 protected:
	void CreateBitmaps(void);
	void DeleteBitmaps(void);
	HBITMAP CaptureWindowToHBITMAP(void);
	bool IsIdenticalScrape();
 protected:
	void ScrapeDealer();
	void ScrapeActionButtons();
	void ScrapeActionButtonLabels();
	void ScrapeInterfaceButtons();
	void ScrapeBetpotButtons();
	void ClearAllPlayerNames();
	void ScrapeName(const int chair);
	void ScrapePlayerCards(int chair);
	void ScrapeSlider();
	void ScrapeCommonCards();
	void ScrapeSeatedActive();
	void ScrapeBetsAndBalances();
	void ScrapeAllPlayerCards();
  void ScrapeColourCodes();
  void ScrapeMTTRegions();
 private:
	void ScrapeSeated(int chair);
	void ScrapeActive(int chair);
 private:
	int ScrapeCard(CString name);
  int ScrapeCardback(CString base_name);
  int ScrapeCardByRankAndSuit(CString base_name);
  int ScrapeCardface(CString base_name);
  int ScrapeNoCard(CString base_name);
 private:
	int CardString2CardNumber(CString card);
 private:
	// private functions and variables - not available via accessors or mutators
  CString ScrapeUPBalance(int chair, char scrape_u_else_p);
	void ScrapeBalance(const int chair);
	void ScrapeBet(const int chair);
	void ScrapePots();
	void ScrapeLimits();
	const double DoChipScrape(RMapCI r_iter);
	BOOL SaveHBITMAPToFile(HBITMAP hBitmap, LPCTSTR lpszFileName);
 private:
	bool ProcessRegion(RMapCI r_iter);
	bool IsExtendedNumberic(CString text);
 private:
  void ResetLimitInfo();
 private:
#define ENT CSLock lock(m_critsec);
  void delete_entire_window_cur() { ENT DeleteObject(_entire_window_cur);}
#undef ENT

  bool CopyWindowClientRectToTexture(
	  ID3D11Device* device,
	  ID3D11DeviceContext* context,
	  ID3D11Texture2D* sourceTexture,
	  HWND hwnd,
	  ID3D11Texture2D** outTexture)
  {
	  if (!device || !context || !sourceTexture || !hwnd) return false;

	  // Step 1: Get client rect and convert to screen coordinates
	  RECT clientRect;
	  RECT winRect;
	  if (!GetClientRect(hwnd, &clientRect))
		  return false;
	  if (!GetWindowRect(hwnd, &winRect))
		  return false;

	  POINT topLeft = { clientRect.left, clientRect.top };
	  ClientToScreen(hwnd, &topLeft);

	  int width = clientRect.right - clientRect.left;
	  int height = clientRect.bottom - clientRect.top;

	  // Step 2: Create destination texture
	  D3D11_TEXTURE2D_DESC srcDesc;
	  sourceTexture->GetDesc(&srcDesc);

	  D3D11_TEXTURE2D_DESC dstDesc = srcDesc;
	  dstDesc.Width = width;
	  dstDesc.Height = height;
	  dstDesc.BindFlags = 0;
	  dstDesc.MiscFlags = 0;
	  dstDesc.Usage = D3D11_USAGE_STAGING;
	  dstDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	  ID3D11Texture2D* destTexture = nullptr;
	  HRESULT hr = device->CreateTexture2D(&dstDesc, nullptr, &destTexture);
	  if (FAILED(hr)) return false;

	  // Step 3: Copy region from source to destination
	  D3D11_BOX srcBox;
	  srcBox.left = 0;         // adjust if sourceTexture isn't full screen
	  srcBox.top = topLeft.y - winRect.top;
	  srcBox.front = 0;
	  srcBox.right = srcBox.left + width;
	  srcBox.bottom = srcBox.top + height;
	  srcBox.back = 1;

	  context->CopySubresourceRegion(destTexture, 0, 0, 0, 0, sourceTexture, 0, &srcBox);

	  *outTexture = destTexture; // return the new texture
	  return true;
  }

 private:
	// private variables - use public accessors and public mutators to address these
  CCritSec		m_critsec;
  // Counter of GDI objects (potential memorz leak)
  // Should be 0 at end of program -- will be checked.
  int         _leaking_GDI_objects;
  // Used for potential optimizations
  int total_region_counter;
  int identical_region_counter;
	HBITMAP			_entire_window_last;
  HBITMAP			_entire_window_cur;
};

extern CScraper *p_scraper;

#endif // INC_CSCRAPER_H


