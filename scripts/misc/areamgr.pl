#!/usr/local/bin/perl
#
# $Id: areamgr.pl,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Do various things with AREAS.BBS file
#

##### Configure me! ##########################################################

$LOG   	 = "<LOGDIR>/log-out";
$AREAS 	 = "/u1/ftn/cfg/areas.bbs";
$ACTIVE	 = "/usr/local/lib/news/active";
  
$NEW   	 = "areas.new";
$DEL   	 = "areas.del";
$U_NODES = "242:4900/99 242:2000/1 242:3000/1";
$F_NODES = "2:242/42 1:105/42 2:2454/91";

##### Configure me! ##########################################################



if($#ARGV == -1) {
    print STDERR
"
----- FIDOGATE areas.bbs maintenance utility -----------------------------

usage: areamgr [-c LIST] [-d] [-i]

         -c LIST    create new Usenet areas from list
         -d         write deleted areas (no longer in active) to areas.del
         -n         write new areas (logged as unknown) to areas.new
         -i         interactive

";
    exit 1
}

require "flush.pl";
require "getopts.pl";

&Getopts('c:dni');



##### Main ###################################################################

print "Reading active\n";
open(A, "$ACTIVE") || die "can't open $ACTIVE\n";
while(<A>) {
    ($ng,$x,$x,$x) = split(' ', $_);
    $ng =~ tr/[a-z-]/[A-Z_]/;
    $areas_active{$ng} = 1;
}
close(A);

print "Reading areas.bbs\n";
open(A, "$AREAS") || die "can't open $AREAS\n";
$_ = <A>;
while(<A>) {
    ($dir,$area,$nodes) = split(' ', $_, 3);
    $areas_bbs{$area} = $nodes;
}
close(A);

print "Reading log file\n";
open(L, "$LOG") || die "can't open $LOG\n";
while(<L>) {
    chop;
    if( /unknown area (.+) from ([0-9:\/.]+)/ ) {
	$area = $1;
	$node = $2;
	if(! $areas_bbs{$area}) {
	    $areas_unknown{$area} = $node;
	}
    }
}
close(L);



&new_areas    	       if($opt_n);
&delete_areas 	       if($opt_d);
&create_areas($opt_c)  if($opt_c);

exit 0;



##### Create new areas #######################################################

sub new_areas {
    open(A, ">$NEW") || die "can't open $NEW\n";

    for $area (sort keys %areas_unknown) {
	if($areas_active{$area}) {
	    printf "Create %s (%s)? [Ynk] ", $area, $areas_unknown{$area};
	    &flush(STDOUT);
	    
	    $yn = <STDIN>;
	    chop $yn;
	    
	    if($yn =~ /^([Yy].*|)$/) {
		print "Adding\n";
		printf A "#X %s %s\r\n",
		         $area, $U_NODES;
	    }
	}			# 
	else {
	    printf "Area %s (%s) not in active file\n",
	    $area, $areas_unknown{$area};
	}
    }
    
    close(A);
    print "** New areas written to areas.new\n";
}



##### Look for deleted areas #################################################

sub delete_areas {
    open(A, ">$DEL") || die "can't open $DEL\n";

    for $area (sort keys %areas_bbs) {
	if($areas_bbs{$area} =~ /^242:/) {
	    if(! $areas_active{$area}) {
		printf "Area %s not in active file\n", $area;
		printf A "#X %s\r\n", $area;
	    }
	}
    }

    close(A);
    print "** Deleted areas written to areas.del\n";
}



##### Create new areas from list #############################################

sub create_areas {
    local($file) = @_;

    open(A, ">$NEW") || die "can't open $NEW\n";
    open(L, "$file") || die "can't open $file\n";

    while(<L>) {
	chop;
	($group) = split(' ', $_);
	$area  = $group;
	$area  =~ tr/[a-z-]/[A-Z_]/;

	if(! $areas_bbs{$area}) {
	    printf A "#X %s %s\r\n", $area, $U_NODES;
	    printf "Area %s\n", $area;
	}
	else {
	    printf "Area %s already in areas.bbs\n", $area;
	}
    }

    close(L);
    close(A);
    print "** New areas written to areas.new\n";
}
