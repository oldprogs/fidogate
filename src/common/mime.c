/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: mime.c,v 4.4 1996/12/17 17:19:43 mj Exp $
 *
 * MIME stuff
 *
 *****************************************************************************
 * Copyright (C) 1990-1997
 *  _____ _____
 * |     |___  |   Martin Junius             FIDO:      2:2452/110
 * | | | |   | |   Radiumstr. 18             Internet:  mj@fido.de
 * |_|_|_|@home|   D-51069 Koeln, Germany
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

#include "fidogate.h"


static int is_qpx		(int);
static int x2toi		(char *);


static int is_qpx(int c)
{
    return isxdigit(c) /**isdigit(c) || (c>='A' && c<='F')**/ ;
}


static int x2toi(char *s)
{
    int val = 0;
    int n;

    n = toupper(*s) - (isalpha(*s) ? 'A'-10 : '0');
    val = val*16 + n;
    s++;
    n = toupper(*s) - (isalpha(*s) ? 'A'-10 : '0');
    val = val*16 + n;

    return val;
}



/*
 * Dequote string with MIME-style quoted-printable =XX
 */
char *mime_dequote(char *d, size_t n, char *s, int flags)
{
    int i;

    for(i=0; i<n-1 && *s; i++, s++)
    {
	if(flags & MIME_QP)
	    if(s[0] == '=')	/* Maybe MIME quoted printable */
	    {
		if(is_qpx(s[1]) && is_qpx(s[2]))	/* =XX */
		{
		    d[i] = x2toi(s+1);
		    s += 2;
		    continue;
		}
		if(s[1]=='\n'                ||		/* =<LF> */
		   (s[1]=='\r' && s[2]=='\n')  )	/* this as well */
		{
		    d[i] = 0;
		    break;
		}
	    }

	if(flags & MIME_US)
	    if(s[0] == '_')
	    {
		d[i] = ' ';
		continue;
	    }
	
	/* Nothing special to do */
	d[i] = *s;
    }
    d[i] = 0;

    return d;
}



/*
 * Decode MIME RFC1522 header
 *
 **FIXME: currently always assumes ISO-8859-1 char set
 **FIXME: optional flag for conversion to 7bit ASCII replacements
 */
#define MIME_HEADER_CODE_START	"=?ISO-8859-1?Q?"
#define MIME_HEADER_CODE_END	"?="

char *mime_deheader(char *d, size_t n, char *s, int flags)
{
    int i;
    int mime_flag = FALSE;

    for(i=0; i<n-1 && *s; i++, s++)
    {
	if(strnieq(s, MIME_HEADER_CODE_START, strlen(MIME_HEADER_CODE_START)))
	{
	    mime_flag = TRUE;
	    s += strlen(MIME_HEADER_CODE_START);
	}

	if(mime_flag)
	{
	    if(strneq(s, MIME_HEADER_CODE_END, strlen(MIME_HEADER_CODE_END)))
	    {
		mime_flag = FALSE;
		continue;
	    }
	    
	    if(s[0] == '=')	/* Maybe MIME quoted printable */
	    {
		if(is_qpx(s[1]) && is_qpx(s[2]))	/* =XX */
		{
		    d[i] = x2toi(s+1);
		    s += 2;
		    continue;
		}
	    }

	    if(s[0] == '_')
	    {
		d[i] = ' ';
		continue;
	    }
	}
	
	/* Nothing special to do */
	d[i] = *s;
    }
    d[i] = 0;

    return d;
}
