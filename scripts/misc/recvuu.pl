#!/usr/local/bin/perl
#
# $Id: recvuu.pl,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Process UUENCODEd arcmail packet mails in stdin
#

$UUINBOUND = "<UUINBOUND>";
$UUDECODE  = "/usr/bin/uudecode";
$PERM      = "666";


chdir($UUINBOUND) || die "recvuu: can't cd to $UUINBOUND\n";

open(UUD, "|$UUDECODE") || die "recvuu: can't open pipe to $UUDECODE\n";

$flag = 0;

while(<>) {
    if( /^begin \d\d\d (.*)$/ ) {
	$name = $1;
	$name =~ s/.*\///;		# Remove path
	$name = "$$.$name";		# Add PID
	$flag = 1;
	print UUD "begin $PERM $name\n";
	next;
    }
    
    print UUD if($flag);    
    
    if( /^end/ ) {
    	$flag = 0;
    }
}

close(UUD);
die "recvuu: ERROR closing pipe to $UUDECODE\n" if($?);

exit(0);
