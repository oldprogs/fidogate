#!/usr/local/bin/perl
#
# $Id: ffxrmail.pl,v 4.0 1996/04/17 18:17:40 mj Exp $
#
# sendmail frontend for processing ffx rmail commands
#

require "getopts.pl";
&Getopts('v');

# configuration
$PROTO    = "FFX";
$SENDMAIL = "/usr/lib/sendmail -odq -oee -oi -oMr$PROTO";

# ignore SIGPIPE
$SIG{"PIPE"} = "IGNORE";


# read From_ line from <STDIN>
$_ = <STDIN>;
if( /^From ([^ ]+) / ) {
    $from = $1;
}
else {
    die "ffxrmail: no From_ line at start of message\n";
}
print "ENVELOPE: $from\n" if($opt_v);

# sendmail command
$from =~ s'\\'\\\\'g;		# quote \ 
$from =~ s'"'\"'g;		# quote "
$cmd = "$SENDMAIL -f\"$from\" @ARGV";

print "CMD: $cmd\n" if($opt_v);

open(PIPE, "|$cmd") || die "ffxrmail: ERROR: open pipe to sendmail\n";
while(<STDIN>) {
    print PIPE $_;
}
close(PIPE);
die "ffxrmail: ERROR: close pipe to sendmail\n" if($?);

exit(0);
