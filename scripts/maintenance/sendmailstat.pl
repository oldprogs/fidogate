#!/usr/bin/perl
#
# $Id: sendmailstat.pl,v 4.0 1996/04/17 18:17:38 mj Exp $
#
# Gather statistics from sendmail V8 syslog output
#

$LOG      = "/var/adm/messages";	# sendmail syslog output

$HOSTNAME = "morannon";
$DOMAIN   = "fido.de";			# No leading `.' !!! 


require "getopts.pl";

&Getopts('l:o:');

$LOG = $opt_l if($opt_l);



sub parse_addr {
    local($addr) = @_;
    local($user,$site);

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

    if( $a =~ /^(.+)@(.+)$/ ) {
	($user,$site) = ($1,$2);
    }
    elsif( $a =~ /^(.+)!(.+)$/ ) {
	($user,$site) = ($2,$1);
    }
    else {
	($user,$site) = ($a,"$HOSTNAME.$DOMAIN");
    }
    $site =~ tr/A-Z/a-z/;

    if($site eq "sungate" || $site eq "sungate.") {
	$site = "sungate.$DOMAIN";
    }

    return ($user,$site);
}



# Open syslog file
open(LOG, "$LOG") || die "sendmailstat: can't open $LOG\n";

# 1st run: collect from=, size= data
while( <LOG> ) {
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
    }
}

# 2nd run: collect and sum from=, to= data
seek(LOG, 0, 0);

while( <LOG> ) {
    chop;

    if( /^(.+) $HOSTNAME sendmail\[\d+\]: (\w+): (.*)$/ ) {
	$date = $1;
	$id   = $2;
	$rest = $3;

	undef %entry;
	for $v (split(', ', $rest)) {
	    if( $v =~ /^(\w+)=(.*)$/ ) {
		$entry{$1} = $2;
	    }
        }

#	print "---------- $id ----------\n";
#	for $v (keys %entry) {
#	    print "$v=$entry{$v}\n";
#	}

	# Ignore stat=queued entries
	next if($entry{"stat"} eq "queued");

	# To entry
	if($entry{"to"}) {
	    ($user,$site) = &parse_addr($entry{"to"});
	    $to   = $site;
	    $size = $id_size{$id};
	    if(!$size) {
		print STDERR "sendmailstat: strange, no from= for mail $id\n";
		next;
	    }
	    $from = $id_from{$id};

	    # To Fido.DE sites
	    if($site =~ /\.fido\.de$/) {
		$site =~ s/^p\d+\.//;
		$site =~ s/\.fido\.de$//;
		$to_total_size{$site} += $size;
		$to_total_msgs{$site} ++;
		$to_total_size        += $size;
		$to_total_msgs        ++;
		if($from =~ /\.fido\.de$/ || $from =~ /\.z2\.fidonet\.org$/) {
		    $to_intern_size{$site} += $size;
		    $to_intern_msgs{$site} ++;
		    $to_intern_size        += $size;
		    $to_intern_msgs        ++;
		}
		elsif($from =~ /\.rwth-aachen\.de$/) {
		    $to_rwth_size{$site} += $size;
		    $to_rwth_msgs{$site} ++;
		    $to_rwth_size        += $size;
		    $to_rwth_msgs        ++;
		}
		elsif($from =~ /\.de$/ || $from =~ /\.sub\.org$/) {
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
	    if($site =~ /\.fido\.de$/) {
		$site =~ s/^p\d+\.//;
		$site =~ s/\.fido\.de$//;
		$from_total_size{$site} += $size;
		$from_total_msgs{$site} ++;
		$from_total_size        += $size;
		$from_total_msgs        ++;
		if($to =~ /\.fido\.de$/ || $to =~ /\.z2\.fidonet\.org$/) {
		    $from_intern_size{$site} += $size;
		    $from_intern_msgs{$site} ++;
		    $from_intern_size        += $size;
		    $from_intern_msgs        ++;
		}
		elsif($to =~ /\.rwth-aachen\.de$/) {
		    $from_rwth_size{$site} += $size;
		    $from_rwth_msgs{$site} ++;
		    $from_rwth_size        += $size;
		    $from_rwth_msgs        ++;
		}
		elsif($to =~ /\.de$/ || $to =~ /\.sub\.org$/) {
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

# Close log file
close(LOG);


# Output statistics
if($opt_o) {
    $output = $opt_o;
    $output =~ s/%D/$output_date/;
    open(OUT, ">$output") || die "sendmailstat: can't open $output\n";
    select(OUT);
}

print "# sendmail statistics: $first_date -- $last_date\n\n";

print "# To site             Total       Fido.DE       RWTH       .DE        Internat.\n";
print "#                   #    bytes    #   bytes   #   bytes   #   bytes   #   bytes\n";
print "# ------------   -------------  ----------- ----------- ----------- -----------\n";

for $site (sort keys(%to_total_msgs)) {
    printf "%-16.16s", $site;
    printf " %4d %8d ", $to_total_msgs{$site}, $to_total_size{$site};
    printf " %3d %7d", $to_intern_msgs{$site}, $to_intern_size{$site};
    printf " %3d %7d", $to_rwth_msgs{$site}, $to_rwth_size{$site};
    printf " %3d %7d", $to_de_msgs{$site}, $to_de_size{$site};
    printf " %3d %7d", $to_intl_msgs{$site}, $to_intl_size{$site};
    print "\n";
}

print "# ------------   -------------  ----------- ----------- ----------- -----------\n";
print "TOTAL           ";
printf " %4d %8d ", $to_total_msgs, $to_total_size;
printf " %3d %7d", $to_intern_msgs, $to_intern_size;
printf " %3d %7d", $to_rwth_msgs, $to_rwth_size;
printf " %3d %7d", $to_de_msgs, $to_de_size;
printf " %3d %7d", $to_intl_msgs, $to_intl_size;
print "\n\n";


print "# From site           Total       Fido.DE       RWTH       .DE        Internat.\n";
print "#                   #    bytes    #   bytes   #   bytes   #   bytes   #   bytes\n";
print "# ------------   -------------  ----------- ----------- ----------- -----------\n";

for $site (sort keys(%from_total_msgs)) {
    printf "%-16.16s", $site;
    printf " %4d %8d ", $from_total_msgs{$site}, $from_total_size{$site};
    printf " %3d %7d", $from_intern_msgs{$site}, $from_intern_size{$site};
    printf " %3d %7d", $from_rwth_msgs{$site}, $from_rwth_size{$site};
    printf " %3d %7d", $from_de_msgs{$site}, $from_de_size{$site};
    printf " %3d %7d", $from_intl_msgs{$site}, $from_intl_size{$site};
    print "\n";
}

print "# ------------   -------------  ----------- ----------- ----------- -----------\n";
print "TOTAL           ";
printf " %4d %8d ", $from_total_msgs, $from_total_size;
printf " %3d %7d", $from_intern_msgs, $from_intern_size;
printf " %3d %7d", $from_rwth_msgs, $from_rwth_size;
printf " %3d %7d", $from_de_msgs, $from_de_size;
printf " %3d %7d", $from_intl_msgs, $from_intl_size;
print "\n\n";


close(OUT) if($opt_o);
