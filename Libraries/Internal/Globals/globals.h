//******************************************************************************
//
// This file is part of the OpenHoldem project
//    Source code:           https://github.com/OpenHoldem/openholdembot/
//    Forums:                http://www.maxinmontreal.com/forums/index.php
//    Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
//
//******************************************************************************
//
// Purpose: Managing access to global data,
//   avoiding circular dependency of DLLs to OpenHoldem.
//   Temp solution until we get rid of all global data,
//   once mpdulatization has been improved
//
//******************************************************************************

#pragma once

class CPreferences;
void InitGlobals();
CPreferences* Preferences();
