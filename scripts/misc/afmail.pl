#!/usr/local/bin/perl
#
# Generate AREAFIX message
#

if($#ARGV != 2) {
    print STDERR "usage: afmail FROM TO PASSWD\n";
    print STDERR "       FROM / TO = Z:N/F.P\n";
    exit 1;
}

$ADDR   = $ARGV[0];
$UPLINK = $ARGV[1];
$PASSWD = $ARGV[2];

$PRG    = "<LIBDIR>/rfc2ftn";
$LIB	= "";
$SPOOL	= "";
$OPT	= "";

## Test
#$PRG	= "../src/rfc2ftn";
#$LIB	= "-L./lib";
#$SPOOL	= "-S.";
#$OPT	= "-vvvvv";

$cmd = "$PRG $LIB $SPOOL $OPT -i -t -a$ADDR -u$UPLINK";

open(PRG, "|$cmd") || die "can't open pipe to $cmd\n";
select(PRG);

#printf "From: Sysop@%s\n", $ADDR;
printf "To: Areafix@%s\n", $UPLINK;
printf "Subject: %s\n", $PASSWD;
printf "X-Flags: MN\n\n";

while(<STDIN>) {
    print;
}

close(PRG);
