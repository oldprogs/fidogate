#!/usr/bin/perl
#
# Automatically search and apply FIDO nodediffs to nodelist
#
# Requires perl scripts
#     latest
#     nl-diff
#

$LIBDIR="<LIBDIR>";

##FIXME: retrieve from fareas.bbs##
$DIFFDIR = "/pub/fido/files/nodelist";
$NAME    = "nodelist";

@ARCX    = ("arc",   "xo");
@LHAX    = ("lha",   "xf");
@ZIPX    = ("unzip", "-joL");


require "getopts.pl";
&Getopts('v');

$dir  = $DIFFDIR;
$name = $NAME;


##### Run program, exit if it fails #####
sub run {
    print "Running @_\n" if($opt_v);
    $st = (system @_) >> 8;
    if($st) {
	print STDERR "nl-autoupd: running @_ failed (exit code=$st)\n";
	exit $st;
    }
}




# Search for current nodelist
$uc = $name;
$uc =~ tr/a-z/A-Z/;
$nodelist = `/home/mj/src/fidogate/scripts/nodelist/latest $name.??? $uc.???`;
chop $nodelist;
die "nl-autoupd: can't find current nodelist\n" if(!$nodelist);

print "Current nodelist: $nodelist\n" if($opt_v);


# Parse filename
if($nodelist =~ /^(.+)\.(\d\d\d)/ ) {
    $basename=$1;
    $day=$2;
    print "Current nodelist: basename=$basename day=$day\n" if($opt_v);
}
else {
    die "nl-autoupd: strange nodelist filename $nodelist\n";
}

# Search for nodediffs
$diff = $basename;
$diff =~ tr/A-Z/a-z/;
$diff =~ s/list/diff/;
$uc   = $diff;
$uc   =~ tr/a-z/A-Z/;

while(1) {
    $found = 0;

    $day += 7;
    ##FIXME: wrap-around for next year, take into account leap years##
    $dd  = $day % 100;
    $dd  = "0"."$dd" if($dd < 10);
    $ddd = $day;
    $ddd = "0"."$ddd" if($ddd < 100);
    $ddd = "0"."$ddd" if($ddd < 10);

    # ARC
    if(!$found) {
	$file = "$dir/$diff.a$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @ARCX;
	}
    }
    if(!$found) {
	$file = "$dir/$uc.A$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @ARCX;
	}
    }
    # LHA
    if(!$found) {
	$file = "$dir/$diff.l$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @LHAX;
	}
    }
    if(!$found) {
	$file = "$dir/$uc.L$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @LHAX;
	}
    }
    # ZIP
    if(!$found) {
	$file = "$dir/$diff.z$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @ZIPX;
	}
    }
    if(!$found) {
	$file = "$dir/$uc.Z$dd";
	if(-f $file) {
	    $found = 1;
	    @extract = @ZIPX;
	}
    }

    # Nothing found
    if(!$found) {
	print "No more nodediff files found, exiting\n" if($opt_v);
	exit 0;
    }
    print "Nodediff: $file, extract=@extract\n" if($opt_v);
	
    # Extract
    push @extract, $file;
    &run(@extract);

    # Find extracted file
    $found = 0;
    if(!$found) {
	$found = -f ($file = "$diff.$ddd");
    }
    if(!$found) {
	$found = -f ($file = "$uc.$ddd");
    }
    die "nl-autoupd: strange, can't find extracted file\n" if(!$found);
    print "Nodediff: $file\n" if($opt_v);

    # Run nl-diff
    @nldiff = ("nl-diff", "-r", "-b$basename");
    push @nldiff, "-v" if($opt_v);
    push @nldiff, $file;
    &run(@nldiff);
    unlink $file;

}
    
