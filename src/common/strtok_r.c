/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: strtok_r.c,v 4.1 1999/06/01 21:19:34 mj Exp $
 *
 * Specialized strtok() variants for FIDOGATE, based on NetBSD strtok_r.c,
 * see below for original copyright.
 *
 *****************************************************************************
 * Copyright (C) 1990-1999
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

/*	$NetBSD: strtok_r.c,v 1.3 1998/02/03 18:49:25 perry Exp $	*/

/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */



#include "fidogate.h"

/**prototypes.h**/
#define DELIM_WS	" \t\r\n"
#define DELIM_EOL	"\r\n"
#define QUOTE		'\"'
#define DQUOTE		'\"'
#define SQUOTE		'\''


char   *strtok_r_ext		(char *, const char *, char **, int);

/****************/



char *strtok_r_ext(char *s, const char *delim, char **lasts, int quote)
{
    char *spanp;
    int c, sc;
    char *tok, *p;

    if(quote == TRUE)
        quote = DQUOTE;
    if(s == NULL && (s = *lasts) == NULL)
        return NULL;

    /* Skip (span) leading delimiters (s += strspn(s, delim), sort
     * of).  */
cont:
    c = *s++;
    for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
        if (c == sc)
	    goto cont;
    }

    if (c == 0) {		/* no non-delimiter characters */
        *lasts = NULL;
	return NULL;
    }
    tok = s - 1;

    /* In quote mode, check for string enclosed in "..." */
    if(quote && *tok==quote)
    {
        tok++;
	while(*s)
	{
	    if(s[0]=='\\' && s[1]==quote)
	        s++;
	    else if(s[0]==quote)
	        break;
	    s++;
	}
	if(*s == quote)
	    *s++ = 0;
	*lasts = s;
	s = p = tok;
	while(*s)
	{
	    if(s[0]=='\\' && s[1]==quote)
	        s++;
	    *p++ = *s++;
	}
	*p = 0;
	return tok;
    }

    /* Scan token (scan for delimiters: s += strcspn(s, delim), sort
     * of).  Note that delim must have one NUL; we stop if we see
     * that, too.  */
    for (;;) {
        c = *s++;
	spanp = (char *)delim;
	do {
	    if ((sc = *spanp++) == c) {
	        if (c == 0)
		    s = NULL;
		else
		    s[-1] = 0;
		*lasts = s;
		return tok;
	    }
	} while (sc != 0);
    }
    
    /* NOTREACHED */
    return NULL;
}




/***** TEST ******************************************************************/

#ifdef TEST
/*
 * Function test
 */
int main(int argc, char *argv[])
{
    char *d, *p;
    char *last;
    int i;


    BUF_COPY(buffer, "Dies ist ein Test fuer die strtok-Funktionen\n");
    d = DELIM_WS;
    printf("String = %s", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, FALSE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, FALSE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "Dies ist \"ein Test\" \"fuer die\" strtok-Funktionen\n");
    d = DELIM_WS;
    printf("String = %s", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, FALSE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, FALSE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "Dies ist \"ein Test\" \"fuer die\" strtok-Funktionen\n");
    d = DELIM_WS;
    printf("String = %s", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, TRUE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, TRUE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "Dies ist \"ein Test\\\" \\\"fuer die\" strtok-Funktionen\n");
    d = DELIM_WS;
    printf("String = %s", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, DQUOTE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, DQUOTE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "Dies 'ist ein Test' 'fuer die' strtok-Funktionen\n");
    d = DELIM_WS;
    printf("String = %s", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, SQUOTE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, SQUOTE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "fido.de,de.answers,comp.misc.misc,alt.x.xx.xxx.xxxx");
    d = ",";
    printf("String = %s\n", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, FALSE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, FALSE))
        printf("    %02d = [%s]\n", i, p);


    BUF_COPY(buffer, "\"12,5\",\"0,9\",\"13,75\",\"0,99\",\"1,23\"");
    d = ",";
    printf("String = %s\n", buffer);
    for(i = 0, p = strtok_r_ext(buffer, d, &last, QUOTE);
	p;
	i++,   p = strtok_r_ext(NULL, d, &last, QUOTE))
        printf("    %02d = [%s]\n", i, p);


    exit(0);

    /**NOT REACHED**/
    return 1;
}
#endif /**TEST**/
