#!/usr/bin/perl
#
# $Id: mksubst.pl,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# Substitute directory names for FIDOGATE scripts
#

require "getopts.pl";
&Getopts('p:c:');

$config = $opt_c ? $opt_c : "config.make";

# Read config.make
open(P, "$config") || die "can't open config.make\n";

while(<P>) {
    chop;
    if( /^\s*BINDIR\s*=\s*(.*)\s*(\#|$)/ ) {
	$BINDIR = $1;
    }
    if( /^\s*LIBDIR\s*=\s*(.*)\s*(\#|$)/ ) {
	$LIBDIR = $1;
    }
    if( /^\s*SPOOLDIR\s*=\s*(.*)\s*(\#|$)/ ) {
	$SPOOLDIR = $1;
    }
    if( /^\s*LOGDIR\s*=\s*(.*)\s*(\#|$)/ ) {
	$LOGDIR = $1;
    }

    if( /^\s*OUTBOUND\s*=\s*(.*)\s*(\#|$)/ ) {
	$OUTBOUND = $1;
    }
    if( /^\s*INBOUND\s*=\s*(.*)\s*(\#|$)/ ) {
	$INBOUND = $1;
    }
    if( /^\s*PINBOUND\s*=\s*(.*)\s*(\#|$)/ ) {
	$PINBOUND = $1;
    }
    if( /^\s*UUINBOUND\s*=\s*(.*)\s*(\#|$)/ ) {
	$UUINBOUND = $1;
    }
}
close(P);

# perl
if($opt_p){
    $_ = <>;
    print "#!$opt_p\n";
}

# Substitute
while(<>) {
    s/<BINDIR>/$BINDIR/g;
    s/<LIBDIR>/$LIBDIR/g;
    s/<SPOOLDIR>/$SPOOLDIR/g;
    s/<LOGDIR>/$LOGDIR/g;

    s/<OUTBOUND>/$OUTBOUND/g;
    s/<INBOUND>/$INBOUND/g;
    s/<PINBOUND>/$PINBOUND/g;
    s/<UUINBOUND>/$UUINBOUND/g;

    print;
}
