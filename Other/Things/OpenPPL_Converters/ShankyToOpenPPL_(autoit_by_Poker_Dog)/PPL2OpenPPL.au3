#cs ----------------------------------------------------------------------------

 AutoIt Version: 3.3.6.1
 Author:         poker_dog
 Requires:       symbols.txt

 Script Function:
	This script will take the contents of a PPL text profile and convert it to OpenPPL for use with OpenHoldem v7.5 and greater.
	The file sysmbols.txt is required which contains all the Hold'em PPL symbols and their OpenPPL counterpart.  It needs to be
	kept in the same folder as this script when it is run.
	
	When you run the script, you will be asked for the PPL profile that you wish to convert.  After selecting the file the conversion
	begins.  A new file is created with the .oppl extension as well as a log file showing all changes that were made by presenting
	the original code on one line and the modified code directly beneath it for easy comparison.  Note - the source PPL file is never
	altered.  However, the converted file will overwrite any existing file with the same name as will the change log.
	
	You will notice in many of the routines that very similar values are searched for and compared.  This is my attempt to account for
	various	developer inconsistencies such as extra spacings and bad formatting that could be found in any random PPL profile.
	
	I am sure some things will get missed and you will ultimately have an error when you open the converted file in	OpenHoldem but
	I am confident the number of errors will be fewer than if you did this conversion process manually.
	
	I hope that you find this utility useful.

#ce ----------------------------------------------------------------------------

HotKeySet("{ESC}", "Terminate")

Dim $version = "1.21"

Dim $PPL = ""
Dim $OpenPPL = ""

Dim $symbols[200]
Dim $source[20000]
Dim $original[20000]
Dim $NumberOfLines = 0
Dim $NumberOfSymbols = 0

Dim $RaiseMethod = "RaiseBy " ; "RaiseTo "
Dim $UseApproxEqualOperator = True ; False
Dim $FixPercentages = False ; True
Dim $FixPercentagesOnly = False ; True

$PPL = FileOpenDialog("Select the source PPL file", "C:\", "(*.txt)", 1)

LoadSymbols()
LoadSourceFile()

Main()

Exit

Func Main()
	
	$start = TimerInit()
	
	ProgressOn("Performing PPL to OpenPPL Conversion", "", "", 68, 75, "")
	
	If $FixPercentagesOnly = False Then
		CreateConsistencies()
		RemoveOptionSettings()
		FixFunctionHeaders()
		RemoveDelays()
		RemoveSitout()
		RemoveBeep()
		FixBets()
		FixRaises()
		If $UseApproxEqualOperator = True Then FixIntegers()
		FixSymbols()
		FixHandExpressions()
		FixBoardExpressions()
		StandardizeKeywords()
		FixUserVariables()
		AddForce()
		If $FixPercentages = True Then FixPercentages()
		CleanUp()
	Else
		FixPercentages()
	EndIf
	
	WriteConvertedFile()
	
	$elapsed = TimerDiff($start)
	
	WriteChangeLog()
	
	ProgressOff()
		
	MsgBox(0, "Finished", "Conversion complete." & @CRLF & @CRLF & "Lines: " & $NumberOfLines & "     Time: " & Round($elapsed/100000*100,5) & " seconds" & @CRLF & @CRLF & "Saved as: " & $OpenPPL & "     ")
	
EndFunc ; ==> Main()

Func CreateConsistencies()
	
	; We create some consistencies centered around spacing which will make subsequent operations more streamlined
	; Once the conversion is complete we will revert some of these changes
	
	; remove all extra spaces (any sequence of two or more spaces)
	
	UpdateProgress(5,"Create consistencies...")
	
	While 1
		$MadeChanges = False
		For $i = 0 to $NumberOfLines Step + 1
			If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "  ") Then
				$source[$i] = StringReplace($source[$i], "  ", " ", 0)
				$MadeChanges = True
			EndIf
		Next
		If $MadeChanges = False Then ExitLoop
	WEnd
		
	; make all ' = ' consistant by removing leading spaces
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " =") Then
			$source[$i] = StringReplace($source[$i], " =", "=", 0)
		EndIf
	Next
	
	; make all ' = ' consistant by removing trailing spaces
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "= ") Then
			$source[$i] = StringReplace($source[$i], "= ", "=", 0)
		EndIf
	Next
	
	; remove any space before '%'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " %") Then
			$source[$i] = StringReplace($source[$i], " %", "%", 0)
		EndIf
	Next
	
	; add a space after all '('
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "(") Then
			$source[$i] = StringReplace($source[$i], "(", "( ", 0)
		EndIf
	Next
	
	
	; add a space before all ')'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], ")") Then
			$source[$i] = StringReplace($source[$i], ")", " )", 0)
		EndIf
	Next
	
	; look for missing spaces in common situations
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "suitedor") Then
			$source[$i] = StringReplace($source[$i], "suitedor", "suited or", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "nothave") Then
			$source[$i] = StringReplace($source[$i], "nothave", "not have", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " not(") Then
			$source[$i] = StringReplace($source[$i], " not(", " not (", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " and(") Then
			$source[$i] = StringReplace($source[$i], " and(", " and (", 0)
		EndIf
	Next
	
	; look for split card ranks and remove the space
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "hand=2 ") Then
				$source[$i] = StringReplace($source[$i], "hand=2 ", "hand=2", 0)
			EndIf
			If StringInStr($source[$i], "hand=3 ") Then
				$source[$i] = StringReplace($source[$i], "hand=3 ", "hand=3", 0)
			EndIf
			If StringInStr($source[$i], "hand=4 ") Then
				$source[$i] = StringReplace($source[$i], "hand=4 ", "hand=4", 0)
			EndIf
			If StringInStr($source[$i], "hand=5 ") Then
				$source[$i] = StringReplace($source[$i], "hand=5 ", "hand=5", 0)
			EndIf
			If StringInStr($source[$i], "hand=6 ") Then
				$source[$i] = StringReplace($source[$i], "hand=6 ", "hand=6", 0)
			EndIf
			If StringInStr($source[$i], "hand=7 ") Then
				$source[$i] = StringReplace($source[$i], "hand=7 ", "hand=7", 0)
			EndIf
			If StringInStr($source[$i], "hand=8 ") Then
				$source[$i] = StringReplace($source[$i], "hand=8 ", "hand=8", 0)
			EndIf
			If StringInStr($source[$i], "hand=9 ") Then
				$source[$i] = StringReplace($source[$i], "hand=9 ", "hand=9", 0)
			EndIf
			If StringInStr($source[$i], "hand=T ") Then
				$source[$i] = StringReplace($source[$i], "hand=T ", "hand=T", 0)
			EndIf
			If StringInStr($source[$i], "hand=J ") Then
				$source[$i] = StringReplace($source[$i], "hand=J ", "hand=J", 0)
			EndIf
			If StringInStr($source[$i], "hand=Q ") Then
				$source[$i] = StringReplace($source[$i], "hand=Q ", "hand=Q", 0)
			EndIf
			If StringInStr($source[$i], "hand=K ") Then
				$source[$i] = StringReplace($source[$i], "hand=K ", "hand=K", 0)
			EndIf
			If StringInStr($source[$i], "hand=A ") Then
				$source[$i] = StringReplace($source[$i], "hand=A ", "hand=A", 0)
			EndIf
		EndIf
	Next
	
	; fix any single rank 'and' that just had the space removed like hand = A and is now hand = Aand
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "hand=2and") Then
				$source[$i] = StringReplace($source[$i], "hand=2and", "hand=2 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=3and") Then
				$source[$i] = StringReplace($source[$i], "hand=3and", "hand=3 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=4and") Then
				$source[$i] = StringReplace($source[$i], "hand=4and", "hand=4 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=5and") Then
				$source[$i] = StringReplace($source[$i], "hand=5and", "hand=5 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=6and") Then
				$source[$i] = StringReplace($source[$i], "hand=6and", "hand=6 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=7and") Then
				$source[$i] = StringReplace($source[$i], "hand=7and", "hand=7 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=8and") Then
				$source[$i] = StringReplace($source[$i], "hand=8and", "hand=8 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=9and") Then
				$source[$i] = StringReplace($source[$i], "hand=9and", "hand=9 and", 0)
			EndIf
			If StringInStr($source[$i], "hand=Tand") Then
				$source[$i] = StringReplace($source[$i], "hand=Tand", "hand=T and", 0)
			EndIf
			If StringInStr($source[$i], "hand=Jand") Then
				$source[$i] = StringReplace($source[$i], "hand=Jand", "hand=J and", 0)
			EndIf
			If StringInStr($source[$i], "hand=Qand") Then
				$source[$i] = StringReplace($source[$i], "hand=Qand", "hand=Q and", 0)
			EndIf
			If StringInStr($source[$i], "hand=Kand") Then
				$source[$i] = StringReplace($source[$i], "hand=Kand", "hand=K and", 0)
			EndIf
			If StringInStr($source[$i], "hand=Aand") Then
				$source[$i] = StringReplace($source[$i], "hand=Aand", "hand=A and", 0)
			EndIf
		EndIf
	Next
	
	; fix any single rank 'or' that just had the space removed like hand = A or is now hand = Aor
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "hand=2or") Then
				$source[$i] = StringReplace($source[$i], "hand=2or", "hand=2 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=3or") Then
				$source[$i] = StringReplace($source[$i], "hand=3or", "hand=3 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=4or") Then
				$source[$i] = StringReplace($source[$i], "hand=4or", "hand=4 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=5or") Then
				$source[$i] = StringReplace($source[$i], "hand=5or", "hand=5 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=6or") Then
				$source[$i] = StringReplace($source[$i], "hand=6or", "hand=6 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=7or") Then
				$source[$i] = StringReplace($source[$i], "hand=7or", "hand=7 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=8or") Then
				$source[$i] = StringReplace($source[$i], "hand=8or", "hand=8 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=9or") Then
				$source[$i] = StringReplace($source[$i], "hand=9or", "hand=9 or", 0)
			EndIf
			If StringInStr($source[$i], "hand=Tor") Then
				$source[$i] = StringReplace($source[$i], "hand=Tor", "hand=T or", 0)
			EndIf
			If StringInStr($source[$i], "hand=Jor") Then
				$source[$i] = StringReplace($source[$i], "hand=Jor", "hand=J or", 0)
			EndIf
			If StringInStr($source[$i], "hand=Qor") Then
				$source[$i] = StringReplace($source[$i], "hand=Qor", "hand=Q or", 0)
			EndIf
			If StringInStr($source[$i], "hand=Kor") Then
				$source[$i] = StringReplace($source[$i], "hand=Kor", "hand=K or", 0)
			EndIf
			If StringInStr($source[$i], "hand=Aor") Then
				$source[$i] = StringReplace($source[$i], "hand=Aor", "hand=A or", 0)
			EndIf
		EndIf
	Next
	
	; look for split card ranks on the board and remove the space
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "board=2 2") Then
				$source[$i] = StringReplace($source[$i], "board=2 2", "board=22", 0)
			EndIf
			If StringInStr($source[$i], "board=3 3") Then
				$source[$i] = StringReplace($source[$i], "board=3 3", "board=33", 0)
			EndIf
			If StringInStr($source[$i], "board=4 4") Then
				$source[$i] = StringReplace($source[$i], "board=4 4", "board=44", 0)
			EndIf
			If StringInStr($source[$i], "board=5 5") Then
				$source[$i] = StringReplace($source[$i], "board=5 5", "board=55", 0)
			EndIf
			If StringInStr($source[$i], "board=6 6") Then
				$source[$i] = StringReplace($source[$i], "board=6 6", "board=66", 0)
			EndIf
			If StringInStr($source[$i], "board=7 7") Then
				$source[$i] = StringReplace($source[$i], "board=7 7", "board=77", 0)
			EndIf
			If StringInStr($source[$i], "board=8 8") Then
				$source[$i] = StringReplace($source[$i], "board=8 8", "board=88", 0)
			EndIf
			If StringInStr($source[$i], "board=9 9") Then
				$source[$i] = StringReplace($source[$i], "board=9 9", "board=99", 0)
			EndIf
			If StringInStr($source[$i], "board=T T") Then
				$source[$i] = StringReplace($source[$i], "board=T T", "board=TT", 0)
			EndIf
			If StringInStr($source[$i], "board=J J") Then
				$source[$i] = StringReplace($source[$i], "board=J J", "board=JJ", 0)
			EndIf
			If StringInStr($source[$i], "board=Q Q") Then
				$source[$i] = StringReplace($source[$i], "board=Q Q", "board=QQ", 0)
			EndIf
			If StringInStr($source[$i], "board=K K") Then
				$source[$i] = StringReplace($source[$i], "board=K K", "board=KK", 0)
			EndIf
			If StringInStr($source[$i], "board=A A") Then
				$source[$i] = StringReplace($source[$i], "board=A A", "board=AA", 0)
			EndIf
			If StringInStr($source[$i], "board=A K Q") Then
				$source[$i] = StringReplace($source[$i], "board=A K Q", "board=AKQ", 0)
			EndIf
			If StringInStr($source[$i], "board=A K J") Then
				$source[$i] = StringReplace($source[$i], "board=A K J", "board=AKJ", 0)
			EndIf
			If StringInStr($source[$i], "board=A K T") Then
				$source[$i] = StringReplace($source[$i], "board=A K T", "board=AKT", 0)
			EndIf
			If StringInStr($source[$i], "board=A Q J") Then
				$source[$i] = StringReplace($source[$i], "board=A Q J", "board=AQJ", 0)
			EndIf
			If StringInStr($source[$i], "board=A Q T") Then
				$source[$i] = StringReplace($source[$i], "board=A Q T", "board=AQT", 0)
			EndIf
			If StringInStr($source[$i], "board=A J T") Then
				$source[$i] = StringReplace($source[$i], "board=A J T", "board=AJT", 0)
			EndIf
			If StringInStr($source[$i], "board=K Q J") Then
				$source[$i] = StringReplace($source[$i], "board=K Q J", "board=KQJ", 0)
			EndIf
			If StringInStr($source[$i], "board=K Q T") Then
				$source[$i] = StringReplace($source[$i], "board=K Q T", "board=KQT", 0)
			EndIf
			If StringInStr($source[$i], "board=K Q 9") Then
				$source[$i] = StringReplace($source[$i], "board=K Q 9", "board=KQ9", 0)
			EndIf
			If StringInStr($source[$i], "board=K J T") Then
				$source[$i] = StringReplace($source[$i], "board=K J T", "board=KJT", 0)
			EndIf
			If StringInStr($source[$i], "board=K J 9") Then
				$source[$i] = StringReplace($source[$i], "board=K J 9", "board=KJ9", 0)
			EndIf
			If StringInStr($source[$i], "board=Q J T") Then
				$source[$i] = StringReplace($source[$i], "board=Q J T", "board=QJT", 0)
			EndIf
			If StringInStr($source[$i], "board=Q J 9") Then
				$source[$i] = StringReplace($source[$i], "board=Q J 9", "board=QJ9", 0)
			EndIf
			If StringInStr($source[$i], "board=Q J 8") Then
				$source[$i] = StringReplace($source[$i], "board=Q J 8", "board=QJ8", 0)
			EndIf
			If StringInStr($source[$i], "board=Q T 9") Then
				$source[$i] = StringReplace($source[$i], "board=Q T 9", "board=QT9", 0)
			EndIf
			If StringInStr($source[$i], "board=J T 9") Then
				$source[$i] = StringReplace($source[$i], "board=J T 9", "board=JT9", 0)
			EndIf
			If StringInStr($source[$i], "board=T 9 8") Then
				$source[$i] = StringReplace($source[$i], "board=T 9 8", "board=T98", 0)
			EndIf
			If StringInStr($source[$i], "board=9 8 7") Then
				$source[$i] = StringReplace($source[$i], "board=9 8 7", "board=987", 0)
			EndIf
			If StringInStr($source[$i], "board=8 7 6") Then
				$source[$i] = StringReplace($source[$i], "board=8 7 6", "board=876", 0)
			EndIf
			If StringInStr($source[$i], "board=2 3 4") Then
				$source[$i] = StringReplace($source[$i], "board=2 3 4", "board=234", 0)
			EndIf
			If StringInStr($source[$i], "board=3 4 5") Then
				$source[$i] = StringReplace($source[$i], "board=3 4 5", "board=345", 0)
			EndIf
			If StringInStr($source[$i], "board=4 5 6") Then
				$source[$i] = StringReplace($source[$i], "board=4 5 6", "board=456", 0)
			EndIf
			If StringInStr($source[$i], "board=5 6 7") Then
				$source[$i] = StringReplace($source[$i], "board=5 6 7", "board=567", 0)
			EndIf
			If StringInStr($source[$i], "board=A K") Then
				$source[$i] = StringReplace($source[$i], "board=A K", "board=AK", 0)
			EndIf
			If StringInStr($source[$i], "board=A Q") Then
				$source[$i] = StringReplace($source[$i], "board=A Q", "board=AQ", 0)
			EndIf
			If StringInStr($source[$i], "board=A J") Then
				$source[$i] = StringReplace($source[$i], "board=A J", "board=AJ", 0)
			EndIf
			If StringInStr($source[$i], "board=K Q") Then
				$source[$i] = StringReplace($source[$i], "board=K Q", "board=KQ", 0)
			EndIf
			If StringInStr($source[$i], "board=K J") Then
				$source[$i] = StringReplace($source[$i], "board=K J", "board=KJ", 0)
			EndIf
			If StringInStr($source[$i], "board=Q J") Then
				$source[$i] = StringReplace($source[$i], "board=Q J", "board=QJ", 0)
			EndIf
			If StringInStr($source[$i], "board=Q T") Then
				$source[$i] = StringReplace($source[$i], "board=Q T", "board=QT", 0)
			EndIf
		EndIf
	Next
	
	; fix any board 'and' that just had the space removed like board = A and is now board = AAnd
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "board=AAnd") Then
				$source[$i] = StringReplace($source[$i], "board=AAnd", "board=A and", 0)
			EndIf
		EndIf
	Next
	
	; remove all extra spaces (any sequence of two or more spaces)
	While 1
		$MadeChanges = False
		For $i = 0 to $NumberOfLines Step + 1
			If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "  ") Then
				$source[$i] = StringReplace($source[$i], "  ", " ", 0)
				$MadeChanges = True
			EndIf
		Next
		If $MadeChanges = False Then ExitLoop
	WEnd
	
EndFunc ; ==> CreateConsistencies()

Func RemoveOptionSettings()
	
	UpdateProgress(10, "Commenting option settings...")
	
	; PPL option settings are commented out and when the converted file is created they are
	; placed under the ##notes## function where they are safe to delete at the users choosing
	For $i = 0 to $NumberOfLines Step + 1
		If $source[$i] = "preflop" Then ExitLoop
		$source[$i] = "// " & $source[$i]
	Next
	
EndFunc ; ==> RemoveOptionSettings()

Func FixFunctionHeaders()
	
	UpdateProgress(15, "Fixing function headers...")
	
	; PPL betting rounds, preflop, flop, turn, river are converted to OpenHoldem function headings; ##f$<round>##
	For $i = 0 to $NumberOfLines
		If $source[$i] = "preflop" Or $source[$i] = "flop" Or $source[$i] = "turn" Or $source[$i] = "river" Then
			$source[$i] = "##f$" & $source[$i] & "##"
		EndIf
	Next

EndFunc ; ==> ChangeFunctionHeaders()

Func RemoveDelays()
	
	UpdateProgress(20, "Removing delay commands...")
	
	; PPL Delay command is not supported by OpenPPL and gets removed but the rest of the line remains intact
	Local $newstring = ""
	
	For $i = 0 to $NumberOfLines
		If StringInStr($source[$i], "delay") Then
			$temp = StringSplit($source[$i], " ")
			For $j = 1 to $temp[0] - 2
				$newstring = $newstring & " " & $temp[$j]
			Next
			$source[$i] = $newstring
			$newstring = ""
		EndIf
	Next

EndFunc ; ==> RemoveDelays()

Func RemoveSitout()
	
	UpdateProgress(25, "Removing sitout commands...")
	
	; PPL Sitout command is not supported by OpenPPL and thus lines containing this command get
	; commented out but left in for the user to modify or delete if desired
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "sitout") Then
			$source[$i] = "// " & $source[$i]
		EndIf
	Next
	
EndFunc ; ==> ChangeSitout()

Func RemoveBeep()
	
	UpdateProgress(30, "Removing beep commands...")
	
	; PPL Beep command is not supported by OpenPPL and thus lines containing this command get
	; commented out but left in for the user to modify or delete if desired
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "beep") Then
			$source[$i] = "// " & $source[$i]
		EndIf
	Next
	
EndFunc ; ==> ChangeBeep()

Func FixBets()
	
	UpdateProgress(35, "Fixing bet commands...")
	
	; PPL command Bet ## gets replaced by OpenPPL 'RaiseBy' or 'RaiseTo' based on user preference at
	; the top of this file
	For $i = 0 to $NumberOfLines Step + 1
		For $j = 1 To 9 Step + 1
			If StringInStr($source[$i], " bet " & $j) Then
				$source[$i] = StringReplace($source[$i], " Bet " & $j, " " & $RaiseMethod & $j)
			EndIf
		Next
	Next

EndFunc ; ==> FixBets()

Func FixRaises()
	
	UpdateProgress(40, "Fixing raise commands...")
	
	; PPL command Raise gets replaced by OpenPPL 'RaiseBy' or 'RaiseTo' based on user preference at
	; the top of this file
	For $i = 0 to $NumberOfLines Step + 1
		For $j = 1 To 9 Step + 1
			If StringInStr($source[$i], " raise " & $j) Then
				$source[$i] = StringReplace($source[$i], " Raise " & $j, " " & $RaiseMethod & $j)
			EndIf
		Next
	Next

EndFunc ; ==> FixRaises()

Func FixIntegers()
	
	UpdateProgress(45, "Adding approximate-equal commands...")
	
	; PPL uses integers and OpenHoldem uses floating point numbers so it is recommended to use the
	; approximatly-equal symbol for 'BetSize' and 'StackSize' which is selectable at the top of this file
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "betsize=") Then
			$source[$i] = StringReplace($source[$i], "betsize=", "BetSize ~~ ")
		EndIf
	Next
	
	; change 'StackSize =' to 'StackSize ~~'
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "stacksize=") Then
			$source[$i] = StringReplace($source[$i], "stacksize=", "StackSize ~~ ")
		EndIf
	Next
	
EndFunc ; ==> FixIntegers()

Func FixSymbols()
	
	UpdateProgress(60, "Fixing symbols...")
	
	; PPL symbol naming and OpenPPL symbol naming differ (use of numbers, capitalization, etc) so
	; all PPL symbols that have a known OpenPPL counterpart are compared and changes are made where necessary
	
	For $i = 1 to $NumberOfLines ; on every line of code...
		For $j = 1 to $NumberOfSymbols ; go through all the symbols in symbols.txt
			$temp = StringSplit($symbols[$j], ",")
			If StringInStr($source[$i], $temp[1], 0) Then
				$source[$i] = StringReplace($source[$i], $temp[1], $temp[2], 0)
			EndIf
		Next
	Next
	
	; work around for HaveSecondNutFlushDraw always being written HaveSecondNutFlushdraw for no apparent reason
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "flushdraw") Then
			$source[$i] = StringReplace($source[$i], "flushdraw", "FlushDraw", 0)
		EndIf
	Next
	
	; work around for HaveSecondBestKickerOrBetter always being written HaveSecondBestKickerorbetter for no apparent reason
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "orbetter") Then
			$source[$i] = StringReplace($source[$i], "orbetter", "OrBetter", 0)
		EndIf
	Next
	
EndFunc ; ==> FixSymbols()

Func FixHandExpressions()
	
	UpdateProgress(65, "Fixing hand expressions...")
	
	; PPL hand expressions are converted to OpenPPL format
	For $i = 0 to $NumberOfLines Step + 1
		While StringInStr($source[$i], " Hand=")
			$source[$i] = StringReplace($source[$i], " Hand=", " hand$")
		WEnd
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		While StringInStr($source[$i], " suited")
			$source[$i] = StringReplace($source[$i], " suited", "suited")
		WEnd
	Next
	
	; fix any single rank that lost its space like hand$QRaiseBy
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" Then
			If StringInStr($source[$i], "hand$2Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$2Raise", "hand$2 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$3Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$3Raise", "hand$3 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$4Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$4Raise", "hand$4 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$5Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$5Raise", "hand$5 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$6Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$6Raise", "hand$6 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$7Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$7Raise", "hand$7 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$8Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$8Raise", "hand$8 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$9Raise") Then
				$source[$i] = StringReplace($source[$i], "hand$9Raise", "hand$9 Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$TRaise") Then
				$source[$i] = StringReplace($source[$i], "hand$TRaise", "hand$T Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$JRaise") Then
				$source[$i] = StringReplace($source[$i], "hand$JRaise", "hand$J Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$QRaise") Then
				$source[$i] = StringReplace($source[$i], "hand$QRaise", "hand$Q Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$KRaise") Then
				$source[$i] = StringReplace($source[$i], "hand$KRaise", "hand$K Raise", 0)
			EndIf
			If StringInStr($source[$i], "hand$ARaise") Then
				$source[$i] = StringReplace($source[$i], "hand$ARaise", "hand$A Raise", 0)
			EndIf
		EndIf
	Next
	
EndFunc ; ==> ChangeHandExpressions()

Func FixBoardExpressions()
	
	UpdateProgress(70, "Fixing board expressions...")
	
	; PPL board expressions are converted to OpenPPL format
	For $i = 0 to $NumberOfLines Step + 1
		While StringInStr($source[$i], " Board=")
			$source[$i] = StringReplace($source[$i], " Board=", " board$")
		WEnd
	Next
	
EndFunc ; ==> ChangeBoardExpressions()

Func StandardizeKeywords()
	
	UpdateProgress(75, "Standardizing keywords...")
	
	; change all 'when' to 'WHEN'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "when ") Then
			$source[$i] = StringReplace($source[$i], "when ", "WHEN ", 0)
		EndIf
	Next
	
	; change all 'force' to 'FORCE'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " force") Then
			$source[$i] = StringReplace($source[$i], " force", " FORCE", 0)
		EndIf
	Next

	; change all 'and' to 'AND'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " and ") Then
			$source[$i] = StringReplace($source[$i], " and ", " AND ", 0)
		EndIf
	Next
	
	; change all 'or' to 'OR'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " or ") Then
			$source[$i] = StringReplace($source[$i], " or ", " OR ", 0)
		EndIf
	Next
	
	; change all 'not' to 'NOT'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " not ") Then
			$source[$i] = StringReplace($source[$i], " not ", " NOT ", 0)
		EndIf
	Next
	
	; change the action 'fold' to 'Fold'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "fold force") Then
			$source[$i] = StringReplace($source[$i], "fold force", "Fold FORCE", 0)
		EndIf
	Next
	
	; change the action 'call' to 'Call'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "call force") Then
			$source[$i] = StringReplace($source[$i], "call force", "Call FORCE", 0)
		EndIf
	Next
	
	; change the action 'call' to 'Call'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "call ") Then
			$source[$i] = StringReplace($source[$i], "call ", "Call ", 0)
		EndIf
	Next
	
	; change the action 'bet' to 'Bet'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " bet") Then
			$source[$i] = StringReplace($source[$i], " bet", " Bet", 0)
		EndIf
	Next
	
	; change the action 'raisemin' to 'RaiseMin'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "raisemin force") Then
			$source[$i] = StringReplace($source[$i], "raisemin force", "RaiseMin FORCE", 0)
		EndIf
	Next
	
	; change the action 'raisemax' to 'RaiseMax'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "raisemax force") Then
			$source[$i] = StringReplace($source[$i], "raisemax force", "RaiseMax FORCE", 0)
		EndIf
	Next
	
	; change the action 'raisepot' to 'RaisePot'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "raisepot force") Then
			$source[$i] = StringReplace($source[$i], "raisepot force", "RaisePot FORCE", 0)
		EndIf
	Next
	
	; change the action 'check' to 'Check'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "=check") Then
			$source[$i] = StringReplace($source[$i], "=check", "=Check", 0)
		EndIf
	Next
	
	; change the action 'first' to 'First'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "=first") Then
			$source[$i] = StringReplace($source[$i], "=first", "=First", 0)
		EndIf
	Next
	
	; change all 'last' to 'Last'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "=last") Then
			$source[$i] = StringReplace($source[$i], "=last", "=Last", 0)
		EndIf
	Next
	
	; change all 'none' to 'None'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "=none") Then
			$source[$i] = StringReplace($source[$i], "=none", "=None", 0)
		EndIf
	Next

EndFunc ; ==> StandardizeKeywords()

Func FixUserVariables()
	
	UpdateProgress(80, "Fixing user defined variables...")
	
	; PPL User statements require a Set command in front of them in OpenPPL but only when you actually intend
	; to set the value and not reference the value so this script only attempts to add set if it is the last User
	; statement in the line
		
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " user") Then
			$source[$i] = StringReplace($source[$i], " user", " SET user", -1)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "when set user") Then
			$source[$i] = StringReplace($source[$i], "when set user", "WHEN user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "when ( set user") Then
			$source[$i] = StringReplace($source[$i], "when ( set user", "WHEN ( user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "and set user") Then
			$source[$i] = StringReplace($source[$i], "and set user", "AND user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "and ( set user") Then
			$source[$i] = StringReplace($source[$i], "and ( set user", "AND ( user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "or set user") Then
			$source[$i] = StringReplace($source[$i], "or set user", "OR user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "or ( set user") Then
			$source[$i] = StringReplace($source[$i], "or ( set user", "OR ( user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "not set user") Then
			$source[$i] = StringReplace($source[$i], "not set user", "NOT user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "not ( set user") Then
			$source[$i] = StringReplace($source[$i], "not ( set user", "NOT ( user")
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " User", 1) Then
			$source[$i] = StringReplace($source[$i], " User", " user")
		EndIf
	Next
	
EndFunc ; ==> FixUserVariables()

Func AddForce()
	
	UpdateProgress(85, "Adding missing force commands...")
	
	; check to make sure lines that end in actions have 'FORCE' at the end
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 4) = " Bet" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 5) = " Call" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 5) = " Fold" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 7) = " BetPot" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 9) = " RaiseMin" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 9) = " RaiseMax" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 9) = " RaisePot" Then
			$source[$i] = $source[$i] & " FORCE"
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "FORCE FORCE") Then
			$source[$i] = StringReplace($source[$i], "FORCE FORCE", "FORCE", 0)
		EndIf
	Next
	
EndFunc ; ==> AddForce()

Func FixPercentages()
	
	UpdateProgress(86, "Fixing 1% - 99% PotSize...")
	
	If $FixPercentagesOnly = True Then
		; remove any space before '%'
		For $i = 0 to $NumberOfLines Step + 1
			If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " %") Then
				$source[$i] = StringReplace($source[$i], " %", "%", 0)
			EndIf
		Next
	EndIf
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% PotSize") Then
			For $j = 1 to 9 Step +1
				$percentage = " " & $j & "% PotSize"
				$fixed_percentage = " 0.0" & $j & " * PotSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% PotSize") Then
			For $j = 10 to 99 Step +1
				$percentage = " " & $j & "% PotSize"
				$fixed_percentage = " 0." & $j & " * PotSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	UpdateProgress(87, "Fixing 100% - 399% PotSize...")
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 100% PotSize") Then
			$source[$i] = StringReplace($source[$i], " 100% PotSize", " 1.00 * PotSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% PotSize") Then
			For $j = 101 to 199 Step +1
				$percentage = " " & $j & "% PotSize"
				$fixed_percentage = " " & $j/100 & " * PotSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 200% PotSize") Then
			$source[$i] = StringReplace($source[$i], " 200% PotSize", " 2.00 * PotSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% PotSize") Then
			For $j = 201 to 299 Step +1
				$percentage = " " & $j & "% PotSize"
				$fixed_percentage = " " & $j/100 & " * PotSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 300% PotSize") Then
			$source[$i] = StringReplace($source[$i], " 300% PotSize", " 3.00 * PotSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% PotSize") Then
			For $j = 301 to 399 Step +1
				$percentage = " " & $j & "% PotSize"
				$fixed_percentage = " " & $j/100 & " * PotSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	UpdateProgress(88, "Fixing 1% - 99% StackSize...")
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% StackSize") Then
			For $j = 1 to 9 Step +1
				$percentage = " " & $j & "% StackSize"
				$fixed_percentage = " 0.0" & $j & " * StackSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% StackSize") Then
			For $j = 10 to 99 Step +1
				$percentage = " " & $j & "% StackSize"
				$fixed_percentage = " 0." & $j & " * StackSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	UpdateProgress(89, "Fixing 100% - 399% StackSize...")
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 100% StackSize") Then
			$source[$i] = StringReplace($source[$i], " 100% StackSize", " 1.00 * StackSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% StackSize") Then
			For $j = 101 to 199 Step +1
				$percentage = " " & $j & "% StackSize"
				$fixed_percentage = " " & $j/100 & " * StackSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 200% StackSize") Then
			$source[$i] = StringReplace($source[$i], " 200% StackSize", " 2.00 * StackSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% StackSize") Then
			For $j = 201 to 299 Step +1
				$percentage = " " & $j & "% StackSize"
				$fixed_percentage = " " & $j/100 & " * StackSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " 300% StackSize") Then
			$source[$i] = StringReplace($source[$i], " 300% StackSize", " 3.00 * StackSize", 0)
		EndIf
	Next
	
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "% StackSize") Then
			For $j = 301 to 399 Step +1
				$percentage = " " & $j & "% StackSize"
				$fixed_percentage = " " & $j/100 & " * StackSize"
				$source[$i] = StringReplace($source[$i], $percentage, $fixed_percentage, 0)
			Next
		EndIf
	Next
	
EndFunc ; ==> FixPercentages()

Func CleanUp()
	
	UpdateProgress(90, "Performing clean up...")
	
	; make all '=' consistant by adding leading and trailing spaces
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "=") Then
			$source[$i] = StringReplace($source[$i], "=", " = ", 0)
		EndIf
	Next
	
	; fix any '< =' that were created
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "< =") Then
			$source[$i] = StringReplace($source[$i], "< =", "<=", 0)
		EndIf
	Next
	
	; fix any '> =' that were created
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "> =") Then
			$source[$i] = StringReplace($source[$i], "> =", ">=", 0)
		EndIf
	Next
	
	; remove any spaces after all '('
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "( ") Then
			$source[$i] = StringReplace($source[$i], "( ", "(", 0)
		EndIf
	Next
	
	; remove any spaces before all ')'
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], " )") Then
			$source[$i] = StringReplace($source[$i], " )", ")", 0)
		EndIf
	Next
	
	; make sure there is a space before all FORCE commands
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringRight($source[$i], 5) = "FORCE" And StringRight($source[$i], 6) <> " FORCE" Then
			$source[$i] = StringReplace($source[$i], "FORCE", " FORCE")
		EndIf
	Next
	
	; make sure there is a space after all WHEN commands
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringLeft($source[$i], 4) = "WHEN" And StringLeft($source[$i], 5) <> "WHEN " Then
			$source[$i] = StringReplace($source[$i], "WHEN", "WHEN ")
		EndIf
	Next
	
	; make sure there is a space after all WHEN commands when preceeded by a space (common)
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringLeft($source[$i], 5) = " WHEN" And StringLeft($source[$i], 6) <> " WHEN " Then
			$source[$i] = StringReplace($source[$i], " WHEN", " WHEN ")
		EndIf
	Next
	
	; fix any OpponentCalledOnFlop that got replaced with OpponentBotCalledOnFlop
	For $i = 0 to $NumberOfLines Step + 1
		If StringLeft($source[$i], 2) <> "//" And StringInStr($source[$i], "OpponentBotCalledOnFlop") Then
			$source[$i] = StringReplace($source[$i], "OpponentBotCalledOnFlop", "OpponentCalledOnFlop", 0)
		EndIf
	Next
	
EndFunc ; ==> CleanUp()

Func LoadSymbols()
	
	$i = 0
	$line = ""
	
	$symbolsfile =  FileOpen(@ScriptDir & "\symbols.txt", 0)
	
	; Check if the input file opened for reading OK
	If $symbolsfile = -1 Then
		MsgBox(0, "Error", "The file 'symbols.txt' (required) was not found.")
		Exit
	EndIf
	
	While 1
		
		$i = $i + 1
		$line = FileReadLine($symbolsfile)
		If @error = -1 Or $line = "" Then ExitLoop
		$symbols[$i] = $line
		$NumberOfSymbols = $i
		
	WEnd
	
	FileClose($symbolsfile)
	
EndFunc ; ==> LoadSymbols()

Func LoadSourceFile()
	
	$i = 0	
	$line = ""
	
	$sourcefile = FileOpen($PPL, 0)
	
	; Check if the input file opened for reading OK
	If $sourcefile = -1 Then
		MsgBox(0, "Error", "Unable to open file.")
		Exit
	EndIf
	
	While 1
		
		$i = $i + 1
		$line = FileReadLine($sourcefile)
		If @error = -1 Then ExitLoop
		$source[$i] = $line
		$original[$i] = $line
		$NumberOfLines = $i
		
	WEnd
	
	FileClose($sourcefile)
	
EndFunc ; ==> LoadSourceFile()

Func WriteConvertedFile()
	
	UpdateProgress(95, "Writing converted file...")
	
	$OpenPPL = StringLeft($PPL, StringLen($PPL)-3) & "oppl"
	$convertedfile = FileOpen($OpenPPL, 2)
	
	; Check if the output file opened for writing OK
	If $convertedfile = -1 Then
		MsgBox(0, "Error", "Unable to open file.")
		Exit
	EndIf
	
	FileWriteLine($convertedfile, "##notes##")
	FileWriteLine($convertedfile, @CRLF)
	FileWriteLine($convertedfile, "///////////////////////////////////////////////////////////////////////")
	FileWriteLine($convertedfile, "//                                                                   //")
	FileWriteLine($convertedfile, "// Converted using Poker_dog's PPL to OpenPPL Converter version " & $version & " //")
	FileWriteLine($convertedfile, "//                                                                   //")
	FileWriteLine($convertedfile, "///////////////////////////////////////////////////////////////////////")
	FileWriteLine($convertedfile, @CRLF)
		
	For $i = 1 To $NumberOfLines Step + 1
		FileWriteLine($convertedfile, $source[$i])
	Next
	
	FileClose($convertedfile)
	
EndFunc ; ==> WriteConvertedFile()

Func WriteChangeLog()
	
	UpdateProgress(100, "Writing change log...")
	
	$Log = StringLeft($PPL, StringLen($PPL)-4) & " - change.log"
	$changelog = FileOpen($Log, 2)
	
	; Check if the output file opened for writing OK
	If $changelog = -1 Then
		MsgBox(0, "Error", "Unable to open file.")
		Exit
	EndIf
	
	FileWriteLine($changelog, "Change Log" & @CRLF & @CRLF)
	FileWriteLine($changelog, @TAB & "Source: " & $PPL)
	FileWriteLine($changelog, @TAB & "Output: " & $OpenPPL & @CRLF & @CRLF)
	
	For $i = 1 To $NumberOfLines Step + 1
		If StringCompare($original[$i], $source[$i], 1) <> 0 Then
			FileWriteLine($changelog, "Line: " & $i)
			FileWriteLine($changelog, " PPL: " & $original[$i])
			FileWriteLine($changelog, "OPPL: " & $source[$i])
			FileWriteLine($changelog, @CRLF & @CRLF)
		EndIf
	Next
	
EndFunc ; ==> WriteChangeLog()

Func UpdateProgress($percent,$routine)
	
	ProgressSet($percent, @LF & $routine, "")
	
EndFunc ; ==> UpdateProgress()

Func Terminate()
	Exit
EndFunc ; ==> Terminate()
