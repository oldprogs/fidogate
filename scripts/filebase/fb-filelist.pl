#!/usr/local/bin/perl
#
# $Id: fb-filelist.pl,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Generate file list
#
# Copyright (C) 1984
#  _____ _____
# |     |___  |   Martin Junius            FIDO:      2:242/6.1
# | | | |   | |   Republikplatz 3          Internet:  mj@sungate.fido.de
# |_|_|_|@home|   D-52072 Aachen, Germany  Phone:     ++49-241-86931 (voice)
#
# $Log: fb-filelist.pl,v $
# Revision 4.0  1996/04/17 18:17:38  mj
# Initial revision
#
# Revision 0.4  1995/01/26  22:01:42  mj
# Add missing files to FILES.BBS, some formatting changes.
#
# Revision 0.3  1994/01/16  12:51:36  mj
# Added area and listing total K, -n option for NEWFILES listing.
#
# Revision 0.2  1994/01/15  16:31:58  mj
# Add version info after epilog.
#
# Revision 0.1  1994/01/15  16:23:47  mj
# First test version.
#

##### Configuration ##########################################################

# Prolog/epilog
$PROLOG   = "/home/mj/mailbox/filelist/prolog.txt";
$EPILOG   = "/home/mj/mailbox/filelist/epilog.txt";

# Missing text
$MISSING  = "--- file description missing ---";

# Maximus file areas
$FILEAREA = "/u1/ftn/max/filearea.ctl";

# Path translation: MSDOS drives -> UNIX path names
%dirs = (
    'c:', 'c:',
    'd:', 'd:',
    'e:', 'e:',
    'f:', 'f:',
    'g:', 'g:',
    'h:', '/home',
    'i:', '/var/spool',
    'p:', '/u1',
    'q:', '/u2',
);


##############################################################################
require "getopts.pl";

&Getopts('n:');

if($opt_n) {
    # -n days
    $time_new = time() - $opt_n*60*60*24;
}

$total_global = 0;


# Generate listing for one directory

sub list_dir {
    local($dir) = @_;
    local($first,$file,$desc,@files,%files_dir);
    local($total) = 0;

    # Read contents of directory

    opendir(DIR, $dir)               || die "filelist: can't open $dir\n";
    @files = readdir(DIR);
    closedir(DIR);

    for $file (@files) {
	# ignore ., ..
	if( $file eq "." || $file eq ".." ) {
	    next;
	}
	# ignore files.{bbs,bak,dat,dmp,idx}
	if( $file =~ /^files\.(bbs|bak|dat|dmp|idx|tr)$/ ) {
	    next;
	}
	$files_dir{$file} = 1;
    }

    # Read and print contents of files.bbs

    open(FILES, "$dir/files.bbs")
	|| die "filelist: can't open $dir/files.bbs\n";
    while(<FILES>) {
	s/\cM?\cJ$//;
	# File entry
	if( /^[a-zA-Z0-9]/ ) {
	    ($file, $desc) = split(' ', $_, 2);
	    $file  =~ tr+A-Z\\+a-z/+;
	    $files_dir{$file} = 2;
	    $total += &list_file($dir, $file, $desc);
	}
	elsif( /^-:crlf/ ) {
	    next;
	}
	elsif(!$time_new) {
	    print $_,"\r\n";
	}
    }
    close(FILES);

    # Print files missing in files.bbs / add to files.bbs
    open(FILES, ">>$dir/files.bbs")
	|| die "filelist: can't open $dir/files.bbs\n";
    for $file (sort keys(%files_dir)) {
	if($files_dir{$file} == 1) {
	    $total += &list_file($dir, $file, $MISSING);
	    printf FILES "%-12s  %s\r\n", $file, $MISSING;
	}
    }
    close(FILES);

    $total_global += $total;
    print "\r\n    File area total:   ",&ksize($total),"\r\n";

    print "\r\n\r\n";

}



sub list_file {
    local($dir,$file,$desc) = @_;
    local($x,$size,$time);

    ($x,$x,$x,$x,$x,$x,$x, $size ,$x, $time ,$x,$x,$x) = stat("$dir/$file");

    if($time_new && $time<$time_new) {
	return 0;
    }

    printf "%-12s  %s %s  %s\r\n", $file, &asctime($time), &ksize($size),
                                   $desc;

    return $size;
}



sub asctime {
    local($time) = @_;

    if($time eq "") {
	return "        ";
    }
    else {
	local($yr, $mn, $dy, $h, $m, $s, $xx);

	($s,$m,$h,$dy,$mn,$yr,$xx,$xx,$xx) = localtime($time);

	return sprintf("%02d.%02d.%02d", $dy,$mn+1,$yr, $h,$m);
    }
}



sub ksize{
    local($size) = @_;

    local($k);

    if($size eq "") {
	return "   N/A";
    }
    else {
	if($size == 0) {
	    $k = 0;
	}
	elsif($size <= 1024) {
	    $k = 1;
	}
	else {
	    $k = $size / 1024;
	}
	return sprintf("%5dK", $k);
    }
}



sub dump_file {
    local($file) = @_;

    open(F, "$file") || die "filelist: can't open $file\n";
    while(<F>) {
	s/\cM?\cJ$//;
	next if( /^\cZ/ );
	print $_,"\r\n";
    }
    close(F);
}



##### Main #####

if(-f $PROLOG) {
    &dump_file($PROLOG);
}

if($opt_n) {
    print "******** Listing of all files newer than ";
    print $opt_n, " days ******************************\r\n\r\n";
}


# Read Maximus filearea.ctl

open(F, "$FILEAREA") || die "filelist: can't open $FILEAREA\n";

while(<F>) {
    s/\cM?\cJ$//;
    s/^\s*//;
    s/\s*$//;
    next if( /^%/ );
    next if( /^$/ );

    ($keyw,$args) = split(' ', $_, 2);
    $keyw =~ tr/[A-Z]/[a-z]/;

    if   ($keyw eq "area"    ) {
	$area = $args;
    }
    elsif($keyw eq "fileinfo") {
	$info = $args;
    }
    elsif($keyw eq "download") {
	$dir  =  $args;
	$dir  =~ tr/[A-Z\\]/[a-z\/]/;
	$drv  =  substr($dir, 0, 2);
	$dir  =  substr($dir, 2, length($dir)-2);
	$dir  =  $dirs{$drv}.$dir;
    }
    elsif($keyw eq "end"     ) {
	$args =~ tr/[A-Z]/[a-z]/;
	if($args eq "area") {
	    $length = 50;
	    if(length($info) > $length) {
		$info = substr($info, 0, $length);
	    }
	    $length -= length($info);
	    printf "### MAX file area \#%-3d ### ", $area;
	    print $info, " ", "#" x $length, "\r\n";
	    print "\r\n";
	    &list_dir($dir);
	}
    }
}

close(F);

print "########################################",
      "######################################\r\n";
print "\r\n      Listing total:  ",&ksize($total_global),"\r\n\r\n";
print "########################################",
      "######################################\r\n";

print "\r\n";
print "This list generated by:\r\n";
print '        $Id: fb-filelist.pl,v 4.0 1996/04/17 18:17:38 mj Exp $', "\r\n";

if(-f $EPILOG) {
    &dump_file($EPILOG);
}
