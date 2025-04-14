//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: automatically connecting to unserved poker-tables,
//   using shared memory and a mutex to synchronize with other instaces.
//
//******************************************************************************

#ifndef INC_CAUTOCONNECTOR_H
#define INC_CAUTOCONNECTOR_H

#include "..\CTablemap\CTablemap.h"
#include "CSpaceOptimizedGlobalObject.h"

class CAutoConnector: public CSpaceOptimizedGlobalObject {
 public:
	CAutoConnector();
	~CAutoConnector();
 public:
	bool Connect(HWND targetHWnd);
	void Disconnect(CString reason_for_disconnection);
	double SecondsSinceLastFailedAttemptToConnect(); 
 public:
  bool IsConnectedToAnything();
  bool IsConnectedToExistingWindow();
  bool IsConnectedToGoneWindow();
 public:
	// public accessors
	const HWND attached_hwnd()    { return _attached_hwnd; }
 private:
	int SelectTableMapAndWindowAutomatically();
	void WriteLogTableReset(CString event_and_reason);
 private:
	void Check_TM_Against_All_Windows_Or_TargetHWND(int tablemap_index, HWND targetHWnd);
  void CheckIfWindowMatchesMoreThanOneTablemap(HWND hwnd);
 private:
  void set_attached_hwnd(const HWND table);
 private:
	void FailedToConnectBecauseNoWindowInList();
	void FailedToConnectProbablyBecauseAllTablesAlreadyServed();
	void GoIntoPopupBlockingMode();
 private:
	// private variables - use public accessors and public mutators to address these
	HWND     _attached_hwnd; // Table that we are attached to
	CCritSec m_critsec;
 private:
	// Mutex used for cross-instance autoconnector coordination
	CMutex *_autoconnector_mutex;

private:
	void StartCaptureSession(HWND hwnd);

public:
	// Create Direct 3D Device
	winrt::com_ptr<ID3D11Device> d3d_device;
	winrt::Windows::Graphics::DirectX::Direct3D11::IDirect3DDevice device;
	ID3D11DeviceContext* d3d_context = nullptr;
	winrt::com_ptr<IDXGIAdapter> adapter;
	winrt::com_ptr<IDXGIFactory2> factory;
	winrt::impl::com_ref<IGraphicsCaptureItemInterop> interop_factory = winrt::get_activation_factory<
		winrt::Windows::Graphics::Capture::GraphicsCaptureItem>().as<IGraphicsCaptureItemInterop>();
	winrt::Windows::Graphics::Capture::GraphicsCaptureItem capture_item = { nullptr };
	winrt::com_ptr<ID3D11Texture2D> texture;
	GraphicsCaptureSession session = { nullptr };
	winrt::Windows::Graphics::Capture::Direct3D11CaptureFramePool m_frame_pool = { nullptr };
};

extern  CAutoConnector *p_autoconnector;

#endif INC_CAUTOCONNECTOR_H