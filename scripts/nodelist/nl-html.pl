#!/usr/bin/perl
#:ts=8
#
# $Id: nl-html.pl,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# nl-html --- convert nodelist to HTML table
#

require "getopts.pl";
&Getopts('b:t:x:');


$zone  = 2;
$net   = 0;
$node  = 0;

$title = $opt_t ? $opt_t : "Nodelist";


# Header
print "<HTML>\n<HEAD>\n<TITLE>$title</TITLE>\n</HEAD>\n\n";
print $opt_b ? "<BODY BACKGROUND=\"$opt_b\">\n\n" : "<BODY>\n\n";

print "<H1>$title</H1>\n\n";

print "$opt_x\n<P>\n\n" if($opt_x);

    
print "<TABLE BORDER=1 CELLPADDING=5>\n";

print "<TR><TD><B>Node</B><TD><B>Name</B><TD><B>Phone</B>\n";
print "<TR><TD><TD><B>Location</B><TD><B>FTN Flags</B>\n";
print "<TR><TD><TD><B>Sysop</B><TD><B>User Flags</B>\n";

while(<>) {
    if ( /^;/ || /^\cZ/ ) {
	# Output comments as is
	next;
    }

    # Split input line
    chop;
    ($key,$num,$name,$loc,$sysop,$phone,$bps,$flags) =
	split(',', $_, 8);

    if($key eq "Zone") {	# Zone line
	$zone = $num;
	$net  = $num;
	$node = 0;
	$hili = 2;
    }
    elsif($key eq "Region") {	# Region line
	$net  = $num;
	$node = 0;
	$hili = 2;
    }
    elsif($key eq "Host") {	# Host line
	$net = $num;
	$node = 0;
	$hili = 2;
    }
    elsif($key eq "Hub") {	# Hub line
	$node = $num;
	$hili = 2;
    }
    else {
	$node = $num;
	$hili = $key ? 1 : 0;
    }

    print "<TR><TD><TD><TD>\n" if($hili == 2);
    $hkey = $key;
    $hkey = "<I>$key</I>" if($hili == 1);
    $hkey = "<B>$key</B>" if($hili == 2);

    $addr = "$zone:$net/$node";

    $flags =~ s/,U[^,]/,U,/;
    if( $flags =~ /^(.*),U,(.*)$/ ) {
	$fflags = $1;
	$uflags = $2;
    }
    else {
	$fflags = $flags;
	$uflags = "";
    }
    $fflags =~ s/,/ /g;
    $uflags =~ s/,/ /g;
    $name   =~ s/_/ /g;
    $loc    =~ s/_/ /g;
    $sysop  =~ s/_/ /g;

    print "<TR><TD>$addr<TD>$name<TD>+$phone\n";
    print "<TR><TD>$hkey<TD>$loc<TD>$fflags\n";
    print "<TR><TD><TD>$sysop<TD>$uflags\n\n";
}

# Trailer
print "</TABLE>\n";
print "\n</BODY>\n</HTML>\n";
