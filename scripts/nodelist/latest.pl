#!/usr/local/bin/perl
#
# Search latest (youngest) file from command line
# 

$t = -1;
$l = "";

for $f (@ARGV) {
    ($x,$x,$x,$x,$x,$x,$x,$x, $atime, $mtime, $ctime, $x,$x) = stat($f);
    if($mtime >= $t) {
	$t = $mtime;
	$l = $f;
    }
}

print $l,"\n";
