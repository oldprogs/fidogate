/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: aliases.c,v 4.0 1996/04/17 18:17:38 mj Exp $
 *
 * Read user name aliases from file. The alias.users format is as follows:
 *	username    Z:N/F.P    Full Name
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



/*
 * Local prototypes
 */
static int anodeeq	    (Node *, Node *);


/*
 * Alias list
 */
static Alias *alias_list = NULL;
static Alias *alias_last = NULL;



/*
 * Read list of aliases from LIBDIR/ALIASES file.
 *
 * Format:
 *     ALIAS	NODE	"FULL NAME"
 */
void alias_init(void)
{
    FILE *fp;
    Alias *p;

    debug(14, "Reading aliases file");
    
    fp = libfopen(ALIASES, R_MODE);

    while(cf_getline(buffer, BUFFERSIZE, fp))
    {
	char *u, *n, *f;
	Node node;
	
	u = xstrtok(buffer, " \t");	/* User name */
	n = xstrtok(NULL,   " \t");	/* FTN node */
	f = xstrtok(NULL,   " \t");	/* Full name */
	if(u==NULL || n==NULL || f==NULL)
	    continue;

	if( asc_to_node(n, &node, FALSE) == ERROR )
	{
	    log("hosts: illegal FTN address %s", n);
	    continue;
	}

	p = (Alias *)xmalloc(sizeof(Alias));
	p->next     = NULL;
	p->node     = node;
	p->username = strsave(u);
	p->fullname = strsave(f);
	
	debug(15, "aliases: %s %s %s", p->username, 
	      node_to_asc(&p->node, TRUE), p->fullname);
	
	/*
	 * Put into linked list
	 */
	if(alias_list)
	    alias_last->next = p;
	else
	    alias_list       = p;
	alias_last       = p;
    }
    
    fclose(fp);
}



/*
 * Lookup alias in alias_list
 *
 * Parameters:
 *     node, username, NULL     --- lookup by FTN node and username
 *     node, NULL    , fullname --- lookup by FTN node and fullname
 *
 * The lookup for node is handled somewhat special: if node->point !=0,
 * then the alias matching the complete address including point will
 * be found. If not, then the alias comparison ignores the point address.
 * e.g.:
 *     mj    2:2452/110.1    "Martin Junius"
 * An alias_lookup(2:2452/110.1, "mj", NULL) as well as
 * alias_lookkup(2:2452/110, "mj", NULL) will return this alias entry.
 */
Alias *alias_lookup(Node *node, char *username, char *fullname)
{
    Alias *a;
    
    for(a=alias_list; a; a=a->next)
    {
	if(username)
	    if(!stricmp(a->username, username) && anodeeq(node, &a->node))
		return a;
	if(fullname)
	    if(!stricmp(a->fullname, fullname) && anodeeq(node, &a->node))
		return a;
    }
    
    return NULL;
}



/*
 * anodeeq() --- compare node adresses
 *
 * Special point treatment: if a->point != 0 and b->point !=0, compare the
 * complete FTN address including point. If not, compare ignoring the point
 * address.
 */
static int anodeeq(Node *a, Node *b)
            			/* Sender/receiver address */
            			/* ALIASES address */
{
    return a->point && b->point
	   ?
	   a->zone==b->zone && a->net==b->net && a->node==b->node &&
	   a->point==b->point
	   :
	   a->zone==b->zone && a->net==b->net && a->node==b->node
	   ;
}
