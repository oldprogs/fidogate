/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: rfcheader.c,v 4.1 1996/04/26 08:41:40 mj Exp $
 *
 * Functions to process RFC822 header lines from messages
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

#include "fidogate.h"

#include "shuffle.h"



/*
 * Textlist to hold header of message
 */
static Textlist header = { NULL, NULL };
static Textline *last_header = NULL;



/*
 * header_ca_rfc() --- Output ^ARFC-Xxxx kludges
 */
int header_ca_rfc(FILE *out, int rfc_level)
{
    static char *rfc_lvl_1[] = { RFC_LVL_1_HEADERS, NULL };
    
    /* RFC level 0 - no ^ARFC-Xxxx kludges */
    if(rfc_level <= 0)
    {
	;
    }
    
    /* RFC level 1 - selected set of ^ARFC-Xxxx kludges */
    else if(rfc_level == 1)
    {
	char **name;
	Textline *p;
	int len;
	int ok = FALSE;
	
	for(p=header.first; p; p=p->next)
	{
	    if(!is_space(p->line[0])) 
	    {
		ok = FALSE;
		for(name=rfc_lvl_1; *name; name++)
		{
		    len  = strlen(*name);
		    if(!strnicmp(p->line, *name, len) && p->line[len]==':')
		    {
			ok = TRUE;		/* OK to output */
			break;
		    }
		}
	    }
	    if(ok)
		fprintf(out, "\001RFC-%s\r\n", p->line);
	}
    }
    
    /* RFC level 2 - all ^ARFC-Xxxx kludges */
    else if(rfc_level >= 2)
    {
	Textline *p;

	for(p=header.first; p; p=p->next)
	    fprintf(out, "\001RFC-%s\r\n", p->line);
    }
    
    return ferror(out);
}



/*
 * header_delete() --- Delete headers
 */
void header_delete(void)
{
    tl_clear(&header);
    last_header = NULL;
}



/*
 * header_read() --- read header lines from file
 */
void header_read(FILE *file)
{
    char buffer[BUFSIZ];
    
    tl_clear(&header);

    while(read_line(buffer, BUFSIZ, file))
    {
	if(buffer[0] == '\n')
	    break;
	strip_crlf(buffer);
	tl_append(&header, buffer);
    }
}



/*
 * header_hops() --- return # of hops (Received headers) of message
 */
int header_hops(void)
{
    char *name = "Received";
    Textline *p;
    int len, hops;
    
    len  = strlen(name);
    hops = 0;
    
    for(p=header.first; p; p=p->next)
	if(!strnicmp(p->line, name, len) && p->line[len]==':')
	    hops++;

    return hops;
}



/*
 * header_get() --- get header line
 */
char *rfcheader_get(Textlist *tl, char *name)
{
    Textline *p;
    int len;
    char *s;
    
    len = strlen(name);
    
    for(p=tl->first; p; p=p->next)
    {
	if(!strnicmp(p->line, name, len) && p->line[len]==':')
	{
	    for(s=p->line+len+1; is_space(*s); s++) ;
	    last_header = p;
	    return s;
	}
    }
    
    last_header = NULL;
    return NULL;
}



/*
 * header_get() --- get header line
 */
char *header_get(char *name)
{
    return rfcheader_get(&header, name);
}



/*
 * header_getnext() --- get next header line
 */

char *header_getnext(void)
{
    char *s;
    
    if(last_header == NULL)
	return NULL;

    last_header = last_header->next;
    if(last_header == NULL)
	return NULL;
    if(!is_space(last_header->line[0])) 
    {
	last_header = NULL;
	return NULL;
    }
    
    for(s=last_header->line; is_space(*s); s++) ;
    return s;
}



/*
 * Return complete header line, concat continuation lines if necessary.
 */
char *header_getcomplete(char *name)
{
    char *p;
    int len, rest, l;
    
    if((p = header_get(name)))
    {
	SHUFFLEBUFFERS;
	rest = MAX_CONVERT_BUFLEN;
	len  = 0;

	do 
	{
	    l    = strlen(p);
	    if(l > rest)
		l = rest - 1;
	    strncpy0(tcharp+len, p, rest);
	    len  += l;
	    rest -= l;

	    if((p = header_getnext()))
	    {
		if(rest > 1)
		{
		    tcharp[len++] = ' ';
		    tcharp[len]   = 0;
		    rest--;
		}
	    }
	}
	while(p);

	return tcharp;
    }

    return NULL;
}



/*
 * name_from_rfcaddr() --- get full name from RFC822 address
 *
 * Supported formats:
 *     user@domain
 *     user@domain (Full Name)
 *     Full Name <user@domain>
 */

char *name_from_rfcaddr(char *field, char *name)
{
    char *p, *q, *r, *s;
    
    if((p=strchr(field, '<')) && (r=strchr(p+1, '>')))
    {
	/*
	 * Full name <user@domain>
	 */
	for(s=field; is_space(*s); s++) ;
	if(!*s || *s=='<') 
	{
	    return NULL;
	}
	for(p=s; *p && *p!='<'; p++)
	{
	    if(is_space(*p))
	    {
		for(q=p; is_space(*q); q++) ;
		if(!*q || *q == '<')
		    break;
	    }
	}
	if(s[0]	 == '\"')
	    s++;
	if(p[-1] == '\"')
	    p--;
	for(; *s && s<p; *name++ = *s++) ;
	*name = 0;
	return name;
    }
    
    if((p=strchr(field, '(')) && (r=strrchr(p+1, ')')))
    {
	/*
	 * user@domain (Full name)
	 */
	for(p++; *p && p<r; *name++ = *p++) ;
	*name = 0;
	return name;
    }
    
    return NULL;
}


	   
/*
 * addr_from_rfcaddr() --- get address from RFC822 address
 *
 * Supported formats:
 *     user@domain
 *     user@domain (Full Name)
 *     Full Name <user@domain>
 */

char *addr_from_rfcaddr(char *field, char *addr)
{
    char *p, *q, *r, *s;
    
    if((p=strchr(field, '<')) && (r=strchr(p+1, '>')))
    {
	/*
	 * Full name <user@domain>
	 */
	for(p++; *p && p<r; *addr++ = *p++) ;
	*addr = 0;
	return addr;
    }
    
    if((p=strchr(field, '(')) && (r=strrchr(p+1, ')')))
    {
	/*
	 * user@domain (Full name)
	 */
	for(s=field; is_space(*s); s++) ;
	if(!*s || *s=='(') 
	{
	    return NULL;
	}
	for(p=s; *p && *p!='('; p++)
	{
	    if(is_space(*p))
	    {
		for(q=p; is_space(*q); q++) ;
		if(!*q || *q == '(')
		    break;
	    }
	}
	if(s[0]	 == '\"')
	    s++;
	if(p[-1] == '\"')
	    p--;
	for(; *s && s<p; *addr++ = *s++) ;
	*addr = 0;
	return addr;
    }
    
    for(s=field; is_space(*s); s++) ;
    for(; *s; *addr++ = *s++) ;
    *addr = 0;
    return addr;
}



/*
 * username_from_rfcaddr() --- get user name from an RFC address
 */

char *username_from_rfcaddr(char *field, char *name)
{
    char *p;

    if((p = strrchr(field, '!')))
    {
	/*
	 * Address contains bang, user name is string following last '!'
	 * char to separator.
	 */
	p++;
    }
    else 
    {
	/*
	 * No bang addressing, copy till end or '@' char
	 */
	p = field;
	/* If address is <user@do.main> skip `<' */
	if(*p == '<')
	    p++;
    }
    
    for(; *p && *p!='@' && !is_space(*p); *name++ = *p++) ;
    *name = 0;
    return name;
}



/*
 * addr_token() --- get addresses from string (',' separated)
 */

char *addr_token(char *line)
{
    static char *save_line = NULL;
    static char *save_p	   = NULL;
    int level;
    char *s, *p;
    
    if(line)
    {
	/*
	 * First call
	 */
	xfree(save_line);
	save_p = save_line = strsave(line);
    }

    if(save_p == NULL)
	return NULL;
    if(!*save_p) 
    {
	save_p = NULL;
	return NULL;
    }
    
    level = 0;
    for(p=s=save_p; *p; p++)
    {
	if(*p == '(')
	    level++;
	if(*p == ')')
	    level--;
	if(*p == ',' && level <= 0)
	    break;
    }
    if(*p)
	*p++ = 0;
    save_p = p;
	
    return s;
}   
