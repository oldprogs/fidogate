/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: kludge.c,v 4.0 1996/04/17 18:17:39 mj Exp $
 *
 * Processing of FTN ^A kludges in message body
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



/*
 * Process the addressing kludge lines in the message body:
 * ^ATOPT, ^AFMPT, ^AINTL. Remove these kludges from MsgBody and put
 * the information in Message.
 */
void kludge_pt_intl(MsgBody *body, Message *msg)
{
    Textline *line;
    Textlist *list;
    char *p, *s;
    Node node;
    
    list = &body->kludge;
    
    /* ^AINTL */
    if( (p = kludge_get(list, "INTL", &line)) )
    {
	/* Retrieve addr from ^AINTL, strtok() will destroy it, but we're */
	/* deleting this kludge anyway.                                   */
	if( (s = strtok(p, " \t\r\n")) )	/* Destination */
	    if( asc_to_node(s, &node, FALSE) == OK )
		msg->node_to = node;
	if( (s = strtok(NULL, " \t\r\n")) )	/* Source */
	    if( asc_to_node(s, &node, FALSE) == OK )
		msg->node_from = node;

	tl_delete(list, line);
    }
    
    /* ^AFMPT */
    if( (p = kludge_get(list, "FMPT", &line)) )
    {
	msg->node_from.point = atoi(p);

	tl_delete(list, line);
    }
    
    /* ^ATOPT */
    if( (p = kludge_get(list, "TOPT", &line)) )
    {
	msg->node_to.point = atoi(p);

	tl_delete(list, line);
    }
}



/*
 * Get a kludge line from a Textlist of all kludge lines.
 */
char *kludge_get(Textlist *tl, char *name, Textline **ptline)
{
    Textline *p;
    char *s, *r;
    int len;

    len = strlen(name);
    
    for(p=tl->first; p; p=p->next)
    {
	s = p->line;
	if(s[0] == '\001'                     &&
	   !strncmp(s+1, name, len)           &&
	   ( s[len+1]==' ' || s[len+1]==':' )    )	/* Found it */
	{
	    r = s + 1 + len;
	    /* Skip ':' and white space */
	    if(*r == ':')
		r++;
	    while( is_space(*r) )
		r++;
	    if(ptline)
		*ptline = p;
	    return r;
	}
    }
    
    /* Not found */
    if(ptline)
	*ptline = NULL;
    return NULL;
}
