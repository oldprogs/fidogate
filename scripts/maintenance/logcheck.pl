#!/usr/local/bin/perl
#
# $Id: logcheck.pl,v 4.6 1998/05/03 12:46:28 mj Exp $
#
# Create report for sendmail check_mail rules
#

$NEWSGROUPS = "fido.de.lists";
$SUBJECT    = "Fido.DE Sendmail Reject Report";

$INEWS      = "/usr/bin/inews -h -S";
$SENDMAIL   = "/usr/sbin/sendmail";


require "getopts.pl";
&Getopts('g:s:nm:vr');

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

    if(/check_mail \(([^\)]*)\) rejection: 551.*/ ||
       /check_mail, arg1=(.*), relay=(.*), reject=551.*/ ) {
	$a = $1;
	$a = "<$a>" if(! $a =~ /^<.*>$/);
	$r = $opt_r ? $2 : "";
	$reject{"$a /// $r"}++;
	print "reject: $a\n" if($opt_v);
    }

    if(/check_mail \(([^\)]*)\) rejection: 451.*/ ||
       /check_mail, arg1=(.*), relay=(.*), reject=451.*/ ) {
	$a = $1;
	$a = "<$a>" if(! $a =~ /^<.*>$/);
	$r = $opt_r ? $2 : "";
	$nodns{"$a /// $r"}++;
	print "no DNS: $a\n" if($opt_v);
    }

    if(/check_rcpt \(([^\)]*)\) rejection: 551.*we do not relay/ ||
       /check_mail, arg1=(.*), relay=(.*), reject=551.*we do not relay/ ) {
	$a = $1;
	$a = "<$a>" if(! $a =~ /^<.*>$/);
	$r = $opt_r ? $2 : "";
	$relay{"$a /// $r"}++;
	print "relay : $a\n" if($opt_v);
    }
}

# Report
print "sendmail reject report: $first_date -- $last_date\n";

if(scalar(%reject)) {
    print
	"\nAddresses rejected using blacklist:\n",
	"-----------------------------------\n";
    for $k (sort { $reject{$b} <=> $reject{$a} } keys(%reject)) {
	($a, $r) = split(" /// ", $k);
	printf "%5d", $reject{$k};
	print " $a\n";
	print "                relay: $r\n" if($opt_r);
    }
}

if(scalar(%nodns)) {
    print
	"\nAddresses without valid DNS entry:\n",
	"----------------------------------\n";
    for $k (sort { $nodns{$b} <=> $nodns{$a} } keys(%nodns)) {
	($a, $r) = split(" /// ", $k);
	printf "%5d", $nodns{$k};
	print " $a\n";
	print "                relay: $r\n" if($opt_r);
    }
}

if(scalar(%relay)) {
    print
	"\nAddresses from relay attempts:\n",
	"------------------------------\n";
    for $k (sort { $relay{$b} <=> $relay{$a} } keys(%relay)) {
	($a, $r) = split(" /// ", $k);
	printf "%5d", $relay{$k};
	print " $a\n";
	print "                relay: $r\n" if($opt_r);
    }
}


close(OUT) if($out_flag);
