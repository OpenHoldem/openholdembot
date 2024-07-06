#******************************************************************************
#
# This file is part of the OpenHoldem project
#   Download page:         http://code.google.com/p/openholdembot/
#   Forums:                http://www.maxinmontreal.com/forums/index.php
#   Licensed under GPL v3: http://www.gnu.org/licenses/gpl.html
#
#******************************************************************************
#
# PURPOSE: Fixing odd formatting in Shanky-PPL bot-logic
#   (multiple line-breaks inside a hand-expression and such)
#   Result: one line per when condition.
#
#******************************************************************************
#
# PREREQUISITES: ActiveState Perl installed
#
# USAGE: at the DOS-prompt type
#
#   perl linebreak_correction_for_non_stqndard_formatted_profiles.pll 
#     < MYPROFILE.TXT > MYPROFILE_CORRECTED.TXT
#
#******************************************************************************

use strict;
use warnings;
use diagnostics;

my $custom_seen = 0;
my $profile_code;

# While there is input: process it line by line
while (<>)
{
    # Replace tabs by spaces
	s/\t/    /;
	if (!$custom_seen) 
	{
		print;
		if (m/^custom/i)
		{
			$custom_seen = 1;
		}
		next;
	}
	# Remove all line-breaks
	chomp;
	# Concatenate everything
	$profile_code .= "   ";
	$profile_code .= $_;
	next;
}

$_ = $profile_code;
#Introduce new line-breaks before "when"
s/   when/\nWHEN/ig;
# Also linebreaks before and after special keywords
s/   custom/\ncustom\n/ig;
s/   preflop/\npreflop\n/ig;
s/   flop/\nflop\n/ig;
s/   turn/\nturn\n/ig;
s/   river/\nriver\n/ig;
# Remove superfluos spaces
s/     / /g;
s/    / /g;
s/   / /g;
s/  / /g;
print;