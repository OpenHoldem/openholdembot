;==============================================================
;
; UniversalRebuyScript
; ====================
;
; PURPOSE:  Very basic "rebuy-script" for OpenHoldem.
;   It does nothing, but show the command-line-parameters.
;
;==============================================================

#include "Array.au3"

; Require variables to be declared.
AutoItSetOption("MustDeclareVars", 1)

; Initialize $CommandLine
Dim $CommandLine[9][2] = [["Number of parameters", 0], _
	["Casino", "Undefined"], ["Windowhandle", 0], ["Userchair", 0], _
	["Balance", 0], ["Smallblind", 0], ["Bigblind", 0], _
	["Bigbet", 0], ["Target-amount", 0]]
For $i=0 To $CmdLine[0]
	$CommandLine[$i][1] = $CmdLine[$i]
Next

; Show array "CommandLine"
_ArrayDisplay($CommandLine)

MsgBox(0x40, "Rebuy.exe", "Demo - nothing to do.", 10)
