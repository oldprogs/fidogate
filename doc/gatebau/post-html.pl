#!/usr/local/bin/perl -i
#
# post-html.pl --- post processor for sgml2html
#

require "getopts.pl";

&Getopts("");


while (<>) {
    s/&circ;/^/g;
    s/\$Revision: 1.1 $]*) \$/$1/;
    s/\$Date: 1997/05/01 17:57:15 $]*) \$/$1/;
    s/^mailto:(.*) <\/LI>/<a href="mailto:$1">&lt;$1&gt;<\/a> <\/li>/;

    print;
}
