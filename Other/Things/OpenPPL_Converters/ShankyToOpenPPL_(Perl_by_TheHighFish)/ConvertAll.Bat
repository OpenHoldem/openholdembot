REM ##################################################
REM
REM   This file is part of the OpenHoldem project
REM     Download page:         http://code.google.com/p/openholdembot/
REM     Forums:                http://www.maxinmontreal.com/forums/index.php
REM     Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
REM
REM ##################################################
REM
REM   Converts all profiles in this directory at once
REM   from Shanky-PPL to OpenPPL
REM   Expected input: *.txt files
REM   Output: *.ohf files
REM
REM ##################################################

del *.ohf
For %%F In (*.txt) Do ( 
    Perl Convert.pl %%F > Temp.ohf
    @REM rename Temp.ohf to ohf file with previous naming
    ren temp.ohf %%~nF.ohf
    @REM a pause to check for copyright warnings
    @REM Pause
)
Del Temp.ohf