##############################################################################
#
# $Id: config.pl,v 4.2 1998/03/07 16:53:25 mj Exp $
#
# Perl functions to read FIDOGATE config file,
# included by <INCLUDE config.pl> when running subst.pl
#

undef %CONFIG;

%CONFIG_default =
    (
     <GEN perl-default>
     );
%CONFIG_abbrev =
    (
     <GEN perl-abbrev>
     );



sub CONFIG_read {
    local($file) = @_;
    local *C;

    $file = &CONFIG_expand($file);

    open(C,"$file") || die "config.pl: can't open config file $file\n";
    while(<C>) {
	chop;
	next if( /^\s*\#/ );	# comments
	next if( /^\s*$/  );	# empty
	s/\s*$//;		# remove trailing white space
	s/^\s*//;		# remove leading white space
	($key,$arg) = split(' ', $_, 2);
	$key =~ tr/A-Z/a-z/;
	&CONFIG_read($arg) if($key eq "include");
	$CONFIG{$key} = $arg if(!$CONFIG{$key});
    }
    close(C);
}


sub CONFIG_get1 {
    local($key) = @_;
    local($ukey);

    $ukey = $key;
    $ukey =~ tr/a-z/A-Z/;
    return $ENV{"FIDOGATE_$ukey"} if($ENV{"FIDOGATE_$ukey"});

    return $CONFIG{$key} if($CONFIG{$key});
    return $CONFIG_default{$key};
}


sub CONFIG_get {
    local($key) = @_;
    local($ret);
    local($exp);

    $key =~ tr/A-Z/a-z/;
    return &CONFIG_expand( &CONFIG_get1($key) );
}


sub CONFIG_expand {
    local($v) = @_;
    local($exp);

    if($v =~ /^%([A-Z])/) {
	$exp = &CONFIG_get1($CONFIG_abbrev{$1});
	$v =~ s/^%./$exp/;
    }

    return $v;
}


sub CONFIG_debug {    
    local($key);

    for $key (keys %CONFIG) {
	print "$key = $CONFIG{$key} -> ", &CONFIG_get($key), "\n";
    }
}

##############################################################################
