#!/usr/bin/perl
#
# $Id: runtoss.pl,v 4.2 1998/03/08 21:07:39 mj Exp $
#
# Wrapper for ftntoss, ftnroute, ftnpack doing the toss process
#
# Usage: runtoss name
#    or  runtoss /path/dir
#

$VERSION = '$Revision: 4.2 $ ';
$PROGRAM = "runtoss";


# Common configuration for perl scripts 
<INCLUDE config.pl>

require "getopts.pl";
&Getopts('vc:');

# read config
$CONFIG      = $opt_c ? $opt_c : "<CONFIG_GATE>";
&CONFIG_read($CONFIG);

# additional arguments for ftntoss/route/pack
$ARGS  = "";
$ARGS .= " -c $opt_c" if($opt_c);
$ARGS .= " -v"        if($opt_v);


$PRG        = &CONFIG_get("libdir");
$SPOOL      = &CONFIG_get("spooldir");
$OUTBOUND   = &CONFIG_get("btbasedir");
$INBOUND    = &CONFIG_get("inbound");
$PINBOUND   = &CONFIG_get("pinbound");
$UUINBOUND  = &CONFIG_get("uuinbound");
$FTPINBOUND = &CONFIG_get("ftpinbound");
$LOGFILE    = &CONFIG_get("logfile");

# Minimum free disk space required for tossing
$MINFREE    = &CONFIG_get("diskfreemin");
$MINFREE    = &CONFIG_get("mindiskfree") if(!$MINFREE);
$MINFREE    = 10000                      if(!$MINFREE);
# Method for determining free disk space
#   prog = use DiskFreeProg
#   none = always assume enough free disk space
$DFMETHOD   = &CONFIG_get("diskfreemethod");
$DFMETHOD   = "prog" if(!$DFMETHOD);		# "prog" = Use DiskFreeProg
# df program, must behave like BSD df accepting path name
#   %p   = path name
$DFPROG     = &CONFIG_get("diskfreeprog");
$DFPROG     = "df -P %p" if(!$DFPROG);		# GNU fileutils df


if($LOGFILE eq "syslog") {
    $LOGFILE = "stdout";
    &log("rununpack WARNING: syslog output not supported, using stdout");
}


if($#ARGV != 0) {
    die "usage: $PROGRAM NAME\n";
}
$NAME = $ARGV[0];

# Set input and grade depending on NAME
if   ( $NAME eq "pin"  ) {
    $INPUT=$PINBOUND;
    $FADIR="-F$INPUT";
    $GRADE="-gp";
    $FLAGS="-s";
}
elsif( $NAME eq "in"   ) {
    $INPUT=$INBOUND;
    $FADIR="-F$INPUT";
    $GRADE="-gi";
    $FLAGS="-s";
}
elsif( $NAME eq "uuin" ) {
    $INPUT=$UUINBOUND;
    $FADIR="";
    $GRADE="-gu";
    $FLAGS="-s";
}
elsif( $NAME eq "ftpin") {
    $INPUT=$FTPINBOUND;
    $FADIR="";
    $GRADE="-gf";
    $FLAGS="-s";
}
elsif( $NAME eq "outpkt") {
    $INPUT="$SPOOL/outpkt";
    $FADIR="";
    $GRADE="";
    $FLAGS="-n -t -p";
}
elsif( $NAME eq "outpkt/mail") {
    $INPUT="$SPOOL/outpkt/mail";
    $FADIR="";
    $GRADE="-gm";
    $FLAGS="-n -t -p";
}
elsif( $NAME eq "outpkt/news") {
    $INPUT="$SPOOL/outpkt/news";
    $FADIR="";
    $GRADE="-gn";
    $FLAGS="-n -t -p";
}
elsif( $NAME =~ /^\/.+/ || $NAME =~ /^\.\/.+/ ) {
    $INPUT=$NAME;
    $FADIR="-F$INPUT";
    $GRADE="";
    $FLAGS="-s";
}
else {
    die "$PROGRAM: unknown $NAME\n";
}

(-d $INPUT) || die "$PROGRAM: $INPUT: no such directory\n";



##### Log message ############################################################

@month = ('Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun',
	  'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec' );

sub log {
    local(@text) = @_;
    local(*F, @x);
    
    print "$PROGRAM @text\n" if($opt_v);

    # Write to log file
    if($LOGFILE eq "stdout") {
	open(F, ">&STDOUT") || die "$PROGRAM: can't open log $LOGFILE\n";
    }
    elsif($LOGFILE eq "stderr") {
	open(F, ">&STDERR") || die "$PROGRAM: can't open log $LOGFILE\n";
    }
    else {
	open(F, ">>$LOGFILE") || die "$PROGRAM: can't open log $LOGFILE\n";
    }
    
    @x = localtime;
    printf
	F "%s %02d %02d:%02d:%02d ",
	$month[$x[4]], $x[3], $x[2], $x[1], $x[0]; 
    print F "$PROGRAM @text\n" if($opt_v);

    close(F);
}



##### Run df program #########################################################

sub df_prog {
    local($path) = @_;
    local(@args, *P, @f, $free);

    # df command, %p is replaced with path name
    @args = split(' ', $DFPROG);
    for($i=0; $i<=$#args; $i++) {
	$args[$i] =~ s/%p/$path/g;
    }

    print "Running @args\n" if($opt_v);

    # ignore SIGPIPE
    $SIG{"PIPE"} = "IGNORE";

    # Safe pipe to df
    $pid = open(P, "-|");
    if ($pid) {   # parent
	while (<P>) {
	    chop;
	    next if(! /^\/dev\// );
	    @f = split(' ', $_);
	    $free = $f[3];
	}
	close(P);
    }
    else {        # child
	exec (@args) || die "$PROGRAM: can't exec df program: $!\n";
	die "$PROGRAM: impossible return from exec\n";
	# NOTREACHED
    }

    die "$PROGRAM: can't determine free disk space\n" if($free eq "");
    return $free;
}



##### Check for free disk space ##############################################

sub df_check {
    local($path) = @_;
    local($free);

    # Use method
    if($DFMETHOD eq "prog") {
	$free = &df_prog($path);
    }
    elsif($DFMETHOD eq "none") {
	return 1;
    }
    else {
	return 1;
    }

    # Check against DiskFreeMin
    print "Free disk space in $path is $free K\n" if($opt_v);
    if($free < $MINFREE) {
	&log("disk space low in $path, $free K free");
	return 0;
    }
    return 1;
}



##### Run program ############################################################

$status = 0;				# Global status of last &run_prog

sub run_prog {
    local($cmd) = @_;
    local(@args, $rc);

    @args = split(' ', $cmd);
    print "Running @args\n" if($opt_v);
    $rc = system { "$PRG/$args[0]" } @args;
    $status = $rc >> 8;
    print "Status $status\n" if($opt_v);

    return $status == 0;
}



##### Main ###################################################################

# Check free disk space in SPOOL
&df_check($SPOOL) || exit 1;

# Run ftntoss/ftnroute/ftnpack
$flag = 1;

while($flag) {
    # Check free disk space in outbound (BTBASEDIR)
    &df_check($OUTBOUND) || exit 1;

    # Toss
    &run_prog("ftntoss -x -I $INPUT $GRADE $FLAGS $ARGS");
    if($status == 0) {		# Normal exit
	$flag = 0;
    }
    elsif($status == 2) {	# MSGID history or lock file busy
	sleep(60);
	$flag = 1;
    }
    elsif($status == 3) {	# Continue tossing
	$flag = 1;
    }
    else {			# Error
	die "$PROGRAM: ftntoss returned $status\n";
    }

    # Route
    &run_prog("ftnroute $GRADE $ARGS");
    if($status != 0) {		# Error
	die "$PROGRAM: ftnroute returned $status\n";
    }

    # Pack
    &run_prog("ftnpack $FADIR $GRADE $ARGS");
    if($status != 0) {		# Error
	die "$PROGRAM: ftnroute returned $status\n";
    }
}

exit 0;
