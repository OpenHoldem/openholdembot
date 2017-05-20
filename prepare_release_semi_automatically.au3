#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.12.0
 Author:         TheHighFish

 Script Function:
	Template AutoIt script.

#ce ----------------------------------------------------------------------------

Local $pre_created_release_dir = "##_OpenHoldem_Release_Directory_##"
Local $new_openholdem_dir      = "OpenHoldem_11.x.y"
Local $new_bot_logic_dir       = $new_openholdem_dir & "\bot_logic"
Local $new_vmware_keyboard_dir = $new_openholdem_dir & "\Keyboard_DLL_VmWare_Unity_Mode"
Local $new_tools_dir           = $new_openholdem_dir & "\tools"
Local $binary_dir              = "Release"
Local $binary_optimized_dir    = "Release - Optimized"
Local $openppl_dir             = "OpenPPL"
Local $openppl_library_dir     = $openppl_dir & "\OpenPPL_Library"
Local $openppl_manual_dir      = $openppl_dir & "\OpenPPL_Manual"
Local $openholdem_manual_dir   = "Documentation\OpenHoldem_Manual_ready_for_release"
Local $pokertracker_docu_dir   = "Documentation\Chapters\symbols"
Local $release_notes           = $pre_created_release_dir & "\documents\OpenHoldem Release Notes.txt"

; Script Start - Add your code below here
MsgBox(0, "Next Step", "Have a look at http://code.google.com/p/openholdembot/source/list and ask the developers, if all work is completed and remind them to update the release-notes")
; Open release-notes for editing
ShellExecute($release_notes)
Sleep(1000)
MsgBox(0, "Next Step", "Update the version and release-date in the release-notes ")
MsgBox(0, "Next Step", "Change the version of OpenHoldem. (Search the OpenHoldem-project for the old one, e.g. ""4.2.5"" and ""4.25"", but don't auto-replace) OpenHoldem\stdaxafx.h OpenHoldem\OpenHoldem.rc")
MsgBox(0, "Next Step", "Choose the correct build option. Usually optimized for OH, but debug if there are some known problems left. Release for the rest (as end-users usually miss debug-DLLs)." )
MsgBox(0, "Next Step", "Rebuild everything")
MsgBox(0, "Next Step", "Test ""everything"", at least briefly that OH ""works"".")
MsgBox(0, "Next Step", "Push everything to GitHub")
MsgBox(0, "Next Step", "Tag the release on GitHub. Comment: ""Tagging OpenHoldem 7.x.y for release"".")

DirRemove($new_openholdem_dir, 1)
DirCopy($pre_created_release_dir, $new_openholdem_dir)
; Copy OpenHoldem to the new main directory plus their LIBs to support linking
CopyNeededFile($binary_optimized_dir, $new_openholdem_dir, "OpenHoldem.exe")
CopyNeededFile($binary_optimized_dir, $new_openholdem_dir, "OpenHoldem.lib")
; Copy tools like ManualMode to the tools-directory
CopyNeededFile($binary_dir, $new_tools_dir, "ManualMode.exe")
CopyNeededFile($binary_dir, $new_tools_dir, "OHReplay.exe")
CopyNeededFile($binary_dir, $new_tools_dir, "OpenReplayShooter.exe")
CopyNeededFile($binary_dir, $new_tools_dir, "OpenScrape.exe")
; Copy DLLs to the new directory  plus their LIBs to support linking
CopyNeededFile($binary_dir, $new_openholdem_dir, "keyboard.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "Reference Keyboard DLL.lib")
CopyNeededFile($binary_dir, $new_openholdem_dir, "mouse.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "Reference Mouse DLL.lib")
CopyNeededFile($binary_dir, $new_openholdem_dir, "pokertracker_query_definitions.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "PokerTracker_Query_Definitions.lib")
CopyNeededFile($binary_dir, $new_openholdem_dir, "user.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "Reference User DLL.lib")
CopyNeededFile($binary_dir, $new_openholdem_dir, "string_functions.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "string_functions.lib")
CopyNeededFile($binary_dir, $new_openholdem_dir, "window_functions.dll")
CopyNeededFile($binary_dir, $new_openholdem_dir, "window_functions.lib")
; Copy window_functions.lib to the tools-directory too,
; because ManualMode needs it
; string_functions too for OpenScrape
CopyNeededFile($binary_dir, $new_tools_dir, "string_functions.dll")
CopyNeededFile($binary_dir, $new_tools_dir, "string_functions.lib")
CopyNeededFile($binary_dir, $new_tools_dir, "window_functions.dll")
CopyNeededFile($binary_dir, $new_tools_dir, "window_functions.lib")
; Add Keyboard_DLL_VmWare_Unity_Mode into separate directory
; CopyNeededFile($binary_dir, $new_vmware_keyboard_dir, "Keyboard_DLL_VmWare_Unity_Mode.dll")
; CopyNeededFile($binary_dir, $new_vmware_keyboard_dir, "Keyboard_DLL_VmWare_Unity_Mode.lib")
; Remove the *.exp and unnecessary *.pdb files.
FileDelete($new_openholdem_dir & "\*.exp")
FileDelete($new_openholdem_dir & "\*.pdb")
; Add the OpenPPL-library
CopyNeededFile($openppl_library_dir, $new_bot_logic_dir, "*.ohf")
; Add OpenPPL_Manual.chm and symbol_pokertracker.pdf
CopyNeededFile($openppl_manual_dir, $new_openholdem_dir, "OpenPPL_Manual.chm")
CopyNeededFile($openholdem_manual_dir, $new_openholdem_dir, "OpenHoldem_Manual.chm")
CopyNeededFile($pokertracker_docu_dir, $new_openholdem_dir, "symbols_pokertracker.pdf")
; Remove replay-direcoty (if existent), logs and other private data
DirRemove($new_openholdem_dir & "\Replay")
FileDelete($new_openholdem_dir & "\logs\*.*")
; Move everything to the userts desktop
DirMove($new_openholdem_dir, @DesktopDir & "\" & $new_openholdem_dir)
; Release it
MsgBox(0, "Next Step", "The new direcory is at your desktop. Please create a rar-archive")
MsgBox(0, "Next Step", "Upload the rar-file to https://sites.google.com/site/openholdempokerbot/downloads")
; Open release-notes for announcement
ShellExecute($release_notes)
Sleep(1000)
MsgBox(0, "Next Step", "Announce the new download in ""OpenHoldem Stickies"" and post the release-notes.")

Func CopyNeededFile($source_dir, $destination_dir, $name)
   Local $source = $source_dir & "\" & $name
   Local $destination = $destination_dir & "\" & $name
   If Not FileCopy($source, $destination) Then
	  MsgBox(0, "Error", "Missing file: " & $source)
	  Exit(-1)
   EndIf
EndFunc


