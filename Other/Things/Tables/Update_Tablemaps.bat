@echo.
@echo This software is part of the Openholdem project
@echo Download page:         https://github.com/OpenHoldem/openholdembot/releases
@echo Forums:                http://www.maxinmontreal.com/forums/index.php
@echo Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
@echo.
@echo This tool will download the "offical" tablemaps 
@echo from the OpenHoldem-repository at GitHub.
@echo It will also download a replay-frame per map
@echo for verification / as setup-instruction.
@echo.
@REM testing if SVN.exe exists
@REM https://stackoverflow.com/questions/4002819/finding-the-path-of-the-program-that-will-execute-from-the-command-line-in-windo
@where /Q SVN.exe
@REM Checking the error-level of the last command
@REM http://steve-jansen.github.io/guides/windows-batch-scripting/part-3-return-codes.html
@IF %ERRORLEVEL% NEQ 0 (
   @echo Required software: TortoiseSVN
   @echo https://tortoisesvn.net/downloads.html
   @echo You need to install its command-line tools.
   @echo.
   @pause
   @exit
)
@REM Check if working-directory == OpenHoldem directory
@REM smart people are able to run this script from the wtong directory
@REM and wonder why they don't get see any tablemaps
@REM http://www.maxinmontreal.com/forums/viewtopic.php?f=117&t=21580
@IF NOT EXIST "Update_Tablemaps.bat" (
   @echo ERROR
   @echo Working directory seems to be not an OpenHoldem directory.
   @echo Don't know where to save the tablemaps.
   @echo Please simply double-click the script or run it from the OpenHoldem folder.
   @echo.
   @pause
   @exit
)
@REM Using the %ProgramFiles% variable
@REM https://stackoverflow.com/questions/19652678/how-to-mention-c-program-files-in-batchfile
@REM Getting a remote directory from GitHub with SVN
@REM https://stackoverflow.com/questions/7106012/download-a-single-folder-or-directory-from-a-github-repo
"%ProgramFiles%\TortoiseSVN\bin\svn.exe" --force export "https://github.com/OpenHoldem/openholdembot/trunk/##_Tablemaps_##/scraper"
@echo.
@pause