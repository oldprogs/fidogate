/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: wildmat.c,v 4.0 1996/04/17 18:17:40 mj Exp $
 *
 * Wildcard matching
 *
 *****************************************************************************
 * Copyright (C) 1990-1996
 *  _____ _____
 * |     |___  |   Martin Junius             FIDO:      2:2452/110
 * | | | |   | |   Republikplatz 3           Internet:  mj@fido.de
 * |_|_|_|@home|   D-52072 Aachen, Germany
 *
 * This file is part of FIDOGATE.
 *
 * FIDOGATE is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * FIDOGATE is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FIDOGATE; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

/* Wildcard matching routines.
   Copyright (C) 1988 Free Software Foundation

This file is part of GNU Tar.

GNU Tar is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 1, or (at your option)
any later version.

GNU Tar is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Tar; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
 * @(#)wildmat.c 1.3 87/11/06
 *
~From: rs@mirror.TMC.COM (Rich Salz)
~Newsgroups: net.sources
~Subject: Small shell-style pattern matcher
Message-ID: <596@mirror.TMC.COM>
~Date: 27 Nov 86 00:06:40 GMT

There have been several regular-expression subroutines and one or two
filename-globbing routines in mod.sources.  They handle lots of
complicated patterns.  This small piece of code handles the *?[]\
wildcard characters the way the standard Unix(tm) shells do, with the
addition that "[^.....]" is an inverse character class -- it matches
any character not in the range ".....".  Read the comments for more
info.

For my application, I had first ripped off a copy of the "glob" routine
from within the find source, but that code is bad news:  it recurses
on every character in the pattern.  I'm putting this replacement in the
public domain.  It's small, tight, and iterative.  Compile with -DTEST
to get a test driver.  After you're convinced it works, install in
whatever way is appropriate for you.

I would like to hear of bugs, but am not interested in additions; if I
were, I'd use the code I mentioned above.
*/
/*
**  Do shell-style pattern matching for ?, \, [], and * characters.
**  Might not be robust in face of malformed patterns; e.g., "foo[a-"
**  could cause a segmentation violation.
**
**  Written by Rich $alz, mirror!rs, Wed Nov 26 19:03:17 EST 1986.
*/

/*
 * Modified 6Nov87 by John Gilmore (hoptoad!gnu) to return a "match"
 * if the pattern is immediately followed by a "/", as well as \0.
 * This matches what "tar" does for matching whole subdirectories.
 *
 * The "*" code could be sped up by only recursing one level instead
 * of two for each trial pattern, perhaps, and not recursing at all
 * if a literal match of the next 2 chars would fail.
 */

/****************************************************************
 *
 *  Modified 5/5/91 by Kai Henningsen (kh@ms.maus.de)
 *                  to eliminate recursion completely
 *                  (what good was that anyway?)
 *
 ****************************************************************/

#include "fidogate.h"



/*
 * Compare two chars
 */
static int char_eq(int c1, int c2, int ic)
{
    if(ic)
    {
	if(islower(c1))  c1 = toupper(c1);
	if(islower(c2))  c2 = toupper(c2);
    }
    return c1 == c2;
}



/*
 * Compatibility with old wildmat() function
 */
int wildmat(char *s, char *pat)
{
    return wildmatch(s, pat, FALSE);
}



/*
 * Compare string with shell-style wildcard pattern ?*[a-z]
 */
int wildmatch(char *s, char *p, int ic)
    /*
     * s  --- string
     * p  --- pattern
     * ic --- TRUE=ignore case, FALSE=don't
     */
{
    char *saved_s, *saved_p=NULL;

    saved_s = "\0"; /* for BackTrack */

TryAgain:

    for ( ; *p; s++, p++)
        switch (*p) {
            case '\\':
                /* Literal match with following character; fall through. */
                p++;
            default:
                if(!char_eq(*s, *p, ic))
                    goto BackTrack;
                continue;
            case '?':
                /* Match anything. */
                if (*s == '\0')
                    return(FALSE);
                continue;
            case '*':
                /* Trailing star matches everything. */
                if (!*++p) return(TRUE);
                saved_s = s;
                saved_p = p;
                goto TryAgain;
            case '[':
	    {
                int last;
                int matched;
                int reverse;

                /* [^....] means inverse character class. */
                if ( ( reverse = (p[1] == '^') ) )
                    p++;
                for (last = 0400, matched = FALSE; *++p && *p != ']'; last=*p)
                    /* This next line requires a good C compiler. */
                    if( *p=='-' ? *s<=*++p && *s>=last : char_eq(*s, *p, ic) )
                        matched = TRUE;
                if (matched == reverse)
                    goto BackTrack;
                continue;
              }
        }

    /* For "tar" use, matches that end at a slash also work. --hoptoad!gnu */
    if (*s == '\0' /**|| *s == '/'**/) return(TRUE);

BackTrack:
  if (*saved_s == '\0' || *(s = ++saved_s) == '\0')
        return FALSE;
  p = saved_p;
  goto TryAgain;
}



#ifdef  TEST
#include <stdio.h>

extern char     *gets();


main(void)
{
    char pattern[80];
    char text[80];

    while (TRUE) {
        printf("Enter pattern: "); fflush(stdout);
        if (gets(pattern) == NULL)
            break;
        while (TRUE) {
            printf("Enter text:    "); fflush(stdout);
            if (gets(text) == NULL)
                exit(0);
            if (text[0] == '\0')
                /* Blank line; go back and get a new pattern. */
                break;
            printf("wildmatch(): noic=%d ic=%d\n",
		   wildmatch(text, pattern, FALSE),
		   wildmatch(text, pattern, TRUE ) );
        }
    }
    exit(0);
}
#endif  /* TEST */
