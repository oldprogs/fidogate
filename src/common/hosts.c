/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: hosts.c,v 4.0 1996/04/17 18:17:39 mj Exp $
 *
 * Process hostname <-> node aliases from hosts file
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
 * Hosts list
 */
static Host *host_list = NULL;
static Host *host_last = NULL;



/*
 * Read list of hosts from LIBDIR/HOSTS file.
 *
 * Format:
 *     NODE	NAME	[-options]
 *
 * Options:
 *     -p	Addresses with pX point address
 *     y	dito, old compatibility
 *     -d	Host down
 */
void hosts_init(void)
{
    FILE *fp;
    Host *p;

    debug(14, "Reading hosts file");
    
    fp = libfopen(HOSTS, R_MODE);

    while(cf_getline(buffer, BUFFERSIZE, fp))
    {
	char *f, *n, *o;
	Node node;
	
	f = strtok(buffer, " \t");	/* FTN address */
	n = strtok(NULL,   " \t");	/* Internet address */
	if(f==NULL || n==NULL)
	    continue;

	if( asc_to_node(f, &node, FALSE) == ERROR )
	{
	    log("hosts: illegal FTN address %s", f);
	    continue;
	}

	p = (Host *)xmalloc(sizeof(Host));
	p->next  = NULL;
	p->node  = node;
	p->flags = 0;
	if(!strcmp(n, "-"))		/* "-" == registered, but no name */
	    p->name = NULL;
	else
	{
	    if(n[strlen(n)-1] == '.')	/* FQDN in HOSTS */
	    {
		n[strlen(n)-1] = 0;
		p->name = strsave(n);
	    }
	    else			/* Add domain */
	    {
		char *dom = cf_hostsdomain();
		int l = strlen(n) + strlen(dom);
		p->name = xmalloc(l + 1);
		strcpy(p->name, n);
		strcat(p->name, dom);
	    }
	}
	
	for(o=strtok(NULL, " \t");	/* Options */
	    o;
	    o=strtok(NULL, " \t")  )
	{
	    if(!strcmp(o, "y"))
	    {
		/* y == -p */
		p->flags |= HOST_POINT;
	    }
	    if(!strcmp(o, "-p"))
	    {
		/* -p */
		p->flags |= HOST_POINT;
	    }
	    if(!strcmp(o, "-d"))
	    {
		/* -d */
		p->flags |= HOST_DOWN;
	    }
	}

	debug(15, "hosts: %s %s %02x", node_to_asc(&p->node, TRUE),
	      p->name, p->flags);
	
	/*
	 * Put into linked list
	 */
	if(host_list)
	    host_last->next = p;
	else
	    host_list       = p;
	host_last       = p;
    }
    
    fclose(fp);
}



/*
 * Lookup node/host in host_list
 *
 * Parameters:
 *     node, NULL     --- lookup by FTN address
 *     NULL, name     --- lookup by Internet address
 */
Host *hosts_lookup(Node *node, char *name)
{
    Host *p;
    
    /*
     * FIXME: the search method should use hashing or similar
     */

    for(p=host_list; p; p=p->next)
    {
	if(node)
	    if(node->zone==p->node.zone &&
	       node->net ==p->node.net  &&
	       node->node==p->node.node &&
	       (node->point==p->node.point || p->node.point==0) )
		return p;
	if(name && p->name && !stricmp(name, p->name))
	    return p;
    }
    
    return NULL;
}
