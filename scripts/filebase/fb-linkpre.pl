#!/usr/bin/perl
#
# Translate long UNIX file name to MSDOS file names creating
# FILES.TR translation list for symlinks
#

$NAME = "fb-linkpre";
$FTR  = "files.tr";


require "getopts.pl";

&Getopts('v');

if($#ARGV != 0) {
    print STDERR
	"\nFile base maintenance: create symlinks for DOS 8+3 filenames\n\n",
	"usage:   fb-linkfiles DIR\n\n";
    exit 1;
}


$dir = $ARGV[0];
$cmd = "find $dir -type f -print";



##### read files.tr ##########################################################

if( open(TR, "$FTR") ) {
    while(<TR>) {
	chop;
	next if( /^\#/ );
	($dos, $name) = split(' ', $_);
	$trans{$name} = $dos;
    }
    close(TR);
}


##### read files in directory ################################################

open(PIPE, "$cmd|") || die "$NAME: ERROR: open pipe to $cmd\n";
@files = <PIPE>;
close(PIPE);
die "$NAME: ERROR: close pipe to $cmd\n" if($?);
chop @files;

if($opt_v) {
    $" = "\n    ";		# "
    print "Files in $dir:\n    @files\n";
}


##### process files, translation #############################################

for $_ (@files) {
    # skip certain files: *.bak, *~
    next if( /\.bak$/ || /~$/ );

    $orig = $_;

    if($trans{$orig}) {
	$new = $trans{$orig};
    }
    else {
	$new  = $_;
	$new =~ s/^.*\///;
	
	# do some translations on our own
	$new =~ s/\.tar.gz/.tgz/;
	$new =~ s/\.tar.z/.tgz/;
	$new =~ s/\.tar.Z/.taz/;
#	$new =~ s/\.ps.gz/.pgz/;
#	$new =~ s/\.doc.gz/.dgz/;
	
	$name = $new;
	$ext  = "";
	if( $new =~ /^(.*)\.([^.]*)$/ ) {
	    $name = $1;
	    $ext  = $2;
	}
	$name =~ tr/A-Z/a-z/;
	$ext  =~ tr/A-Z/a-z/;
	$name =~ s/[^a-z0-9]//g;
	$ext  =~ s/[^a-z0-9]//g;
	$new = $ext ? "$name.$ext" : $name;

	$trans{$orig} = $new;

#	print "$orig -> $new\n";
    }	
}


##### write new files.tr #####################################################

print
    "# DOS---------        ",
    "ORIG---------------------------------------------------\n";

for $f (sort keys(%trans)) {
    $dos = $trans{$f};
    $ok  = "*";
    # check for DOS limits
    $ok = " " if( $dos =~ /^[a-z0-9]{1,8}(\.[a-z0-9]{1,3})?$/ ); # o.k.
    printf "%s %-19s %s\n", $ok, $dos, $f;
}

exit(0);







