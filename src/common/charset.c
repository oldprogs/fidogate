/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: charset.c,v 4.0 1996/04/17 18:17:39 mj Exp $
 *
 * ^ACHARSET handling stuff as described in FSC-0054
 *
 *****************************************************************************
 * Copyright (C) 1990-1996
 *  _____ _____
 * |	 |___  |   Martin Junius	     FIDO:	2:2452/110
 * | | | |   | |   Republikplatz 3	     Internet:	mj@fido.de
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FIDOGATE; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

#include "fidogate.h"



#define MAXCHARSET	7

#define CHARSET_N	4

#define CHARSET_NONE	0
#define CHARSET_LATIN1	1
#define CHARSET_IBMPC	2
#define CHARSET_MAC	3

#ifdef CHARSET_DEFAULT_IBMPC
/* Default charset ist "IBMPC" */
# define CHARSET_DEFAULT	CHARSET_IBMPC
#else
/* Default charset ist "NONE" */
# define CHARSET_DEFAULT	CHARSET_NONE
#endif


static char *level_2_opt[MAXCHARSET] = {
    "ASCII",	    "LATIN-1",	    "IBMPC",	    "MAC",
    "VT100",	    "PC-8",	    "AMIGA"
};

static int   level_2_chrs[MAXCHARSET] = {
    CHARSET_NONE,   CHARSET_LATIN1, CHARSET_IBMPC,  CHARSET_MAC,
    CHARSET_NONE,   CHARSET_IBMPC,  CHARSET_LATIN1
};

static int charset = CHARSET_DEFAULT;

static char *xtab[CHARSET_N][128] = {
#include "charset.h"			    /* Translation table */
};



/***** Reset char set ********************************************************/

void charset_reset(void)
{
    charset = CHARSET_DEFAULT;
}



/***** Set character set from ^ACHARSET kludge line **************************/

void charset_set(char *s)
{
    char *name;
    char *p;
    int i=0;
    int level;
    
    while(is_space(*s))
	s++;

    debug(5, "^ACHARSET: %s", s);

    s = strsave(s);
    name  = strtok(s,	 " ");
    if(!name)
	return;
    p	  = strtok(NULL, " ");
    if(!p)
	/*
	 * In this case it's a FSC-0050 kludge without the class code.
	 * Treat it like FSC-0054 level 2.
	 */
	level = 2;
    else
	level = atoi(p);
    
    charset = CHARSET_NONE;
    if(level == 2)
	for(charset=i=0; i<MAXCHARSET; i++)
	    if(!strcmp(name, level_2_opt[i])) {
		charset = level_2_chrs[i];
		break;
	    }
    xfree(s);

    if(charset != CHARSET_NONE)
	debug(5, "charset=%d (%s)", charset, level_2_opt[i]);
}



/***** Get translated char ***************************************************/

char *charset_xlate(int c)
{
    if(charset>=0 && charset<CHARSET_N)
	return xtab[charset][c & 0x7f];
    else
	return NULL;
}
