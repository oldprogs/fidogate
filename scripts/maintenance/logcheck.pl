#!/usr/local/bin/perl
#
# $Id: logcheck.pl,v 4.4 1998/01/02 14:37:04 mj Exp $
#
# Create report for sendmail check_mail rules
#

$NEWSGROUPS = "fido.de.lists";
$SUBJECT    = "Fido.DE Sendmail Reject Report";

$INEWS      = "/usr/bin/inews -h -S";
$SENDMAIL   = "/usr/sbin/sendmail";


require "getopts.pl";
&Getopts('g:s:nm:v');

if($opt_g) {
    $NEWSGROUPS = $opt_g;
}
if($opt_s) {
    $SUBJECT    = $opt_s;
}
if($opt_n) {
    open(OUT, "|$INEWS") || die "logreport: can't open pipe to inews\n";
    select(OUT);
    $out_flag = 1;
}
if($opt_m) {
    open(OUT, "|$SENDMAIL $opt_m")
	|| die "logreport: can't open pipe to sendmail\n";
    select(OUT);
    $out_flag = 1;
}



print "Newsgroups: $NEWSGROUPS\n" if($opt_n);
print "Subject: $SUBJECT\n" if($opt_m || $opt_n);
print "\n" if($opt_m || $opt_n);


# Read sendmail log
while(<>) {
    if( /^(... .\d \d\d:\d\d:\d\d) / ) {
	$first_date = $1 if(!$first_date);
	$last_date  = $1;
    }

    if( /check_mail \((<.*>)\) rejection: 551.*/ ) {
	$reject{$1}++;
	print "reject: $1\n" if($opt_v);
    }

    if( /check_mail \((<.*>)\) rejection: 451.*unresolv/ ) {
	$nodns{$1}++;
	print "no DNS: $1\n" if($opt_v);
    }

    if( /check_rcpt \((<.*>)\) rejection: 551.*we do not relay/ ) {
	$relay{$1}++;
	print "relay : $1\n" if($opt_v);
    }
}

# Report
print "sendmail reject report: $first_date -- $last_date\n";

if(scalar(%reject)) {
    print
	"\nAddresses rejected using blacklist:\n",
	"-----------------------------------\n";
    for $k (sort { $reject{$b} <=> $reject{$a} } keys(%reject)) {
	printf "%8d %s\n", $reject{$k}, $k;
    }
}

if(scalar(%nodns)) {
    print
	"\nAddresses without valid DNS entry:\n",
	"----------------------------------\n";
    for $k (sort { $nodns{$b} <=> $nodns{$a} } keys(%nodns)) {
	printf "%8d %s\n", $nodns{$k}, $k;
    }
}

if(scalar(%relay)) {
    print
	"\nAddresses from relay attempts:\n",
	"------------------------------\n";
    for $k (sort { $relay{$b} <=> $relay{$a} } keys(%relay)) {
	printf "%8d %s\n", $relay{$k}, $k;
    }
}


if($out_flag) {
    close(OUT);
}