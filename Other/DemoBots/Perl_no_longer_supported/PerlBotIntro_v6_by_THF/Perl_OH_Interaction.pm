###################################################################
#
#  PERL_OH_INTERACTION.PL
#  ======================
#
#  Author: TheHighFish
#
#  Created: 2008.01.08
#
#  Last change: 2008.06.24 by Elvis
#
#  Purpose: A Perl module for OpenHoldem
#    containing the technical stuff to get
#    OpenHoldem working together with Perl.
#
###################################################################
#
#  USAGE:
#  ======
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
#    To request the table name:
#      use the function "gwt ()"
#    To request a player name:
#      use the function "gwt (<PLAYER NUMBER>)"
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
##################################################################

package Perl_OH_Interaction;

#
#  Enforcing a strict, conservative coding style
#
#  As our embedded interpreter couldn't send errors and warnings 
#    via stderr, we recommend to at least compile your script 
#    at the command line, before loading it into OpenHoldem:      
#      > perl -w -c mybot.pl
#

use strict;
use warnings;
use diagnostics;

#
#  For developing only
#
use Debug;

###################################################################
#
#  EXPORT INTERFACE
#
###################################################################

use Exporter;
#  Exporting some functions
our @EXPORT = qw( gws set_CallbackPointer_to_gws gwt set_CallbackPointer_to_gwt gwp set_CallbackPointer_to_gwp );
our @ISA = qw (Exporter);


###################################################################
#
#  CALLBACK FUNCTION (to get_OpenHoldem_Symbol)
#
#  Unfortunately our embeddable Perl interpreter doesn't support 
#    callback functions directly, so we have to build them 
#    the tricky dirty way:
#    - Handle a pointer to the callback C function to our Perl Script.
#    - Pass this pointer to a DLL, written in C.
#    - Inside the library: Call back via the pointer.
#
###################################################################

#
#  Use the Win32 API library to access DLLs
#
#  Alternatives are:
#    - FFI::Library 
#    - C::Dynalib;
#
use Win32::API;


#
#  Global pointer to C function "gws","gwt,"gwp"
#    implemented in OHs "Perl.cpp"
#
my $Pointer_to_OpenHoldem_CallbackFunction_gws;
my $Pointer_to_OpenHoldem_CallbackFunction_gwt;
my $Pointer_to_OpenHoldem_CallbackFunction_gwp;


#
#  set_CallbackPointer_to_gws
#  set_CallbackPointer_to_gwt
#  set_CallbackPointer_to_gwp
#
#  This function shall be called once by OpenHoldem
#    to initialize the pointer to the function "gws",
#    directly after loading the interpreter
#    and prior to any calls to "get_OpenHoldem_Symbol".
#
#  Input: A C function pointer,
#    typecasted to an integer to be on the secure side,
#    as Perl doesn't support pointers to C functions.
# 
sub set_CallbackPointer_to_gws
{
    ($Pointer_to_OpenHoldem_CallbackFunction_gws) = @_;
    send_DebugMsg("set_CallbackPointer_to_gws:",
    		  $Pointer_to_OpenHoldem_CallbackFunction_gws);
    #  The right time to load the callback function from the DLL.
    load_DLL_Callback_Function_gws();

    return 1;
}

sub set_CallbackPointer_to_gwt
{
    ($Pointer_to_OpenHoldem_CallbackFunction_gwt) = @_;
    send_DebugMsg("set_CallbackPointer_to_gwt:",
    		  $Pointer_to_OpenHoldem_CallbackFunction_gwt);
    #  The right time to load the callback function from the DLL.
    load_DLL_Callback_Function_gwt();
    
    return 1;
}

sub set_CallbackPointer_to_gwp
{
    ($Pointer_to_OpenHoldem_CallbackFunction_gwp) = @_;
    send_DebugMsg("set_CallbackPointer_to_gwp:",
    		  $Pointer_to_OpenHoldem_CallbackFunction_gwp);
    #  The right time to load the callback function from the DLL.
    load_DLL_Callback_Function_gwp();
    
    return 1;
}

#
#  Function reference to "get_OpenHoldem_x" 
#    in the callback DLL
#
my $DLL_get_OpenHoldem_Symbol;
my $DLL_get_OpenHoldem_TableName;
my $DLL_get_OpenHoldem_PlayerName;


sub load_DLL_Callback_Function_gws
{
    $DLL_get_OpenHoldem_Symbol = new Win32::API(
        #						
    	#  Perl callback DLL
	    #    Make sure, the DLL can be found by the loader.
        #
        'Perl_Callback.dll',
        #
        #  Function to load - make sure, you take the mangled name, 
        #    as it appears in the DLL.
        #    (DependencyWalker or FileAlyzer may be helpful.)
        #
        '_get_OpenHoldem_Symbol_indirect@12', 
        #
        #  Parameters
        #    1. Integer, representing a pointer to the function gws.
        #      We got an integer and have to pass it that way.
        #      If we would specify a pointer, Perl would create
        #      a pointer to that integer.
        #    2. Pointer to string (symbol) .    
        #    3. Pointer to string (result); we use a string, as 
        #      Perls Win32::API seems to have problems returning 
        #      values > 4 bytes (e.g. doubles); 
        #
        'IPP',
        #
        #  Return type
        #    None
        #
        '');
    send_DebugMsg("load_DLL_Callback_Function: $DLL_get_OpenHoldem_Symbol");
    
}

sub load_DLL_Callback_Function_gwt
{
    $DLL_get_OpenHoldem_TableName = new Win32::API(
        #						
        #  Perl callback DLL
	    #  Make sure, the DLL can be found by the loader.
        #
        'Perl_Callback.dll',
        #
        #  Function to load - make sure, you take the mangled name, 
        #    as it appears in the DLL.
        #    (DependencyWalker or FileAlyzer may be helpful.)
        #
        '_get_OpenHoldem_TableName_indirect@8', 
        #
        #  Parameters
        #    1. Integer, representing a pointer to the function gws.
        #      We got an integer and have to pass it that way.
        #      If we would specify a pointer, Perl would create
        #      a pointer to that integer.
        #    2. Pointer to string (result).    
        #
        'IP',
        #
        #  Return type
        #    None
        #
        '');
    send_DebugMsg("load_DLL_Callback_Function: $DLL_get_OpenHoldem_TableName");
    
}

sub load_DLL_Callback_Function_gwp
{
    $DLL_get_OpenHoldem_PlayerName = new Win32::API(
        #						
        #  Perl callback DLL
	    #  Make sure, the DLL can be found by the loader.
        #
        'Perl_Callback.dll',
        #
        #  Function to load - make sure, you take the mangled name, 
        #    as it appears in the DLL.
        #    (DependencyWalker or FileAlyzer may be helpful.)
        #
        '_get_OpenHoldem_PlayerName_indirect@12', 
        #
        #  Parameters
        #    1. Integer, representing a pointer to the function gws.
        #      We got an integer and have to pass it that way.
        #      If we would specify a pointer, Perl would create
        #      a pointer to that integer.
        #    2. Integer ( player number ).
        #    3. Pointer to string (result).    
        #
        'IIP',
        #
        #  Return type
        #    None
        #
        '');
    send_DebugMsg("load_DLL_Callback_Function: $DLL_get_OpenHoldem_TableName");
    
}

#
#  This function shall be called once by OH
#    to check, if the callback function could be loaded correctly.
#    If not: Most probably DLL not found.
#
sub check_DLL_Callback_Function
{
    return $DLL_get_OpenHoldem_Symbol;
}

#
#  Global result buffer for the callback function
#    There's no need to allocate and initialize 
#    a local one on each call.
#    128 bytes is more than enough for a double.
#
my $the_Result = " " x 128;

#
#  get_OpenHoldem_Symbol
#
#  Input: a string (OpenHoldem symbol)
#
#  Output: a double (OpenHoldem symbol value)
#
#  Note: If you have large formulas, you might get
#    a performance improvement, if you buffer the
#    symbols once each scrape (e.g. in a hash or an array) 
#    instead of requesting them each time from OH.
#
sub get_OpenHoldem_Symbol
{
    my ($SymbolName) = @_;
    if (defined($DLL_get_OpenHoldem_Symbol))
    {
	$DLL_get_OpenHoldem_Symbol->Call
	    ($Pointer_to_OpenHoldem_CallbackFunction_gws,
	     $SymbolName,
	     $the_Result);
        #send_DebugMsg("Working");
    }
    else
    {
    send_DebugMsg("ERROR: Callback function undefined(gws).");
	$the_Result = 0;
    }
    #send_DebugMsg("get_OpenHoldem_Symbol: $the_Result");
    return unpack("Z128",$the_Result);
}

#
#  get_OpenHoldem_TableName
#
#  Output: a string (OpenHoldem table name)
#
sub get_OpenHoldem_TableName
{
    if (defined($DLL_get_OpenHoldem_TableName))
    {
	$DLL_get_OpenHoldem_TableName->Call
	    ($Pointer_to_OpenHoldem_CallbackFunction_gwt,
	     $the_Result);
        #send_DebugMsg("Working");
    }
    else
    {
    send_DebugMsg("ERROR: gwt Callback function undefined(gwt).");
	$the_Result = 0;
    }
    #send_DebugMsg("get_OpenHoldem_Symbol: $the_Result");
    return unpack("Z128",$the_Result);
}

#
#  get_OpenHoldem_PlayerName
#
#  Input: an integer (Player number)
#
#  Output: a string (Player name)
#
sub get_OpenHoldem_PlayerName
{
    my ($PlayerNumber) = @_;
    if (defined($DLL_get_OpenHoldem_PlayerName))
    {
	$DLL_get_OpenHoldem_PlayerName->Call
	    ($Pointer_to_OpenHoldem_CallbackFunction_gwp,
	     $PlayerNumber,
	     $the_Result);
        #send_DebugMsg("Working");
    }
    else
    {
    send_DebugMsg("ERROR: Callback function undefined(gwp).");
	$the_Result = 0;
    }
    #send_DebugMsg("get_OpenHoldem_Symbol: $the_Result");
    return unpack("Z128",$the_Result);
}

#
#  Abbreviations for DLL coders and lazy people
#
sub gws
{
    my ($SymbolName) = @_;
    my $the_Result = get_OpenHoldem_Symbol($SymbolName);
    #send_DebugMsg("gws: $SymbolName ".$result);    
    #  Make sure, we return a number as a result,
    #    not a string with trailing trash. 
    #    This does not matter, except for boolean values,
    #    where e.g. "0.0 #################" 
    #    will not be interpreted as false.
    $the_Result = 1.0 * $the_Result;
    return $the_Result;
}

sub gwt
{
    my $result = get_OpenHoldem_TableName();
    #send_DebugMsg("gwt: TableName: ".$result);    
    return $result;
}

sub gwp
{
    my ($PlayerNumber) = @_;
    my $result = get_OpenHoldem_PlayerName($PlayerNumber);
    #send_DebugMsg("gwp: "."$PlayerNumber".". player name: ".$result);    
    return $result;
}

#
#  Package loaded successfully
#
return 1;

# EOF
