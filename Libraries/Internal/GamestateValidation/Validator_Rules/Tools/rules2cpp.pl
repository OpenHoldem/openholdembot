#*****************************************************************************
#
# This file is part of the OpenHoldem project
#   Source code:           https://github.com/OpenHoldem/openholdembot/
#   Forums:                http://www.maxinmontreal.com/forums/index.php
#   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
#
#
#*****************************************************************************
#
# Purpose:
#
#*****************************************************************************

##################################################
#
# Rules2Cpp
# =========
#
# PURPOSE: Translating rules for the OH-Validator
#   into usable CPP-code (preprocessor-macros).
#
# USAGE: At the command-line:
#   "perl rules2cpp.pl <THE_RULES_FILE>"
#   This will print the generated code to stdout.
#
# NOTE: For bitwise operations you have to typecast
#   gws-operands to integer manually.
#
# AUTHOR: TheHighFish
#
# DATE: 2008.11.31
#
##################################################
#
# Rules look like this:
#
# BEGIN_TESTCASE
#     TESTCASE_ID ("0074")
#     REASONING ("Range check for symbol nbetstorais")
#     PRECONDITION (true)
#     POSTCONDITION (nbetstorais >= 1)
#     SYMBOLS_POSSIBLY_AFFECTED ("nbetstorais")
# END_TESTCASE
#
# We only have to put each symbol into a gws-function-call,
#   and leave the rest as is.
#
##################################################
#
# Operate on all lines...
while (<>)
{
	# Fix 2 common misspellings
	s/TEST_ID/TESTCASE_ID/g;
	s/TASTCASE/TESTCASE/g;
	# Print to stdout without change
	# * lines, that shall stay as is
	# * lines, that already got converted (including "gws")
	# * preprocessor macros ("#define")
	# * empty lines
	# * comments
	if ((/^.*BEGIN_TESTCASE.*$/)
		|| (/^.*TESTCASE_ID.*/)
		|| (/^.*HEURISTIC_RULE.*$/)
		|| (/^.*REASONING.*$/)
		|| (/^.*SYMBOLS_POSSIBLY_AFFECTED.*$/)
		|| (/^.*END_TESTCASE.*$/)
		|| (/^.*gws.*$/)
		|| (/^#define.*$/)
		|| (/^[ ]*$/)
		|| (/^.*\/\/.*$/))
	{
		print $_;
	}
	else
	{
		# In the other lines place a gws-call around each symnols,
		#   then print to stdout.
		# Each symbol has to start either with a character or a Dollar,
		#   but fortunatelly not with "x", as this way we avoid 
		#   substituting hexadecimal numbers.			
		s/[a-wyz\$][a-z_\$0-9]*/gws("$&")/g;
		# If we substituted true or false,
		#   substitute it back.
		s/gws\("true"\)/true/g;
		s/gws\("false"\)/false/g;
		print $_;
	}
}