#!/usr/bin/perl
#
# $Id: logsendmail.pl,v 4.1 1998/03/22 17:57:35 mj Exp $
#
# Gather statistics from sendmail V8 syslog output
#

$VERSION    = '$Revision: 4.1 $ ';
$PROGRAM    = "logsendmail";

$NEWSGROUPS = "fido.de.lists";
$SUBJECT    = "Fido.DE Sendmail Accounting Report";

$INEWS      = "/usr/bin/inews -h -S";
$SENDMAIL   = "/usr/sbin/sendmail";


# Common configuration for perl scripts 
<INCLUDE config.pl>

require "getopts.pl";
&Getopts('vc:o:g:s:nm:');

# read config
$CONFIG      = $opt_c ? $opt_c : "<CONFIG_GATE>";
&CONFIG_read($CONFIG);

$HOSTNAME   = &CONFIG_get("hostname");
$DOMAIN     = &CONFIG_get("domainname");
$DOMAIN     = ".$DOMAIN" if(! $DOMAIN =~ /^\./);
##TEST
$HOSTNAME   = "morannon";
$DOMAIN     = ".fido.de";
##TEST
$FQDN       = $HOSTNAME.$DOMAIN;

if($opt_g) {
    $NEWSGROUPS = $opt_g;
}
if($opt_s) {
    $SUBJECT    = $opt_s;
}
if($opt_n) {
    open(OUT, "|$INEWS") || die "$PROGRAM: can't open pipe to inews\n";
    select(OUT);
    $out_flag = 1;
}
if($opt_m) {
    open(OUT, "|$SENDMAIL $opt_m")
	|| die "$PROGRAM: can't open pipe to sendmail\n";
    select(OUT);
    $out_flag = 1;
}
if($opt_o) {
    $output = $opt_o;
    open(OUT, ">$output") || die "$PROGRAM: can't open $output\n";
    select(OUT);
    $out_flag = 1;
}



print "Newsgroups: $NEWSGROUPS\n" if($opt_n);
print "Subject: $SUBJECT\n" if($opt_m || $opt_n);
print "\n" if($opt_m || $opt_n);

print "# statistics for $FQDN\n" if ($opt_v);


sub parse_addr {
    local($addr) = @_;
    local($user,$site);

    # Special: address is program
    if( $addr =~ /^\"\|/ ) {
	return ("", $FQDN);
    }

    # Parse address (1)
    if( $addr =~ /<(.+@.+)>/ ) {
	$a = $1;
    }
    elsif( $addr =~ /([^ ]+@.[^ ]+) \(.*\)/ ) {
	$a = $1;
    }
    else {
	$a = $addr;
    }
    $a =~ s/\s//g;			# Remove white space

    # Parse address (2)
    if( $a =~ /^(.+)@(.+)$/ ) {
	($user,$site) = ($1,$2);
    }
    elsif( $a =~ /^(.+)!(.+)$/ ) {
	($user,$site) = ($2,$1);
    }
    else {
	($user,$site) = ($a,$FQDN);
    }
    $site =~ tr/A-Z/a-z/;

    return ($user,$site);
}



# 1st run: collect from=, size= data
while(<>) {
    chop;

    if( /^(.+) $HOSTNAME sendmail\[\d+\]: (\w+): (.*)$/ ) {
	$date = $1;
	$id   = $2;
	$rest = $3;

	if(!$first_date) {
	    $first_date = $1;
	    $output_date = substr($first_date, 0, 6);
	    $output_date =~ s/ /-/g;
	}
	$last_date  = $1;

	# Process clone
	if($rest =~ /^clone (\w+),/) {
	    print "# $id: $rest\n" if($opt_v);
	    $id_size{$id} = $id_size{$1};
	    $id_from{$id} = $id_from{$1};
	}

	undef %entry;
	for $v (split(', ', $rest)) {
	    if( $v =~ /^(\w+)=(.*)$/ ) {
		$entry{$1} = $2;
	    }
        }

	# From entry
	if($entry{"from"}) {
	    ($user,$site) = &parse_addr($entry{"from"});
	    $size = $entry{"size"};
	    $id_size{$id} = $size;
	    $id_from{$id} = $site;
	}


	next if($entry{"stat"} =~ /^Queued/);
	next if($entry{"stat"} =~ /^Deferred/);
#	next if($entry{"stat"} =~ /^User unknown/);

	# To entry
	if($entry{"to"}) {
	    ($user,$site) = &parse_addr($entry{"to"});
	    $to   = $site;
	    $size = $id_size{$id};
	    if(!$size) {
		print "# no from= for mail $id to=$entry{\"to\"}\n" if($opt_v);
		$unaccounted++;
		next;
	    }
	    $from = $id_from{$id};

	    # To Fido.DE sites
	    if($site =~ /fido\.de$/i) {
		##FIXME: not very generic
		$site = "morannon.fido.de" if($site =~ /^fido\.de$/i);
		$site =~ s/^p\d+\.//i;
		$site =~ s/\.fido\.de$//i;
		$to_total_size{$site} += $size;
		$to_total_msgs{$site} ++;
		$to_total_size        += $size;
		$to_total_msgs        ++;
		if($from =~ /\.fido\.de$/i || $from =~ /\.z2\.fidonet\.org$/i){
		    $to_intern_size{$site} += $size;
		    $to_intern_msgs{$site} ++;
		    $to_intern_size        += $size;
		    $to_intern_msgs        ++;
		}
		elsif($from =~ /\.de$/i) {
		    $to_de_size{$site} += $size;
		    $to_de_msgs{$site} ++;
		    $to_de_size        += $size;
		    $to_de_msgs        ++;
		}
		else {
		    $to_intl_size{$site} += $size;
		    $to_intl_msgs{$site} ++;
		    $to_intl_size        += $size;
		    $to_intl_msgs        ++;
		}
	    }

	    # From Fido.DE sites
	    $site = $from;
	    if($site =~ /fido\.de$/i) {
		##FIXME: not very generic
		$site = "morannon.fido.de" if($site =~ /^fido\.de$/i);
		$site =~ s/^p\d+\.//i;
		$site =~ s/\.fido\.de$//i;
		$from_total_size{$site} += $size;
		$from_total_msgs{$site} ++;
		$from_total_size        += $size;
		$from_total_msgs        ++;
		if($to =~ /\.fido\.de$/i || $to =~ /\.z2\.fidonet\.org$/i) {
		    $from_intern_size{$site} += $size;
		    $from_intern_msgs{$site} ++;
		    $from_intern_size        += $size;
		    $from_intern_msgs        ++;
		}
		elsif($to =~ /\.de$/i) {
		    $from_de_size{$site} += $size;
		    $from_de_msgs{$site} ++;
		    $from_de_size        += $size;
		    $from_de_msgs        ++;
		}
		else {
		    $from_intl_size{$site} += $size;
		    $from_intl_msgs{$site} ++;
		    $from_intl_size        += $size;
		    $from_intl_msgs        ++;
		}
	    }

	}

    }
}




print "# sendmail statistics: $first_date -- $last_date\n\n";

print "# $unaccounted mails not accounted (bounced messages)\n\n"
    if($unaccounted);

print "# ------------------------------ I N B O U N D -----------------------------\n";
print "#   FROM->                   Total       Fido.DE           .DE     Internat.\n";
print "# TO-v                  #    bytes    #    bytes    #    bytes    #    bytes\n";
print "# ------------------ ------------- ------------- ------------- -------------\n";

for $site (sort keys(%to_total_msgs)) {
    printf "%-20.20s", $site;
    printf " %4d %8d", $to_total_msgs{$site}, $to_total_size{$site};
    printf " %4d %8d", $to_intern_msgs{$site}, $to_intern_size{$site};
    printf " %4d %8d", $to_de_msgs{$site}, $to_de_size{$site};
    printf " %4d %8d", $to_intl_msgs{$site}, $to_intl_size{$site};
    print "\n";
}

print "# ------------------ ------------- ------------- ------------- -------------\n";
print "TOTAL               ";
printf " %4d %8d", $to_total_msgs, $to_total_size;
printf " %4d %8d", $to_intern_msgs, $to_intern_size;
printf " %4d %8d", $to_de_msgs, $to_de_size;
printf " %4d %8d", $to_intl_msgs, $to_intl_size;
print "\n\n";

print "# ---------------------------- O U T B O U N D -----------------------------\n";
print "#     TO->                   Total       Fido.DE           .DE     Internat.\n";
print "# FROM-v                #    bytes    #    bytes    #    bytes    #    bytes\n";
print "# ------------------ ------------- ------------- ------------- -------------\n";

for $site (sort keys(%from_total_msgs)) {
    printf "%-20.20s", $site;
    printf " %4d %8d", $from_total_msgs{$site}, $from_total_size{$site};
    printf " %4d %8d", $from_intern_msgs{$site}, $from_intern_size{$site};
    printf " %4d %8d", $from_de_msgs{$site}, $from_de_size{$site};
    printf " %4d %8d", $from_intl_msgs{$site}, $from_intl_size{$site};
    print "\n";
}

print "# ------------------ ------------- ------------- ------------- -------------\n";
print "TOTAL               ";
printf " %4d %8d", $from_total_msgs, $from_total_size;
printf " %4d %8d", $from_intern_msgs, $from_intern_size;
printf " %4d %8d", $from_de_msgs, $from_de_size;
printf " %4d %8d", $from_intl_msgs, $from_intl_size;
print "\n\n";


close(OUT) if($out_flag);
