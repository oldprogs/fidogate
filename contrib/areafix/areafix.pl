#!/usr/bin/perl

require "fido.pli";

# Read Config
&readconfig;

# Create Hash of Akalist
&aka2hash;

# areas.bbs einlesen
&parseareasbbs;

# Packet oeffnen
$createpacket=1;
if ($config{"test"} == 0) {
    for $toparse (@toparse) {
	&getfilename ($toparse);
	if (-f $parsefilename) {
	    &openpacket($parsefilename);
	    # Packt erzeugen
	    if ($createpacket) {
		&createpacket ($config{"mainaka"},$config{"mainaka"},"GEHEIM\x00\x00");
		undef $createpacket;
                $packetexist=1;
	    }
	    # Message einlesen, parsen, (Meldung als Debug ausgeben), Antwort
	    # Mail schreiben
	    while (&getnextmessage) {
		&parseareafixmail($orig,$from,$subject,$message);
#		print $messageback;
		$zonetosend = $orig;
		$zonetosend =~ s/:.*$//;
		&addnm ($akalist{$zonetosend},$orig,"Areafix",$from,"Your Areafix request",$messageback,"Areafix .5beta");
	    }
	    close PACKET;
	    unlink $parsefilename or die "Kann $parsefile nicht loeschen";
	}    
    }
} else {
    &openpacket("test.pkt") or die "Kann Testfile nicht oeffnen\n";
    while (&getnextmessage) {
	&parseareafixmail($orig,$from,$subject,$message);
	print $messageback;
	$zonetosend = $orig;
	$zonetosend =~ s/:.*$//;
    }
    close PACKET;
}

# Geaenderte areas.bbs auf Platte schreiben.
if ($areasbbschanged) {
    &writeareasbbs;
}

# Eine Area beim Uplink bestellen?
if ( -f "tosubscribe" ) {
    open (SUB,"tosubscribe");
    while (<SUB>) {
	chomp;
	if ($_) {
	    if ($createpacket) {
		&createpacket ($config{"mainaka"},$config{"mainaka"},"GEHEIM\x00\x00");
                undef $createpacket;
		$packetexist=1;
	    }
	    &addnm ($akalist{$zone{$_}},$uplink{$zone{$_}},"Areafix","Areafix",$passwd{$uplink{$zone{$_}}},"$_\r\n",$config{"tearline"});
	}
    }
    unlink "tosubscribe";
}

# Eine Area beim Uplink abbestellen?
if ( -f "tounsubscribe" ) {
    open (SUB,"tounsubscribe");
    while (<SUB>) {
	chomp;
	if ($_) {
            if ($createpacket) {
		&createpacket ($config{"mainaka"},$config{"mainaka"},"GEHEIM\x00\x00");
		undef $createpacket;
                $packetexist=1;
	    }
	    &addnm ($akalist{$zone{$_}},$uplink{$zone{$_}},"Areafix","Areafix",$passwd{$uplink{$zone{$_}}},"-$_\r\n",$config{"tearline"});
	}
    }
    unlink "tounsubscribe";
}

# Array in dem das Packet steht auf Platte schreiben
if ($packetexist) {
    while ( -f $config{"tosendfile"}) {
	sleep 10;
    }
    &closepacket;
    open (TO,">$config{'tosendfile'}") or die "Kann tosend nicht schreiben";
    for (@packet) { print TO; }
    close TO;
}
