/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: routing.c,v 4.2 1996/05/11 15:05:36 mj Exp $
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
    if(znfp_parse_diff(p, &src, &old) == ERROR)
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
    if(znfp_parse_diff(p, &dest, &old) == ERROR)
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
    if(znfp_parse_diff(p, &src, &old) == ERROR)
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
    if(znfp_parse_diff(p, &dest, &old) == ERROR)
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
	    if(znfp_parse_diff(p, &node, &old) == ERROR)
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
    if(pat->zone !=EMPTY &&  pat->zone !=WILDCARD && node->zone !=pat->zone )
	return FALSE;		       	 	     		  
    if(pat->net  !=EMPTY &&  pat->net  !=WILDCARD && node->net  !=pat->net  )
	return FALSE;		       	 	     		  
    if(pat->node !=EMPTY &&  pat->node !=WILDCARD && node->node !=pat->node )
	return FALSE;		       	 	     		  
    if(pat->point!=EMPTY &&  pat->point!=WILDCARD && node->point!=pat->point)
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
