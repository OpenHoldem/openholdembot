package Debug;


use strict;
use warnings;
use diagnostics;

use Time::localtime;

use Exporter;                    
our @EXPORT = qw( send_DebugMsg );  #  Exporting some functions
our @ISA = qw( Exporter );


use constant the_DebugFile => "C:\\Documents\\Jeff\\Desktop\\";


use constant TRUE => 1;
use constant FALSE => 0;


my $first_Action = TRUE;


#  send_DebugMsg
#
sub send_DebugMsg
{
    if ($first_Action == TRUE)
    {
	#  Open for writing,
	#    i.e. delete the old file.
	open(DEBUG, ">", the_DebugFile);
	$first_Action = FALSE;
    }
    else
    {
	#  Open for appending.
	open(DEBUG, ">>", the_DebugFile);
    }
    #  Print time to debug log.
    print (DEBUG scalar(gmtime()));
    foreach my $Msg(@_)
    {
	print(DEBUG " "); 
	print(DEBUG $Msg);	
    }
    print(DEBUG "\n");
    #  Immediately close file,
    #    to create a persistent debug log.
    close(DEBUG);
}


return 1;
