#!/bin/sh
#
# $Id: ffxrun.sh,v 4.0 1996/04/17 18:17:40 mj Exp $
#
# Run ffxqt: mail first, then news.
#

# unbatch mail, process mail jobs (grade `f')
<LIBDIR>/ffxqt -gf $*
<LIBDIR>/ffxqt -gf $*

# process news jobs (grade `n')
<LIBDIR>/ffxqt -gn $*
