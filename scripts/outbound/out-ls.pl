#!/usr/local/bin/perl
#
# $Id: out-ls.pl,v 4.2 2000/10/17 21:04:35 mj Exp $
#
# This script lists the outbound of BinkleyTerm. It is currently tailored
# to a UNIX system serving a DOS NFS client. DOS filenames are translated
# to the corresponding filename on the server if possible.
#

$OUTBOUND = "<BTBASEDIR>";


require "getopts.pl";

&Getopts('lsaptB:mx');

if($opt_B) {
    $OUTBOUND = $opt_B;
}


my %dirs;

%dirs = (
    'c:', 'c:',
    'd:', 'd:',
    'e:', 'e:',
    'f:', 'f:',
    'g:', 'g:',
    'h:', '/home',
    'i:', '/var/spool',
    'p:', '/pub',
    'q:', '/pub',
);


sub file2addr {
    local($zone, $name) = @_;

    $net   = hex(substr($name,  0, 4));
    $node  = hex(substr($name,  4, 4));
    $point = hex(substr($name, 17, 4));

    if($point != 0) {
	return "$zone:$net/$node.$point";
    }
    else {
	return "$zone:$net/$node";
    }
}



sub do_file {
    local($zone,$dir,$file) = @_;

    local($flavor, $isflo, $isout, $addr, $t, $s, $n);

    if($file =~ /pnt/) {
	$flavor = substr($file, 22, 3);
    }
    else {
	$flavor = substr($file, 9, 3);
    }
    $flavor =~ tr/a-z/A-Z/;
    $isflo  =  $file =~ /\.[fhdc]lo$/;
    $isout  =  $file =~ /\.[ohdc]ut$/;
    $addr   =  &file2addr($zone, $file);

    print $flavor, " ";
    if($opt_a) {
	printf "%-45s(%s)\n", $addr, $file;
    }
    else {
	print $addr, "\n";
    }

    if($opt_l && $isflo) {
	open(FLO, "$dir/$file")      || die "Can't open $dir/$file";
	$s = 0;
	$n = 0;
	while(<FLO>) {
	    s/\cM?\cJ$//;
	    next if( /^;/ );
	    $s += &print_flo_entry( $dir, $_ );
	    $n++;
	}
	print "    ", &ksize($s), "\n" if $n>1;
    }
    if(($opt_s || $opt_t) && $isout) {
	($s, $t) = &size_time("$dir/$file");
	print "    ";
	print &ksize($s),   "  "  if $opt_s;
	print &asctime($t), "  "  if $opt_t;
	print "\n";
    }
}



sub print_flo_entry {
    local($dir, $line) = @_;

    local($type, $drive, $file, $short, $t, $s);

    $type  = substr($line, 0, 1);
    if($type =~ /[#~^]/ ) {
	$line = substr($line, 1, length($line)-1);
    }
    else {
	$type = " ";
    }
    if(! $opt_m) {
	$line  =~ tr/[A-Z\\]/[a-z\/]/;
	$drive =  substr($line, 0, 2);
	$file  =  substr($line, 2, length($line)-2);
	$file  =  $dirs{$drive}.$file;
    }
    else {
	$file  = $line;
    }
    $short =  $file;
    $short =~ s+^$dir/++;

    ($s, $t) = &size_time($file);

    print "    ";
    print &ksize($s),   "  "  if $opt_s;
    print &asctime($t), "  "  if $opt_t;
    print $type, " ", $short, "\n";

    return $s;
}



sub asctime {
    local($time) = @_;

    if($time eq "") {
	return "              ";
    }
    else {
	local($yr, $mn, $dy, $h, $m, $s, $xx);

	($s,$m,$h,$dy,$mn,$yr,$xx,$xx,$xx) = localtime($time);

	return sprintf("%02d.%02d.%02d %02d:%02d", $dy,$mn+1,$yr, $h,$m);
    }
}



sub size_time {
    local($file) = @_;

    local($t, $s, $d);

    ($d,$d,$d,$d,$d,$d,$d, $s, $d, $t) = stat($file);

    return ($s, $t);
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



sub do_point_dir {
    local($zone, $dir, $pdir) = @_;

    opendir(DIR, "$dir/$pdir")       || die "Can't open $dir/$pdir";
    local(@files) = readdir(DIR);
    closedir(DIR);
    @files = sort(@files);

    for(@files) {
	if( /^0000[0-9a-f]{4}\.([fhdc]lo|[ohdc]ut|bsy)$/ ) {
	    &do_file($zone,$dir,"$pdir/$_");
	}
    }
}



sub do_dir {
    local($zone,$dir) = @_;

    opendir(DIR, $dir)               || die "Can't open $dir";
    local(@files) = readdir(DIR);
    closedir(DIR);
    @files = sort(@files);

    for(@files) {
	if( /^[0-9a-f]{8}\.([fhdc]lo|[ohdc]ut|bsy|\$\$.)$/ ) {
	    &do_file($zone,$dir,$_);
	}
	if( !$opt_p && /^[0-9a-f]{8}\.pnt$/ ) {
	    &do_point_dir($zone,$dir,$_);
	}
    }
}



&do_dir(2,   "$OUTBOUND/out");
&do_dir(242, "$OUTBOUND/out.0f2");
