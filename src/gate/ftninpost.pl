#!/usr/bin/perl
#
# $Id: ftninpost.pl,v 4.7 1998/01/24 14:07:31 mj Exp $
#
# Postprocessor for ftnin, feeds output of ftn2rfc to rnews and sendmail.
# Call via ftnin's -x option or run after ftn2rfc. Replaces old fidorun
# script.
#

require "getopts.pl";
&Getopts('vL:S:c:');

# defaults
$CONFIG      = $opt_c ? $opt_c : "<CONFIG_GATE>";
$LIBDIR      = $opt_L ? $opt_L : "<LIBDIR>";

# config.pl
require "$LIBDIR/config.pl";
&CONFIG_read($CONFIG);

# options
undef @options;
if($opt_c) {
    push @options, ("-c", $CONFIG);
}
if($opt_v) {
    push @options, "-v";
}
if($opt_L) {
    $CONFIG{"libdir"} = $opt_L;
    push @options, ("-L", $opt_L);
}
if($opt_S) {
    $CONFIG{"spooldir"} = $opt_S;
    push @options, ("-S", $opt_S);
}


# get gate.conf parameters
$SENDMAIL    = &CONFIG_get("FTNInSendmail");
$RNEWS       = &CONFIG_get("FTNInRnews");
$RECOMB      = &CONFIG_get("FTNInRecombine");

$OUTRFC_MAIL = &CONFIG_get("OUTRFC_MAIL");
$OUTRFC_NEWS = &CONFIG_get("OUTRFC_NEWS");

if(! $SENDMAIL) {
    print STDERR "ftninpost:$CONFIG:FTNInSendmail not specified\n";
    exit 1;
}
if(! $RNEWS) {
    print STDERR "ftninpost:$CONFIG:FTNInRnews not specified\n";
    exit 1;
}

print
    "sendmail  = $SENDMAIL\n",
    "rnews     = $RNEWS\n",
    "recombine = $RECOMB\n",
    "mail      = $OUTRFC_MAIL\n",
    "news      = $OUTRFC_NEWS\n"
    if($opt_v);


# command lists
@sendmail = split(' ', $SENDMAIL);
@rnews    = split(' ', $RNEWS);
# remove -f%s option from sendmail command if present
# (compatibility with old configurations)
$fidx = -1;
for($i=0; $i<=$#sendmail; $i++) {
    $fidx = $i if($sendmail[$i] eq "-f%s");
}
if($fidx > -1) {
    splice(@sendmail, $fidx, 1);
    print "sendmail  = @sendmail\n" if($opt_v);
}



# ----- main -----------------------------------------------------------------

# do recombining of split messages
if($RECOMB) {
    @cmd = (&CONFIG_expand($RECOMB));
    push(@cmd, @options);
    print "Running @cmd\n" if($opt_v);
    system @cmd;
}

# mail
$dir = $OUTRFC_MAIL;

opendir(DIR, "$dir") || die "ftninpost: can't open $dir\n";
@files = grep(/\.rfc$/, readdir(DIR));
closedir(DIR);

for $f (sort @files) {
    &do_file(1, "$dir/$f");
}

# news
$dir = $OUTRFC_NEWS;

opendir(DIR, "$dir") || die "ftninpost: can't open $dir\n";
@files = grep(/\.rfc$/, readdir(DIR));
closedir(DIR);

for $f (sort @files) {
    &do_file(0, "$dir/$f");
}



# ----- do_file() - process mail message or news batch -----------------------

sub do_file {
    local($mail, $file) = @_;
    local($ret, $bad);

    if($mail) {
	# Mail
	@cmd = @sendmail;
	$from = &get_sender($file);
	push(@cmd, "-f$from") if($from);
    }
    else {
	# News
	@cmd = @rnews;
    }

    print "CMD: @cmd" if($opt_v);

    # Save STDIN, open $file as new STDIN
    open(SAVE, "<&STDIN") || die "ftninpost: can't save STDIN\n";
    open(STDIN, "$file") || die "ftninpost: can't open STDIN with $file\n";

    # Run
    $ret = system(@cmd) >> 8;

    # Restore STDIN
    close(STDIN);
    open(STDIN, "<&SAVE") || die "ftninpost: can't restore STDIN\n";

    if($ret == 0) {
	print " - SUCCESS\n" if($opt_v);
	unlink($file) || die "ftninpost: can't unlink $file\n";
    }
    else {
	print " - ERROR\n" if($opt_v);
	$bad = $file;
	$bad =~ s/\.rfc$/.bad/;
	rename($file, $bad) || die "ftninpost: can't move $file -> $bad\n";
    }
}



# ----- get_sender() - get envelope sender for mail --------------------------

sub get_sender {
    local($file) = @_;

    open(FILE, "$file") || die "ftninpost: can't open $file\n";
    $_ = <FILE>;
    close(FILE);

    if( /^From ([^ ]+) / ) {
	return $1;
    }
    else {
	return "";
    }
}
