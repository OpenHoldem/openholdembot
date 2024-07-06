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

#include "stdafx.h"
#include "CAllinSlider.h"

#include "CAutoConnector.h"
#include "CAutoplayerTrace.h"
#include "CBetsizeInputBox.h"
#include "CCasinoInterface.h"

#include "CTablemap/CTablemap.h"
#include "OpenHoldem.h"

CAllinSlider::CAllinSlider() {
  ResetHandlePosition();
  // Not really (0, 0), but (-1, -1), out of the screen
  POINT	point_null = { kUndefined, kUndefined };
}

CAllinSlider::~CAllinSlider() {
}

void CAllinSlider::SetHandlePosition(const POINT position) {
  _position.x = position.x;
  _position.y = position.y;
}

void CAllinSlider::ResetHandlePosition() {
  _position.x = kUndefined;
  _position.y = kUndefined;
}

bool CAllinSlider::SlideAllin() {
  if (!SlideAllinPossible()) {
   write_log(Preferences()->debug_autoplayer(), "[AllinSlider] ...ending DoSlider early (i3handle or i3slider are not defined in the tablemap)\n");
    return false;
  }
 write_log(Preferences()->debug_autoplayer(), "[AllinSlider] Starting DoSlider...\n");
  if ((_position.x == kUndefined) || (_position.y == kUndefined)) {
   write_log(Preferences()->debug_autoplayer(), "[AllinSlider] ...ending DoSlider early (handle not found - i3handle must use a transform that resolves to either 'handle' or 'true')\n");
    return false;
  }
  // Click and drag handle
  RECT drag_region;
  GetSliderRegions();
  drag_region.left = _position.x + ((_i3_handle.right - _i3_handle.left) / 2);
  drag_region.top = _position.y + ((_i3_handle.bottom - _i3_handle.top) / 2);
  drag_region.right = _position.x + (_i3_slider.right - _i3_slider.left);
  drag_region.bottom = drag_region.top;

  write_log(Preferences()->debug_autoplayer(), "[AllinSlider] Slider : Calling mouse.dll to jam from %d,%d to %d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
  // Not really (0, 0), but (-1, -1), out of the screen
  POINT	point_null = { kUndefined, kUndefined };
  (theApp._dll_mouse_click_drag) (p_autoconnector->attached_hwnd(), drag_region);

  write_log(Preferences()->debug_autoplayer(), "[AllinSlider] Sleeping %d ms\n.", Preferences()->swag_delay_3());
  Sleep(Preferences()->swag_delay_3());

  // Click confirmation button 
  p_casino_interface->_betsize_input_box.Confirm();
  write_log(Preferences()->debug_autoplayer(), "[AllinSlider] Jam complete: %d,%d,%d,%d\n", drag_region.left, drag_region.top, drag_region.right, drag_region.bottom);
  write_log(Preferences()->debug_autoplayer(), "[AllinSlider] ...ending DoSlider.\n");
  return true;
}

bool CAllinSlider::GetSliderRegions() {
  p_tablemap->GetTMRegion("i3slider", &_i3_slider);
  p_tablemap->GetTMRegion("i3handle", &_i3_handle);
  if ((_i3_slider.bottom < 0)
    || (_i3_slider.left < 0)
    || (_i3_slider.right < 0)
    || (_i3_slider.top < 0)) {
    return false;
  }
  if ((_i3_handle.bottom < 0)
    || (_i3_handle.left < 0)
    || (_i3_handle.right < 0)
    || (_i3_handle.top < 0)) {
    return false;
  }
  return true;
}

bool CAllinSlider::SlideAllinPossible() {
  // Required: betsize-confirmation-button, slider and handle
  if (p_tablemap->swagconfirmationmethod() == BETCONF_CLICKBET) {
    if (!p_casino_interface->BetsizeConfirmationButton()->IsClickable()) {
      return false;
    }
  }
  if (!p_tablemap->ItemExists("i3slider")) {
    return false;
  }
  if (!p_tablemap->ItemExists("i3handle")) {
    return false;
  }
  if (!GetSliderRegions()) {
    return false;
  }
  return true;
}