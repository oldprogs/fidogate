/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: misc.c,v 4.12 1999/01/02 16:34:59 mj Exp $
 *
 * Miscellaneous functions
 *
 *****************************************************************************
 * Copyright (C) 1990-1999
 *  _____ _____
 * |	 |___  |   Martin Junius	     FIDO:	2:2452/110
 * | | | |   | |   Radiumstr. 18  	     Internet:	mj@fido.de
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FIDOGATE; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

#include "fidogate.h"



/*
 * Last char in string
 */
int str_last(char *s, size_t len)
{
    int l = strlen(s) - 1;
    if(l >= len)
	l = len - 1;
    if(l < 0)
	l = 0;

    return s[l];
}



/*
 * Pointer to last char in string
 */
char *str_lastp(char *s, size_t len)
{
    int l = strlen(s) - 1;
    if(l >= len)
	l = len - 1;
    if(l < 0)
	l = 0;

    return s + l;
}



/*
 * Convert to lower case
 */
char *str_lower(char *s)
{
    char *p;

    if(!s)
	return NULL;
    
    for(p=s; *p; p++)
	if(isupper(*p))
	    *p = tolower(*p);
    
    return s;
}



/*
 * Convert to upper case
 */
char *str_upper(char *s)
{
    char *p;

    if(!s)
	return NULL;
    
    for(p=s; *p; p++)
	if(islower(*p))
	    *p = toupper(*p);
    
    return s;
}



/*
 * Secure string functions:
 *
 * str_copy  (d,n,s)              - copy string
 * str_copy2 (d,n,s1,s2)          - copy concatenation of 2 strings
 * str_copy3 (d,n,s1,s2,s3)       - copy concatenation of 3 strings
 * str_copy4 (d,n,s1,s2,s3,s4)    - copy concatenation of 4 strings
 * str_copy5 (d,n,s1,s2,s3,s4,s5) - copy concatenation of 5 strings
 * str_append(d,n,s)              - append string
 *
 * d = destination buffer
 * n = destination size
 */
char *str_copy(char *d, size_t n, char *s)
{
    strncpy(d, s, n);
    d[n-1] = 0;
    return d;
}

char *str_append(char *d, size_t n, char *s)
{
    int max = n - strlen(d) - 1;
    strncat(d, s, max);
    d[n-1] = 0;
    return d;
}    

char *str_copy2(char *d, size_t n, char *s1, char *s2)
{
    str_copy  (d, n, s1);
    str_append(d, n, s2);
    return d;
}

char *str_copy3(char *d, size_t n, char *s1, char *s2, char *s3)
{
    str_copy  (d, n, s1);
    str_append(d, n, s2);
    str_append(d, n, s3);
    return d;
}

char *str_copy4(char *d, size_t n, char *s1, char *s2, char *s3, char *s4)
{
    str_copy  (d, n, s1);
    str_append(d, n, s2);
    str_append(d, n, s3);
    str_append(d, n, s4);
    return d;
}

char *str_copy5(char *d, size_t n, char *s1, char *s2, char *s3, char *s4, char *s5)
{
    str_copy  (d, n, s1);
    str_append(d, n, s2);
    str_append(d, n, s3);
    str_append(d, n, s4);
    str_append(d, n, s5);
    return d;
}

    

/*
 * Copy string range
 */
char *str_copy_range(char *d, size_t n, char *s, char *lim)
{
    int i;
    
    for(i=0; i<n-1 && s<lim; i++, s++)
	d[i] = *s;
    d[i] = 0;

    return d;
}



/*
 * strncpy0() --- strncpy() with terminating '\0' char
 */
char *strncpy0(char *d, char *s, int n)
{
    strncpy(d, s, n);
    d[n-1] = 0;
    return d;
}



/*
 * strncat0() --- strncat() with terminating '\0' char and
 *                n == destination buffer size
 */
char *strncat0(char *d, char *s, int n)
{
    int max = n - strlen(d) - 1;
    strncat(d, s, max);
    d[n-1] = 0;
    return d;
}    


    
#if !defined(HAS_STRCASECMP) && !defined(HAS_STRICMP)
/***** strnicmp() --- compare n chars of strings ignoring case ***************/

int strnicmp(char *sa, char *sb, int len)
{
    while(len--)
	if(tolower(*sa) == tolower(*sb)) {
	    sa++;
	    sb++;
	}
	else if(tolower(*sa) < tolower(*sb))
	    return(-1);
	else
	    return(1);
    return(0);
}



/***** stricmp() --- compare strings ignoring case ***************************/

int stricmp(char *sa, char *sb)
{
    while(tolower(*sa) == tolower(*sb)) {
	if(!*sa)
	    return(0);
	sa++;
	sb++;
    }
    return(tolower(*sa) - tolower(*sb));
}
#endif /**!HAS_STRCASECMP && !HAS_STRICMP**/



/*
 * Remove \r\n from end of line
 */
void strip_crlf(char *line)
{
    int len;

    if(!line)
	return;
    
    len = strlen(line);
    if( line[len-1] == '\n' )
	line[len-1] = 0;
    
    len = strlen(line);
    if( line[len-1] == '\r' )
	line[len-1] = 0;
}



/*
 * Remove white space at end of line
 */
void strip_space(char *line)
{
    int i;
    
    if(!line)
	return;
    
    for(i=strlen(line)-1; i>=0; i--)
	if(is_space(line[i]))
	    line[i] = 0;
	else
	    break;
}



/*
 * isspace() replacement, checking for SPACE, TAB, CR, LF
 */
int is_space(int c)
{
    return c==' ' || c=='\t' || c=='\r' || c=='\n';
}


/*
 * checking for SPACE or TAB
 */
int is_blank(int c)
{
    return c==' ' || c=='\t';
}


/*
 * signed char-safe version of isdigit()
 */
int is_digit(int c)
{
    /* Some <ctype.h> implementation only accept a parameter value range
     * of [-1,255]. This may lead to problems, because we're quite often
     * passing *p's to is_digit() with a char *p variable. The default
     * char type is signed in most C implementation. */
    return isdigit((c & 0xff));
}


/*
 * Check for hex digits, signed char-safe version of isxdigit()
 */
int is_xdigit(int c)
{
    /* Some <ctype.h> implementation only accept a parameter value range
     * of [-1,255]. This may lead to problems, because we're quite often
     * passing *p's to is_digit() with a char *p variable. The default
     * char type is signed in most C implementation. */
    return isxdigit((c & 0xff));
}


/*
 * Check for octal digits
 */
int is_odigit(int c)
{
    return c>='0' && c<'8';
}



/*
 * Expand %X at start of file names
 *
 * See config.make for definition of abbreviations.
 */
static struct st_atable
{
    int c;
    char *(*func)(void);
}
atable[] = 
{
    /* Include code generated by subst.pl -A */
#   include "cf_abbrev.c"
    { 0, NULL }
};


char *str_expand_name(char *d, size_t n, char *s)
{
    int i;
    
    d[0] = 0;

    if(s[0] == '%')
    {
	for(i=0; atable[i].c; i++)
	    if(atable[i].c == s[1])
	    {
		str_append(d, n, atable[i].func());
		s+=2;
		break;
	    }
    }
    
    str_append(d, n, s);
    
    return d;
}



/*
 * Convert `/' to `\' for MSDOS / OS2 filenames
 */
char *str_dosify(char *s)
{
    for(; *s; s++)
	switch(*s)
	{
	case '/':
	    *s = '\\';  break;
	}
    return s;
}



/*
 * Run system command, return exit code
 */
int run_system(char *s)
{
    char cmd[MAXPATH];

    BUF_EXPAND(cmd, s);
    DOSIFY_IF_NEEDED(cmd);
    return (system(cmd) >> 8) & 0xff;
}
