#!/usr/bin/perl
#
# $Id: subst.pl,v 4.1 1996/09/28 08:13:51 mj Exp $
#
# Substitute directory names for FIDOGATE scripts
#

require "getopts.pl";
&Getopts('p:c:v');

$config = $opt_c ? $opt_c : "config.make";

# Read config.make
open(P, "$config") || die "can't open config.make\n";

undef %macros;

while(<P>) {
    chop;
    next if( /^\s\#/ );
    s/\#.*$//;

    if( /^([A-Z0-9_]+)\s*=\s*(.*)\s*$/ ) {
	print "subst: $1=$2\n" if($opt_v);
	$macros{$1}=$2;
    }
}
close(P);

# perl interpreter
if($opt_p){
    $_ = <>;
    print "#!$opt_p\n";
}

# Substitute
while(<>) {

    for $s (keys %macros) {
	$r = $macros{$s};
	s/<$s>/$r/g;
    }

    print;
}
