###################################################################
#
#  EmptyPerlBot.Pl
#  ======================
#
#  Author: TheHighFish
#
#  Created: 2008.03.05
#
#  Last change: 2008.03.18
#
#  Purpose: A skeleton of a Perl bot 
#    to show people how to use Perl
#    for OpenHoldem.
#
###################################################################
#
#  USAGE:
#  ======
#
#    You need ActiveState Perl properly installed.
#      OH must be able to find "PerlEz.dll"
#      You maybe must change the PATH search path from the DOS prompt:
#      > set PATH = C:\Programs\Perl\Bin;%PATH%
#    The package Win32::API must be installed. 
#      Use Perls package ppm.
#    For details please have a look at the docu
#      in the OpenHoldem Wiki.
#      
#    To request an OH symbol or DLL symbol:
#      use the function "gws (<SYMBOL>)"  
#
#    To request a Perl symbol
#      a) inside the DLL: use "gws (<SYMBOL>)"
#      b) inside a formula: Just access the symbol :)
#   
#   Perl symbols have to begin with "pl_", NOT "pl$",
#     as the Dollar character is reserved in Perl.
#
#   Perl symbols shall be functions without arguments;
#     if you need constant symbols, just use constant functions.
#     (A function call is cheaper than the evaluation of a string,
#     which involves a compilation step.)  
#
#   You nevertheless need a classic OpenHoldem formula,
#     but you may redirect any symbol to a perl function;
#     eg:
#             ##f$call##
#             pl_call
#
##################################################################
#
#  It's better to use a strict and conservative coding style.
#    As we have limited debugging support running our bot,
#    I recommend to test as much as possible offline
#    and do at least a syntax check (from the command line)
#    > perl -w -c EmptyPerlBot.pl
#

use strict;
use warnings;
use diagnostics;

##################################################################
#
#  Including Perl modules
#

#  Set the search path for our Perl modules.
#    This path has to be absolute (as a security measure)
#    (Note: '\' is an escape character. We have to write
#    two to get a single one.)
#
use lib ("C:\\Dokuments\\Jeff\\Desktop\\PerlBotIntro\\");


#  The module "Perl_OH_Interaction.pm" contains the technical stuff
#      to get OpenHoldem running with Perl.
#
use Debug;
use Perl_OH_Interaction;


##################################################################
#
#  Some constants and variables.
#

#  A hash to cache OpenHoldem symbols
#
#  You can later access the symbol values
#    by their name, e.g.:  $OHS{'dealerchair'}
#    
my %OHS;


#  Symbols we want to cache
#    (Note: "qw" avoids putting each symbol in quotation marks.)
#
my @OH_Symbolnames = qw(br dealerchair pot nbetstocall islistalli ishipair);

##################################################################
#
#  Some functions
#

#  Calling OpenHoldem from Perl is not implemented
#    in an extremely efficient way.
#  Therefore it is recommended to cache the symbols     
#    you need on Perl side to request them only
#    once per cycle.
#  Updating the symbol cache should be the first action
#    in a cycle. You may put this operation e.g. 
#    into the "allin" function.
#
sub cache_Symbols
{
    #  Get your symbol values
    send_DebugMsg('cache_Symbols');
    foreach my $Symbol (@OH_Symbolnames)
    {
	$OHS{$Symbol} = gws($Symbol); 
	send_DebugMsg($Symbol, '=>', $OHS{$Symbol});
    }
}


sub pl_play
{
    #  Return values
    #    -2:  Leave the table
    #    -1:  Do nothing
    #     0:  Sit out
    #     1:  Play (and post blinds)
    # 
    #  Just to get it running....
    return 1;
}


sub pl_call
{    
    #  You can put any kind of Perl code
    #    to oyur functions.
    #
    send_DebugMsg('caching');
    cache_Symbols();
    send_DebugMsg('dealerpos', $OHS{'dealerpos'});
    return ($OHS{'dealerpos'} > $OHS{'br'}); 
    return 1;
}


#
#  AUTOLOAD handles unknown functions. 
#
#    As this should happen only for requests of unknown symbols,
#    we don`t check the expected return type and return 0.0,
#    as this should be fine for OpenHoldem.
#

sub AUTOLOAD
{
    #  Put some error handling here, if you need.
    send_DebugMsg('AUTOLOAD');
    return 0;	
}

