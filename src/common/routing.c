/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: routing.c,v 4.1 1996/04/22 14:31:13 mj Exp $
 *
 * Routing config file reading for ftntoss and ftnroute.
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



Routing *routing_first = NULL;
Routing *routing_last  = NULL;

Remap   *remap_first   = NULL;
Remap   *remap_last    = NULL;

Rewrite *rewrite_first = NULL;
Rewrite *rewrite_last  = NULL;



/*
 * Return: >=0 number, -1 error, -2 "*" or "All"
 */
int get_rnumber(char **ps)
{
    char *s = *ps;
    int val = 0;

    if(*s == '*')
    {
	s++;
	val = -2;
    }
    else if(!strnicmp(s, "all", 3))
    {
	s += 3;
	val = -2;
    }
    else if(!isdigit(*s))
	return ERROR;
    else
	while(isdigit(*s))
	    val = val*10 + *s++ - '0';

    *ps = s;
    
    return val;
}



/*
 * Convert partial Z:N/F.P address to Node, allowing "*" or "all" as wildcard
 */
int asc_to_rnode_partial(char *asc, Node *node)
{
    Node n;
    char *s = asc;
    int val1, val;

    val1    = -1;
    node_invalid(&n);

    if(!*s)
	return ERROR;

    if(*s != '.')
	if( (val1 = get_rnumber(&s)) == ERROR )	/* First number first */
	    return ERROR;

    if(*s == ':')			/* zone followed by net */
    {
	s++;
	if(val1 != -1)
	{
	    n.zone = val1;
	    val1   = -1;
	}
	if( (val = get_rnumber(&s)) == ERROR )
	    return ERROR;
	n.net = val;
    }
    if(*s == '/')			/* net followed by node */
    {
	s++;
	if(val1 != -1)
	{
	    n.net = val1;
	    val1  = -1;
	}
	if( (val = get_rnumber(&s)) == ERROR )
	    return ERROR;
	n.node = val;
    }
    if(*s == '.')			/* node followed by point */
    {
	s++;
	if(val1 != -1)
	{
	    n.node = val1;
	    val1   = -1;
	}
	if( (val = get_rnumber(&s)) == ERROR )
	    return ERROR;
	n.point = val;
    }
    if(val1 != -1)			/* Single number is node */
	n.node = val1;
    
    if(*s == '@')			/* Domain address may follow */
    {
	s++;
	strncpy0(n.domain, s, sizeof(n.domain));
    }
    else if(*s)
	return ERROR;

    *node = n;
    return OK;
}



/*
 * Convert partial Z:N/F.P address to Node, using previous node address
 */
int asc_to_rnode_diff(char *asc, Node *node, Node *oldnode)
{
    if(asc_to_rnode_partial(asc, node) == ERROR)
	return ERROR;

    /*
     * "*" alone means all'n'all
     */
    if(node->zone==-1 && node->net==-1 && node->node==-2 && node->point==-1)
    {
	node->node = -1;
	return OK;
    }
    
    if(node->zone == -1)
    {
	/* No zone, use old zone address */
	node->zone = oldnode->zone;
	if(node->net == -1)
	{
	    /* No net, use old net address */
	    node ->net = oldnode->net;
	    if(node->node == -1)
	    {
		node->node = oldnode->node;
	    }
	}
    }
    
    if(node->zone==-2)
	node->zone = -1;
    if(node->net==-2)
	node->net = -1;
    if(node->node==-2)
	node->node = -1;
    if(node->point==-2)
	node->point = -1;
    
    return OK;
}



/*
 * Parse routing command keyword or "NetMail"/"EchoMail"
 */
int parse_cmd(char *s)
{
    if(!stricmp(s, "netmail"))
	return TYPE_NETMAIL;
    if(!stricmp(s, "echomail"))
	return TYPE_ECHOMAIL;
    
    if(!stricmp(s, "send"))
	return CMD_SEND;
    if(!stricmp(s, "route"))
	return CMD_ROUTE;
    if(!stricmp(s, "change"))
	return CMD_CHANGE;
    if(!stricmp(s, "hostroute"))
	return CMD_HOSTROUTE;
    if(!stricmp(s, "hubroute"))
	return CMD_HUBROUTE;
    if(!stricmp(s, "remap"))
	return CMD_REMAP;
    if(!stricmp(s, "rewrite"))
	return CMD_REWRITE;
    if(!stricmp(s, "sendmove"))
	return CMD_SENDMOVE;
    return ERROR;
}



/*
 * Parse flavor keyword
 */
int parse_flav(char *s)
{
    if(!stricmp(s, "normal") || !stricmp(s, "out"))
	return FLAV_NORMAL;
    if(!stricmp(s, "hold")   || !stricmp(s, "hut"))
	return FLAV_HOLD;
    if(!stricmp(s, "crash")  || !stricmp(s, "cut"))
	return FLAV_CRASH;
    if(!stricmp(s, "direct") || !stricmp(s, "dut"))
	return FLAV_DIRECT;
    return ERROR;
}



/*
 * Process "remap" and "rewrite" entries in ROUTING config file
 */
void routing_remap		(void);
void routing_rewrite		(void);

void routing_remap(void)
{
    char *p;
    Node old, src, dest;
    Remap *r;
    
    node_invalid(&old);
    old.zone = cf_zone();

    /*
     * Source node (pattern)
     */
    p = xstrtok(NULL, " \t");
    if(!p)
    {
	log("remap: source node address missing");
	return;
    }
    if(asc_to_rnode_diff(p, &src, &old) == ERROR)
    {
	log("remap: illegal node address %s", p);
	return;
    }
    
    /*
     * Destination node (pattern)
     */
    p = xstrtok(NULL, " \t");
    if(!p)
    {
	log("remap: dest node address missing");
	return;
    }
    if(asc_to_rnode_diff(p, &dest, &old) == ERROR)
    {
	log("remap: illegal node address %s", p);
	return;
    }

    /*
     * Name pattern
     */
    p = xstrtok(NULL, " \t");
    if(!p)
    {
	log("remap: name missing");
	return;
    }
    
    /*
     * Create new entry and put into list
     */
    r = (Remap *)xmalloc(sizeof(Remap));
    r->from = src;
    r->to   = dest;
    r->name = strsave(p);
    r->next = NULL;
    
    if(remap_first)
	remap_last->next = r;
    else
	remap_first      = r;
    remap_last = r;
    
    debug(9, "remap: from=%s to=%s name=%s",
	  node_to_asc(&r->from, TRUE), node_to_asc(&r->to, TRUE), r->name);
}


void routing_rewrite(void)
{
    char *p;
    Node old, src, dest;
    Rewrite *r;
    
    node_invalid(&old);
    old.zone = cf_zone();

    /*
     * Source node (pattern)
     */
    p = xstrtok(NULL, " \t");
    if(!p)
    {
	log("rewrite: source node address missing");
	return;
    }
    if(asc_to_rnode_diff(p, &src, &old) == ERROR)
    {
	log("rewrite: illegal node address %s", p);
	return;
    }
    
    /*
     * Destination node (pattern)
     */
    p = xstrtok(NULL, " \t");
    if(!p)
    {
	log("rewrite: dest node address missing");
	return;
    }
    if(asc_to_rnode_diff(p, &dest, &old) == ERROR)
    {
	log("rewrite: illegal node address %s", p);
	return;
    }

    /*
     * Create new entry and put into list
     */
    r = (Rewrite *)xmalloc(sizeof(Rewrite));
    r->from = src;
    r->to   = dest;
    r->next = NULL;
    
    if(rewrite_first)
	rewrite_last->next = r;
    else
	rewrite_first      = r;
    rewrite_last = r;
    
    debug(9, "rewrite: from=%s to=%s",
	  node_to_asc(&r->from, TRUE), node_to_asc(&r->to, TRUE) );
}



/*
 * Read ROUTING config file
 */
void routing_init(char *name)
{
    FILE *fp;
    Routing *r;
    char *p;
    int type=TYPE_NETMAIL;
    int cmd, flav, flav_new=FLAV_NONE;
    Node old, node;
    LON lon;
    
    fp = xfopen(name, R_MODE);

    while(cf_getline(buffer, BUFFERSIZE, fp))
    {
	/*
	 * Command
	 */
	p = xstrtok(buffer, " \t");
	if(!p)
	{
	    log("routing: empty line, strange");
	    continue;
	}
	if((cmd = parse_cmd(p)) == ERROR)
	{
	    log("routing: unknown command %s", p);
	    continue;
	}
	if(cmd == TYPE_NETMAIL  ||  cmd == TYPE_ECHOMAIL)
	{
	    type = cmd;
	    continue;
	}
	if(cmd == CMD_REMAP)
	{
	    routing_remap();
	    continue;
	}
	if(cmd == CMD_REWRITE)
	{
	    routing_rewrite();
	    continue;
	}
	
	/*
	 * Target flavor
	 */
	p = xstrtok(NULL, " \t");
	if(!p)
	{
	    log("routing: flavor argument missing");
	    continue;
	}
	if((flav = parse_flav(p)) == ERROR)
	{
	    log("routing: unknown flavor %s", p);
	    continue;
	}

	/*
	 * Second flavor arg for CHANGE command
	 */
	if(cmd == CMD_CHANGE)
	{
	    p = xstrtok(NULL, " \t");
	    if(!p)
	    {
		log("routing: second flavor argument missing");
		continue;
	    }
	    if((flav_new = parse_flav(p)) == ERROR)
	    {
		log("routing: unknown flavor %s", p);
		continue;
	    }
	}

	/*
	 * List of nodes follows, using "*" or "all" wildcard pattern
	 */
	node_invalid(&old);
	old.zone = cf_zone();
	lon_init(&lon);
	
	p = xstrtok(NULL, " \t");
	if(!p)
	{
	    log("routing: node address argument missing");
	    continue;
	}
	while(p)
	{
	    if(asc_to_rnode_diff(p, &node, &old) == ERROR)
	    {
		log("routing: illegal node address %s", p);
	    }
	    else
	    {
		old = node;
		lon_add(&lon, &node);
	    }
	    
	    p = xstrtok(NULL, " \t");
	}


	/*
	 * Create new entry and put into list
	 */
	r = (Routing *)xmalloc(sizeof(Routing));
	r->type     = type;
	r->cmd      = cmd;
	r->flav     = flav;
	r->flav_new = flav_new;
	r->nodes    = lon;
	r->next     = NULL;
	
	if(routing_first)
	    routing_last->next = r;
	else
	    routing_first      = r;
	routing_last = r;
	
	debug(9, "routing: type=%c cmd=%c flav=%c flav_new=%c",
	      r->type, r->cmd, r->flav, r->flav_new            );
	lon_debug(9, "routing: nodes=", &r->nodes, TRUE);

    }

    fclose(fp);
}



/*
 * Compare nodes using wildcard pattern
 */
int node_match(Node *node, Node *pat)
{
    if(pat->zone  != -1  &&  node->zone  != pat->zone )
	return FALSE;
    if(pat->net   != -1  &&  node->net   != pat->net  )
	return FALSE;
    if(pat->node  != -1  &&  node->node  != pat->node )
	return FALSE;
    if(pat->point != -1  &&  node->point != pat->point)
	return FALSE;
    return TRUE;
}



/*
 * Parse packet name to PktDesc struct
 */
PktDesc *parse_pkt_name(char *name, Node *from, Node *to)
{
    static PktDesc desc;
    char *p;

    p = strrchr(name, '/');
    p = p ? p+1 : name;
    
    if(strlen(p) != 8+1+3)		/* Must be  GTFxxxxx.pkt */
    {
	log("strange packet name %s", name);
	return NULL;
    }
    
    desc.from      = *from;
    desc.to        = *to;
    desc.grade     = p[0];
    desc.type      = p[1];
    desc.flav      = p[2];
    desc.move_only = FALSE;
    
    return &desc;
}
