#!/usr/bin/perl
#
# $Id: ffxrmail.pl,v 4.4 1998/02/25 09:15:27 mj Exp $
#
# sendmail frontend for processing ffx rmail commands
#
 
<INCLUDE config.pl>

require "getopts.pl";
&Getopts('vc:');

# read config
$CONFIG      = $opt_c ? $opt_c : "<CONFIG_FFX>";
&CONFIG_read($CONFIG);

# configuration
$SENDMAIL = &CONFIG_get("FFXRmailSendmail");
die "ffxrmail:$CONFIG:FFXRmailSendmail not defined\n" if(! $SENDMAIL);


# ignore SIGPIPE
$SIG{"PIPE"} = "IGNORE";


# read From_ line from <STDIN>
$_ = <STDIN>;
if( /^From ([^ ]*) / ) {
    $from = $1;
    $from = "news\@localhost" if($from eq "");
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
