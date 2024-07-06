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
# Verify_SymbolNames
# ====================
#
# PURPOSE: Checking rules for the validator
#   for invalid symbol names (typos) by generating
#   an UDF that contains all symbols and can be checked 
#   using OH.
#
# USAGE: At the command-line:
#   "perl verify_symbolnames.pl <THE_RULES_FILE>"
#   This will print a gnererated UDF to stdout,
#
# AUTHOR: TheHighFish
#
# DATE: 2008.12.17
#
##################################################

# Print UDF header
# and leading 0 to generate a valid expression.
print "##f\$verify_symbolnames##\n0";
while (<>)
{
	# Skip lines without symol names
	if ((/^.*BEGIN_TESTCASE.*$/)
		|| (/^.*TESTCASE_ID.*/)
		|| (/^.*HEURISTIC_RULE.*/)
		|| (/^.*REASONING.*$/)
		|| (/^.*END_TESTCASE.*$/)
		|| (/^#define.*$/)
		|| (/^[ ]*$/)
		|| (/^.*\/\/.*$/))
	{		
		next;	
	}
	# Remove everything from the code, that's not part of a symbol,
	# i.e replace it by space to not concat symbols.
	# Then remove newlines and tabs 
	s/PRECONDITION/ /g;
	s/POSTCONDITION/ /g;
	s/SYMBOLS_POSSIBLY_AFFECTED//g;
	s/gws/ /g;
	s/int/ /g;
	s/[()><=!&|",]/ /g;
	s/\n/ /g;
	s/\t/ /g;
	# Add a leading "+" to each symbol to generate a valid expression
	s/[a-z0-9_\$]+/+$&/g;
	# Finally remove all blanks
	s/ +//g;
	# Print out an UDF for OH.
	print "$_";
}