//*******************************************************************************
//
// This file is part of the OpenHoldem project
//   Download page:         http://code.google.com/p/openholdembot/
//   Forums:                http://www.maxinmontreal.com/forums/index.php
//   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//*******************************************************************************
//
// Purpose:
//
//*******************************************************************************

#include "pch.h"
#include "CLineCounter.h"

CLineCounter line_counter;

CLineCounter::CLineCounter() {
  ResetOnNewSourceFile();
}

CLineCounter::~CLineCounter() {
}

void CLineCounter::ResetOnNewSourceFile() {
  _absolute_line = 1;
  _starting_line_of_function = 1;
  _starting_line_of_next_function = 1;
  _relative_line = 1;
}

void CLineCounter::OnLineBreak() {
  ++_absolute_line;
  ++_relative_line;
}

void CLineCounter::OnNextFunction() {
  _starting_line_of_function = /*_starting_line_of_next_function;
  _starting_line_of_next_function =*/ _absolute_line;
  //assert(_starting_line_of_function < _starting_line_of_next_function);
  _relative_line = 1;

}
