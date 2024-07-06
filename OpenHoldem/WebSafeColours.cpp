//*******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose: calculating the closest web/safe colour (216 values)
//   for a RGB-colour (16777216 values) to avoid problems with
//   fuzzyness when scraping colour-codes.
//   Also 216 colours are enough and makes it possible to introduce 
//   named colour constants.
//
//*******************************************************************************

#include "StdAfx.h"
#include "WebSafeColours.h"
#include "Debugger/debugger.h"

int ClosestWebSafeColourComponent(const int red_or_green_or_blue_component) {
  assert(red_or_green_or_blue_component >= 0);
  assert(red_or_green_or_blue_component <= 0xFF);
  if (red_or_green_or_blue_component < 0x1A) {
    return 0x00;
  } else if (red_or_green_or_blue_component < 0x4D) {
    return 0x33;
  } else if (red_or_green_or_blue_component < 0x80) {
    return 0x66;
  } else if (red_or_green_or_blue_component < 0xA3) {
    return 0x99;
  } else if (red_or_green_or_blue_component < 0xD6) {
    return 0xcc;
  } else {
    return 0xFF;
  }
}

int ClosestWebSafeColour(const int argb_colour) {
  // Web-safe colours use only 6 values for R / G / B
  // 0x00, 0x33, 0x66, 0x99, 0xCC and 0xFF,
  // resulting in 6**3 216 different colours.
  // That's what we need for colour-codes,
  // because 24- or 32-bit-colours would be too much
  // and could be affected by fuzzyness.
  int red   = (argb_colour >> 16) & 0xFF;
  int green = (argb_colour >>  8) & 0xFF;
  int blue  = argb_colour & 0xFF;
  int new_red   = ClosestWebSafeColourComponent(red);
  int new_green = ClosestWebSafeColourComponent(green);
  int new_blue  = ClosestWebSafeColourComponent(blue);
  int closest_web_safe_colour = RGB(new_red, new_green, new_blue);
  return closest_web_safe_colour;
}