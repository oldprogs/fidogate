#!/usr/bin/perl
#
# $Id: logcheck.pl,v 4.8 1999/11/21 17:25:32 mj Exp $
#
# Create report for sendmail check_mail rules
#

use strict;
use vars qw($opt_g $opt_s $opt_n $opt_m $opt_v $opt_r);
use Getopt::Std;
use FileHandle;


my $NEWSGROUPS = "fido.de.lists";
my $SUBJECT    = "Fido.DE Sendmail Reject Report";

my $INEWS      = "/usr/bin/inews -h -S";
my $SENDMAIL   = "/usr/sbin/sendmail";

my $out_flag   = 0;


getopts('g:s:nm:vr');

if($opt_g) {
    $NEWSGROUPS = $opt_g;
}
if($opt_s) {
    $SUBJECT    = $opt_s;
}
if($opt_n) {
    open(OUT, "|$INEWS")
      || die "logreport: can't open pipe to inews\n";
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


my $first_date;
my $last_date;
my $addr;
my $r;
my $k;
my %rbl_rss;
my %rbl_dul;
my %rbl_rbl;
my %reject;
my %nodns;
my %relay;


# Read sendmail log
while(<>) {
    chop;

    if( /^(... .\d \d\d:\d\d:\d\d) / ) {
	$first_date = $1 if(!$first_date);
	$last_date  = $1;
    }

    if( /ruleset=check_relay, arg1=(.*), arg2=(.*), relay=(.*), reject=553 Rejected -/ ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $3;
	$rbl_rbl{"$addr /// $r"}++;
	print "rbl rbl: $addr\n" if($opt_v);
    }
    elsif( /ruleset=check_relay, arg1=(.*), arg2=(.*), relay=(.*), reject=553 Dialup -/ ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $3;
	$rbl_dul{"$addr /// $r"}++;
	print "rbl dul: $addr\n" if($opt_v);
    }
    elsif( /ruleset=check_relay, arg1=(.*), arg2=(.*), relay=(.*), reject=553 Open spam relay -/ ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $3;
	$rbl_rss{"$addr /// $r"}++;
	print "rbl rss: $addr\n" if($opt_v);
    }
    elsif( /ruleset=check_mail \(([^\)]*)\) rejection: 551/ ||
        /ruleset=check_mail, arg1=(.*), relay=(.*), reject=55\d/ ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $opt_r ? $2 : "";
	$reject{"$addr /// $r"}++;
	print "reject: $addr\n" if($opt_v);
    }
    elsif( /ruleset=check_mail \(([^\)]*)\) rejection: 451/ ||
	   /ruleset=check_mail, arg1=(.*), relay=(.*), reject=(451|501)/ ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $opt_r ? $2 : "";
	$nodns{"$addr /// $r"}++;
	print "no DNS: $addr\n" if($opt_v);
    }
    elsif( /ruleset=check_rcpt \(([^\)]*)\)()() rejection: 551/             ||
	   /ruleset=check_mail, arg1=(.*),() relay=(.*), reject=551/        ||
	   /ruleset=check_relay, arg1=(.*), arg2=(.*), relay=(.*), reject=550/ ||
	   /ruleset=check_rcpt, arg1=(.*),() relay=(.*), reject=5\d\d/     ) {
	$addr = $1;
	$addr = "<$addr>" if(! $addr =~ /^<.*>$/);
	$r = $opt_r ? $3 : "";
	$relay{"$addr /// $r"}++;
	print "relay : $addr\n" if($opt_v);
    }
    elsif(/check_/) {
	print "NOT MATCHED: $_\n" if($opt_v);
    }
}

# Report
print "sendmail reject report: $first_date -- $last_date\n";

if(scalar(%reject)) {
    print
	"\nLocal blacklist rejects:\n",
	"------------------------\n";
    for $k (sort { $reject{$b} <=> $reject{$a} } keys(%reject)) {
	($addr, $r) = split(" /// ", $k);
	printf "%5d", $reject{$k};
	print " $addr\n";
	print "                relay: $r\n" if($opt_r);
    }
}

if(scalar(%nodns)) {
    print
	"\nNo DNS rejects:\n",
	"---------------\n";
    for $k (sort { $nodns{$b} <=> $nodns{$a} } keys(%nodns)) {
	($addr, $r) = split(" /// ", $k);
	printf "%5d", $nodns{$k};
	print " $addr\n";
	print "                relay: $r\n" if($opt_r);
    }
}

if(scalar(%relay)) {
    print
	"\nRelay attempt rejects:\n",
	"----------------------\n";
    for $k (sort { $relay{$b} <=> $relay{$a} } keys(%relay)) {
	($addr, $r) = split(" /// ", $k);
	printf "%5d", $relay{$k};
	print " $addr\n";
	print "                relay: $r\n" if($opt_r);
    }
}

if(scalar(%rbl_rbl)) {
    print
	"\nMail-Abuse RBL rejects:\n",
	"-----------------------\n";
    for $k (sort { $rbl_rbl{$b} <=> $rbl_rbl{$a} } keys(%rbl_rbl)) {
	($addr, $r) = split(" /// ", $k);
	printf "%5d", $rbl_rbl{$k};
	print " relay: $r\n";
    }
}

if(scalar(%rbl_dul)) {
    print
	"\nMail-Abuse DUL rejects:\n",
	"-----------------------\n";
    for $k (sort { $rbl_dul{$b} <=> $rbl_dul{$a} } keys(%rbl_dul)) {
	($a, $r) = split(" /// ", $k);
	printf "%5d", $rbl_dul{$k};
	print " relay: $r\n";
    }
}

if(scalar(%rbl_rss)) {
    print
	"\nMail-Abuse RSS rejects:\n",
	"-----------------------\n";
    for $k (sort { $rbl_rss{$b} <=> $rbl_rss{$a} } keys(%rbl_rss)) {
	($addr, $r) = split(" /// ", $k);
	printf "%5d", $rbl_rss{$k};
	print " relay: $r\n";
    }
}


close(OUT) if($out_flag);
