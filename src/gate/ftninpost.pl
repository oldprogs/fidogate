#!/usr/local/bin/perl
#
# $Id: ftninpost.pl,v 4.1 1996/10/18 16:58:05 mj Exp $
#
# Postprocessor for ftnin, feeds output of ftn2rfc to rnews and sendmail.
# Call via ftnin's -x option or run after ftn2rfc. Replaces old fidorun
# script.
#

require "getopts.pl";
&Getopts('vL:S:I:');

# defaults
$LIBDIR   = "<LIBDIR>";
$SPOOLDIR = "<SPOOLDIR>";
$INDIR    = "<SPOOLDIR>/in";


# options
$options = "";
if($opt_L) {
    $LIBDIR   = $opt_L;
    $options  = "$options -L$LIBDIR";
}
if($opt_S) {
    $SPOOLDIR = $opt_S;
    $options  = "$options -S$SPOOLDIR";
}
$INDIR    = $opt_I if($opt_I);
if($opt_v) {
    $options  = "$options -v";
}

# get config.gate parameters
$SENDMAIL = `$LIBDIR/ftnconfig -c $LIBDIR/config.gate -l FTNInSendmail`;
$RNEWS    = `$LIBDIR/ftnconfig -c $LIBDIR/config.gate -l FTNInRnews`;
$RECOMB   = `$LIBDIR/ftnconfig -c $LIBDIR/config.gate -l -t FTNInRecombine`;

print
    "sendmail  = $SENDMAIL\n",
    "rnews     = $RNEWS\n",
    "recombine = $RECOMB\n"
    if($opt_v);
exit;


# ----- main -----------------------------------------------------------------

# do recombining of split messages
if($RECOMB) {
    &do_cmd("$LIBDIR/ftninrecomb $options");
}

# mail
$dir = "$INDIR/mail";

opendir(DIR, "$dir") || die "ftninpost: can't open $dir\n";
@files = grep(/\.msg$/, readdir(DIR));
closedir(DIR);

for $f (sort @files) {
    &do_file($dir, $f);
}

# news
$dir = "$INDIR/news";

opendir(DIR, "$dir") || die "ftninpost: can't open $dir\n";
@files = grep(/\.msg$/, readdir(DIR));
closedir(DIR);

for $f (sort @files) {
    &do_file($dir, $f);
}



# ----- do_file() - process mail message or news batch -----------------------

sub do_file {
    local($dir, $f) = @_;
    local($file)    = "$dir/$f";
    local($bad)     = "$INDIR/bad/$f";

    $from = &get_sender($file);
    if($from eq "::NEWS::") {
	$cmd = "$RNEWS <$file";
    }
    else {
	$cmd = sprintf("$SENDMAIL <$file", $from);
    }

    print "CMD: $cmd\n" if($opt_v);

    if(&do_cmd($cmd) == 0) {
	print "SUCCESS\n" if($opt_v);
	unlink($file) ||
	    print STDERR "ftninpost: can't unlink $file\n";
    }
    else {
	print "ERROR\n" if($opt_v);
	rename($file, $bad) ||
	    print STDERR "ftninpost: can't move $file -> $bad\n";
    }
}



# ----- get_sender() - get envelope sender for mail --------------------------

sub get_sender {
    local($file) = @_;

    open(FILE, "$file") || die "ftninpost: can't open $file\n";
    $_ = <FILE>;
    close(FILE);

    if( /^From ([^ ]+) / ) {
	return "$1";
    }
    else {
	return "::NEWS::";
    }
}

    

# ----- do_cmd() - exec command ----------------------------------------------

sub do_cmd {
    local($cmd) = @_;

    return system($cmd) >> 8;
}
