Short manual to use RegressionTests:
====================================

Install:
--------
- install Python 3.4. If location is not C:\Python34\python.exe, then adapt RegressionTests/setPythonEnv.bat
- install PyQt4.x for Python 3.4 (e.g. PyQt4-4.11.1-gpl-Py3.4-Qt4.8.6-x32.exe)
- compile xmlrpc-c release (sln in xmlrpc-c\Windows\project\vs2010, could not add to main solution,
  that is not possible with vc express)
- compile OH sln (for OH stuff, SymbolDumperDll and ManualMode-XMLRPC)
- copy versus.bin to RegressionTests/tmp_run_dir (want to automate that, but not sure if I can
  extract RAR from python, will check)


Running:
--------
- execute RegressionTests/runTestDefault.bat or RegressionTests/runTestShort.bat (same single test atm)


Creating Tests, using TestSuite2/HandHistoryParser:
---------------------------------------------------
- RegressionTests\Tools\startTestSuite.bat
- RegressionTests\Tools\startHandHistoryParser.bat
  The imported hands end up in RegressionTests\Tools\HandHistoryParser\parsed.

You can use HandHistoryParser to import your HH. The RegressionTests run tests
in RegressionTests\Testcases\TestsDefault|TestsShort with the 2 bat files. The output is
compared to RegressionTests\ExpectedOutput. So if you have new tests, copy the logs to
RegressionTests\ExpectedOutput.

Adding/Configuring new Tests:
-----------------------------
Look at the RegressionTests\runTestDefault.bat|runTestShort.bat and
RegressionTests\SettingsDefault.py|SettingsShort.py to see how to configure/run new tests.
There are even more config possibilities in some of the .py files...

ToDo after change/add/remove of Symbols in OH:
--------------------------------------------------
If you changed/added/removed symbols in OH, recreate the AllSymbols.h of SymbolDumperDll
by running RegressionTests\Tools\recreateAllSymbolsHeaderForDumperDll.bat. Run all tests,
verify logs, and recreate the RegressionTests\ExpectedOutput\ExpectedOutput.zip with the new
logs and commit it.

