/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftntoss.c,v 4.16 1997/03/30 16:25:20 mj Exp $
 *
 * Toss FTN NetMail/EchoMail
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
#include "getopt.h"

#include <fcntl.h>
#include <signal.h>



#define PROGRAM 	"ftntoss"
#define VERSION 	"$Revision: 4.16 $"
#define CONFIG		CONFIG_MAIN



/*
 * Prototypes
 */
void	addtoseenby_init	(void);
void	zonegate_init		(void);
int	lon_to_kludge		(Textlist *, char *, LON *);
int	lon_to_kludge_sorted	(Textlist *, char *, LON *);
int	toss_echomail		(Message *, MsgBody *, LON *, LON *, LON *);
int	kludge_to_lon		(Textlist *, LON *);
int	node_eq_echo		(Node *, Node *);
int	node_eq_echo_pa		(Node *, Node *);
int	lon_search_echo		(LON *, Node *);
int	is_local_addr		(Node *, int);
int	do_seenby		(LON *, LON *, LON *);
int	do_path			(LON *);
int	do_bad_msg		(Message *, MsgBody *);
Node   *lon_first		(LON *);
int	do_4dpoint		(LON *, LON *, Node *, Node *);
int	do_echomail		(Packet *, Message *, MsgBody *);
void	add_via			(Textlist *, Node *);
void	change_addr		(Node *, Node *);
void	do_rewrite		(Message *);
int	do_remap		(Message *);
int	check_empty		(MsgBody *);
int	do_netmail		(Packet *, Message *, MsgBody *);
int	unpack			(FILE *, Packet *);
int	rename_bad		(char *);
int	unpack_file		(char *);
void	prog_signal		(int);
void	short_usage		(void);
void	usage			(void);



/*
 * Command line options
 */
int g_flag = 'n';			/* Processing grade */
int t_flag = FALSE;			/* Insecure tossing enabled */
int n_flag = FALSE;			/* Accept EchoMail messages not */
					/* addressed to our AKA         */
int s_flag = FALSE;			/* Strip CRASH, HOLD attribute */
int maxmsg = 0;				/* Process maxmsg messages */
int x_flag = FALSE;			/* Exit after maxmsg messages */
int l_flag = FALSE;			/* Create lock file */
int p_flag = FALSE;			/* -p --passthru */

static char in_dir[MAXPATH];		/* Input directory */


static int must_exit = FALSE;		/* Flag for -x operation */
static int msg_count = 0;		/* Counter for -m, -x operation */

static int severe_error = OK;		/* ERROR: exit after error */

static int signal_exit = FALSE;		/* Flag: TRUE if signal received */



/*
 * Config options
 */
int kill_empty    = FALSE;		/* config: KillEmpty */
int kill_unknown  = FALSE;		/* config: KillUnknown */
int kill_routed   = FALSE;		/* config: KillRouted */
int kill_insecure = FALSE;		/* config: KillInsecure */
int kill_circular = FALSE;		/* config: KillCircular */
int log_netmail   = FALSE;		/* config: LogNetMail */
int check_path    = FALSE;		/* config: CheckPath */
int dupe_check    = FALSE;		/* config: DupeCheck */
int kill_dupe     = FALSE;		/* config: KillDupe */
int kill_nomsgid  = FALSE;		/* config: KillNoMSGID */
int kill_old      = FALSE;		/* config: KillOld */
int echomail4d    = FALSE;		/* config: EchoMail4D */ 
int no_empty_path = FALSE;		/* config: NoEmptyPath */ 

/* Values checking for old messages */
static double max_history = 14;		/* Max. number of days entry stays
					   in MSGID history database */
static time_t max_sec = 0;
static time_t now_sec = 0;
static time_t exp_sec = 0;


/*
 * Global stat counts
 */
static long msgs_in       = 0;		/* Input messages */
static long msgs_netmail  = 0;		/* Output messages NetMail */
static long msgs_echomail = 0;		/* Output messages EchoMail */
static long msgs_routed   = 0;		/* Routed EchoMail messages */
static long msgs_insecure = 0;		/* Insecure EchoMail messages */
static long msgs_unknown  = 0;		/* Unknown EchoMail area messages */
static long msgs_empty    = 0;		/* Empty NetMail messages */
static long msgs_path     = 0;		/* Circular path */
static long msgs_dupe     = 0;		/* Dupe */

static long pkts_in       = 0;		/* Input packets */
static long pkts_bytes    = 0;		/* Input bytes */



/*
 * AddToSeenBy list
 */
typedef struct st_addtoseenby
{
    char *area;				/* Area pattern */
    LON add;				/* Nodes to add to SEEN-BY */
    struct st_addtoseenby *next;
}
AddToSeenBy;

static AddToSeenBy *addto_first = NULL;
static AddToSeenBy *addto_last  = NULL;



/*
 * Init AddToSeenBy list from config file
 */
void addtoseenby_init(void)
{
    char *s, *parea, *pnodes;
    AddToSeenBy *p;
    
    for(s = cf_get_string("AddToSeenBy",TRUE);
	s && *s;
	s = cf_get_string("AddToSeenBy",FALSE) )
    {
	debug(8, "config: addtoseenby %s", s);
	
	BUF_COPY(buffer, s);
	parea  = xstrtok(buffer, " \t");
	pnodes = xstrtok(NULL  , "\n");

	if(!parea || !pnodes)
	    continue;
	    
	p = (AddToSeenBy *)xmalloc(sizeof(AddToSeenBy));
	p->next = NULL;
	p->area = strsave(parea);
	lon_init(&p->add);
	lon_add_string(&p->add, pnodes);

	if(addto_first)
	    addto_last->next = p;
	else
	    addto_first = p;
	addto_last = p;
    }
}

    

/*
 * Zonegate list
 */
typedef struct st_zonegate
{
    Node node;
    LON seenby;
    struct st_zonegate *next;
}
ZoneGate;

static ZoneGate *zonegate_first = NULL;
static ZoneGate *zonegate_last  = NULL;



/*
 * Init zonegate list
 */
void zonegate_init(void)
{
    char *s;
    ZoneGate *p;
    
    for(s = cf_get_string("ZoneGate",TRUE);
	s && *s;
	s = cf_get_string("ZoneGate",FALSE) )
    {
	debug(8, "config: zonegate %s", s);
	
	p = (ZoneGate *)xmalloc(sizeof(ZoneGate));
	p->next = NULL;
	lon_init(&p->seenby);
	lon_add_string(&p->seenby, s);
	if(p->seenby.first)
	{
	    p->node = p->seenby.first->node;
	    lon_remove(&p->seenby, &p->node);
	}
	else
	    node_invalid(&p->node);
	
	if(zonegate_first)
	    zonegate_last->next = p;
	else
	    zonegate_first = p;
	zonegate_last = p;
    }
}    


/*
 * Create new SEEN-BY or ^APATH line from LON
 */
#define MAX_LENGTH 76

int lon_to_kludge(Textlist *tl, char *text, LON *lon)
{
    LNode *p;
    Node old;
    char *s = NULL;
    int n = 0;
    
    BUF_COPY(buffer, text);
    node_invalid(&old);
    old.zone = cf_zone();

    for(p=lon->first; p; p=p->next)
	if(p->node.point==0 || echomail4d)	/* Normally, no 4D Points */
	{
	    p->node.zone = cf_zone();		/* No zone, use current one */
	    s = node_to_asc_diff(&p->node, &old);
	    old = p->node;
	    
	    if(strlen(buffer)+strlen(s)+1 > MAX_LENGTH)
	    {
		BUF_APPEND(buffer, "\r\n");
		tl_append(tl, buffer);
		
		node_invalid(&old);
		old.zone = cf_zone();
		
		s = node_to_asc_diff(&p->node, &old);
		old = p->node;
		BUF_COPY(buffer, text);
		BUF_APPEND(buffer, " ");
		BUF_APPEND(buffer, s);
	    }
	    else
	    {
		BUF_APPEND(buffer, " ");
		BUF_APPEND(buffer, s);
	    }

	    n++;
	}
    
    BUF_APPEND(buffer, "\r\n");
    tl_append(tl, buffer);

    return n;
}


int lon_to_kludge_sorted(Textlist *tl, char *text, LON *lon)
{
    Node old;
    char *s = NULL;
    int i;
    int n = 0;

    lon_sort(lon, 0);
    
    BUF_COPY(buffer, text);
    node_invalid(&old);
    old.zone = cf_zone();

    for(i=0; i<lon->size; i++)
	if(lon->sorted[i]->point==0 || echomail4d)/* Normally, no 4D Points */
	{
	    lon->sorted[i]->zone = cf_zone();	/* No zone, use current one */
	    s = node_to_asc_diff(lon->sorted[i], &old);
	    old = *lon->sorted[i];
	    
	    if(strlen(buffer)+strlen(s)+1 > MAX_LENGTH)
	    {
		strncat0(buffer, "\r\n", sizeof(buffer));
		tl_append(tl, buffer);
		
		node_invalid(&old);
		old.zone = cf_zone();
		
		s = node_to_asc_diff(lon->sorted[i], &old);
		old = *lon->sorted[i];
		BUF_COPY(buffer, text);
		BUF_APPEND(buffer, " ");
		BUF_APPEND(buffer, s);
	    }
	    else
	    {
		BUF_APPEND(buffer, " ");
		BUF_APPEND(buffer, s);
	    }

	    n++;
	}
    
    BUF_APPEND(buffer, "\r\n");
    tl_append(tl, buffer);

    return n;
}

    

/*
 * Toss EchoMail, writing message to packet for each downlink
 */
int toss_echomail(Message *msg, MsgBody *body,
		  LON *seenby, LON *path, LON *nodes)
{
    LNode *p;
    FILE *fp;
    Textlist save;
    int is_saved;
    
    for(p=nodes->first; p; p=p->next)
    {
	is_saved = FALSE;

	debug(7, "toss_echomail(): message for %s",
	      node_to_asc(&p->node, TRUE));
	
	/* Check for msg addressed to zonegate */
	if(zonegate_first)
	{
	    ZoneGate *pz;
	    
	    for(pz=zonegate_first; pz; pz=pz->next)
		if(node_eq(&p->node, &pz->node))
		{
		    debug(7, "toss_echomail(): message is for zonegate, "
			     "stripping SEEN-BYs");
		    
		    save = body->seenby;
		    tl_init(&body->seenby);
		    is_saved = TRUE;
		    lon_to_kludge_sorted(&body->seenby,
					 "SEEN-BY:", &pz->seenby);
		    break;
		}
	}

	/* Rewrite message header */
	msg->node_from = cf_n_addr();
	msg->node_to   = p->node;
	/* Open packet file, EchoMail, Normal */
	fp = outpkt_open(cf_addr(), &p->node, g_flag, 'e', 'n', FALSE);
	if(fp == NULL)
	    return severe_error=ERROR;
	/* Write message header and body */
	if( pkt_put_msg_hdr(fp, msg, FALSE) != OK )
	    return severe_error=ERROR;
	if( msg_put_msgbody(fp, body, TRUE) != OK )
	    return severe_error=ERROR;

	if(is_saved)
	{
	    tl_clear(&body->seenby);
	    body->seenby = save;
	}
	
	msgs_echomail++;
    }
    
    return OK;
}



/*
 * Convert SEEN-BY or ^APATH lines to LON
 */
int kludge_to_lon(Textlist *tl, LON *lon)
{
    Node old, node;
    Textline *l;
    char *p;

    for(l=tl->first; l; l=l->next)
    {
	p = l->line;
	/* Skip SEEN-BY:, ^APATH: or whatever, copy to buffer[] */
	while(*p && !is_space(*p) && *p!=':')
	    p++;
	if(*p == ':')
	    p++;
	while(*p && is_space(*p))
	    p++;
	strncpy0(buffer, p, sizeof(buffer));
	strip_crlf(buffer);
	/* Tokenize and convert to node addresses */
	node_invalid(&old);
	old.zone = cf_zone();
	for(p=strtok(buffer, " \t"); p; p=strtok(NULL, " \t"))
	    if(asc_to_node_diff(p, &node, &old) == OK)
	    {
		lon_add(lon, &node);
		old = node;
	    }
    }
	
    return OK;
}



/*
 * node_eq_echo() --- compare node adresses, ignoring zone
 */
int node_eq_echo(Node *a, Node *b)
{
    if(a==NULL || b==NULL)
	return FALSE;
    
    return
	a->net  == b->net  &&
	a->node == b->node && a->point ==b->point   ;
}



/*
 * node_eq_echo_pa() --- compare node adresses, ignoring zone and point
 *                  if a->point == 0.
 */
int node_eq_echo_pa(Node *a, Node *b)
{
    if(a==NULL || b==NULL)
	return FALSE;
    
    return
	a->net==b->net  &&  a->node==b->node  &&
	(a->point==0 || a->point==b->point);
}



/*
 * Search node in SEEN-BY list, ignoring zone
 */
int lon_search_echo(LON *lon, Node *node)
{
    LNode *p;
    
    for(p=lon->first; p; p=p->next)
	if(node_eq_echo(&p->node, node))
	    return TRUE;
    
    return FALSE;
}



/*
 * Check for local address
 */
int is_local_addr(Node *node, int only3d)
{
    Node *n;
    int found = FALSE;
    
    for(n=cf_addr_trav(TRUE); n; n=cf_addr_trav(FALSE))
	if(only3d ? node_eq_echo(node, n) : node_eq(node, n))
	{
	    found = TRUE;
	    break;
	}
    
    return found;
}



/*
 * Add nodes to SEEN-BY
 */
int do_seenby(LON *seenby, LON *nodes, LON *new)
    /* seenby --- nodes in SEEN-BY lines */
    /* nodes  --- nodes in AREAS.BBS */
    /* new    --- new nodes added */
{
    LNode *p;
    
    for(p=nodes->first; p; p=p->next)
	if(! lon_search_echo(seenby, &p->node) )
	{
	    lon_add(seenby, &p->node);
	    if(new)
		lon_add(new, &p->node);
	}
	    
    return OK;
}



/*
 * Add current address to ^APATH
 */
int do_path(LON *path)
{
    if(path->last && node_eq_echo(&path->last->node, cf_addr()))
	/* Already there */
	return OK;
    
    lon_add(path, cf_addr());
    return OK;
}



/*
 * Check ^APATH for circular path
 */
int do_check_path(LON *path)
{
    LNode *p;
    
    for(p=path->first; p; p=p->next)
	if(p->next)		/* Don't check last node in list */
	    if(is_local_addr(&p->node, TRUE))
		return ERROR;
    
    return OK;
}



/*
 * Save bad message
 */
int do_bad_msg(Message *msg, MsgBody *body)
{
    FILE *fp;

    /* Open packet file, EchoMail, Normal */
    fp = outpkt_open(&msg->node_from, &msg->node_to, 'b', 'a', 'd', TRUE);
    if(fp == NULL)
	return severe_error=ERROR;
    /* Write message header and body */
    if( pkt_put_msg_hdr(fp, msg, FALSE) != OK )
	return severe_error=ERROR;
    if( msg_put_msgbody(fp, body, TRUE) != OK )
	return severe_error=ERROR;
    
    return OK;
}



/*
 * First node in LON
 */
Node *lon_first(LON *lon)
{
    if(lon->first)
	return &lon->first->node;
    
    return NULL;
}



/*
 * Special SEEN-BY / PATH processing if message originates from a 4d point
 */
int do_4dpoint(LON *seenby, LON *path, Node *from, Node *addr)
{
    /*
     * If there is only one node in SEEN-BY, which is the same address
     * (2d) as the sender, then replace it with the sender 4d address.
     */
    if( seenby->size == 1 && node_eq_echo_pa(lon_first(seenby), from) )
	*lon_first(seenby) = *from;

    /*
     * Same for PATH
     */
    if( path->size == 1 && node_eq_echo_pa(lon_first(path), from) )
	*lon_first(path) = *from;

    /*
     * Now make sure that also our current AKA is in SEEN-BY
     */
    if( !lon_search_echo(seenby, cf_addr()) )
	lon_add(seenby, cf_addr());
    
    return OK;
}



/*
 * Process EchoMail message
 */
int do_echomail(Packet *pkt, Message *msg, MsgBody *body)
{
    AreasBBS *area;
    AddToSeenBy *addto;
    LON seenby, path, new;
    int ret, n;

    /*
     * Lookup area and set zone
     */
    strncpy0(buffer, body->area + strlen("AREA:"), sizeof(buffer));
    strip_crlf(buffer);
    debug(5, "EchoMail AREA: %s", buffer);
    
    if( (area = areasbbs_lookup(buffer)) == NULL )
    {
	/* Unknown area */
	log("unknown area %s from %s", buffer,
	    node_to_asc(&msg->node_from, TRUE) );
	msgs_unknown++;
	if(!kill_unknown)
	    return do_bad_msg(msg, body);
	return OK;
    }

    /* Set address for this area */
    if(area->zone != -1)
	cf_set_zone(area->zone);
    if(area->addr.zone != -1)
	cf_set_curr(&area->addr);

    
    /*
     * Dupe check
     */
    if(dupe_check)
    {
	char *p, *msgid;
	Textline *tl;
	
	if( (p = kludge_get(&body->kludge, "MSGID", &tl)) )
	{
	    /* ^AMSGID */
	    p = tl->line;
	    if(*p == '\001')
		p++;
	    BUF_COPY3(buffer, area->area, " ", p);
	    strip_crlf(buffer);
	    msgid = buffer;
	    /* Replace char illegal for DBZ */
	    for(p=msgid; *p; p++)
		if(*p=='\t' || *p=='\r' || *p=='\n')
		    *p = '_';
	}
	else 
	{
	    /* If KillNoMSGID ... */
	    if(kill_nomsgid)
	    {
		log("no ^AMSGID treated as dupe from %s: %s",
		    node_to_asc(&msg->node_from, TRUE), area->area);
		msgs_dupe++;
		if(!kill_dupe)
		    return do_bad_msg(msg, body);
		return OK;
	    }

	    /* No ^AMSGID, use sender, date and checksum */
	    if(msg_parse_origin(body->origin, &msg->node_orig) == ERROR)
	    {
		log("invalid * Origin treated as dupe from %s: %s",
		    node_to_asc(&msg->node_from, TRUE), area->area);
		msgs_dupe++;
		if(!kill_dupe)
		    return do_bad_msg(msg, body);
		return OK;
	    }

	    /* Compute CRC for strings from, to, subject */
	    crc32_init();
	    crc32_compute(msg->name_from, strlen(msg->name_from));
	    crc32_compute(msg->name_to  , strlen(msg->name_to  ));
	    crc32_compute(msg->subject  , strlen(msg->subject  ));

	    sprintf(buffer, "%s NOMSGID: %s %s %08lx",
		    area->area, node_to_asc(&msg->node_orig, TRUE),
		    date("%y%m%d %H%M%S", &msg->date), crc32_value() );
	    msgid = buffer;
	}

	/* Check for old message (date < NOW - MaxHistory) */
	if(kill_old)
	{
	    if(msg->date < exp_sec)
	    {
		log("message too old, treated as dupe: %s origin %s date %s",
		    area->area, node_to_asc(&msg->node_orig, TRUE),
		    date(DATE_FTS_0001, &msg->date)                          );
		msgs_dupe++;
		if(!kill_dupe)
		    return do_bad_msg(msg, body);
		return OK;
	    }
	}
	
	/* Check for existence in MSGID history */
	if(hi_test(msgid))
	{
	    /* Dupe! */
	    log("dupe from %s: %s", node_to_asc(&msg->node_from, TRUE), msgid);
	    msgs_dupe++;
	    if(!kill_dupe)
		return do_bad_msg(msg, body);
	    return OK;
	}
	else
	{
	    /* Put into MSGID history */
	    if(hi_write(msg->date, msgid) == ERROR)
		return ERROR;
	}
    }

    
    /*
     * Lookup area in AddToSeenBy list
     */
    for(addto=addto_first; addto; addto=addto->next)
	if( wildmatch(area->area, addto->area, TRUE) )
	    break;
    
    /*
     * Check that this message is addressed to one of our AKAs
     */
    if(!n_flag)
    {
	if(! node_eq(&msg->node_to, cf_addr())   &&
	   ! is_local_addr(&msg->node_to, FALSE)   )
	{
	    /* Routed EchoMail */
	    log("routed echomail area %s from %s to %s", area->area,
		node_to_asc(&msg->node_from, TRUE),
		node_to_asc(&msg->node_to, TRUE)                    );
	    msgs_routed++;
	    if(!kill_routed)
		return do_bad_msg(msg, body);
	    return OK;
	}
    }
    
    /*
     * Check that origin is listed in AREAS.BBS
     */
    if(!t_flag)
    {
	if(! lon_search(&area->nodes, &msg->node_from) )
	{
	    /* Insecure EchoMail */
	    log("insecure echomail area %s from %s", area->area,
		node_to_asc(&msg->node_from, TRUE)              );
	    msgs_insecure++;
	    if(!kill_insecure)
		return do_bad_msg(msg, body);
	    return OK;
	}
    }
    
    /*
     * Convert SEEN-BY and ^APATH to LON
     */
    lon_init(&seenby);
    lon_init(&path);
    lon_init(&new);
    kludge_to_lon(&body->seenby, &seenby);
    kludge_to_lon(&body->path  , &path);

    /*
     * Do special processing if message is from a 4d point
     */
    do_4dpoint(&seenby, &path, &msg->node_from, cf_addr());
    
    lon_debug(9, "SEEN-BY: ", &seenby, TRUE);
    lon_debug(9, "Path   : ", &path,   TRUE);
    
    /*
     * Add nodes not already in SEEN-BY to seenby and new.
     */
    do_seenby(&seenby, &area->nodes, &new);
    /*
     * Add extra nodes to SEEN-BY
     */
    if(addto)
	do_seenby(&seenby, &addto->add, NULL);
    /*
     * If not passthru area and not from our own address (point gateway
     * setup with Address==Uplink), add our own address to new
     */
    if(! p_flag                               &&
       ! (area->flags & AREASBBS_PASSTHRU)    &&
       ! node_eq(&msg->node_from, cf_addr())    )
	lon_add(&new, cf_addr());
    /*
     * Add our address to end of ^APATH, if not already there.
     */
    do_path(&path);

    lon_debug(9, "SEEN-BY: ", &seenby, TRUE);
    lon_debug(9, "Path   : ", &path,   TRUE);
    lon_debug(9, "New    : ", &new,    TRUE);

    /*
     * Check for circular ^APATH
     */
    if(check_path && do_check_path(&path)==ERROR)
    {
	    /* Circular ^APATH EchoMail */
	    log("circular path echomail area %s from %s to %s",
		area->area, node_to_asc(&msg->node_from, TRUE),
		node_to_asc(&msg->node_to, TRUE)                 );
	    msgs_path++;

	    lon_delete(&seenby);
	    lon_delete(&path);
	    lon_delete(&new);

	    if(!kill_circular)
		return do_bad_msg(msg, body);
	    return OK;
    }
    
    /*
     * Create new SEEN-BY and ^APATH lines
     */
    tl_clear(&body->seenby);
    tl_clear(&body->path);
    n = lon_to_kludge_sorted(&body->seenby, "SEEN-BY:", &seenby);
    n = lon_to_kludge       (&body->path  , "\001PATH:", &path );

    if(no_empty_path && n==0)
	tl_clear(&body->path);
    
    /*
     * Send message to all downlinks in new
     */
    ret = toss_echomail(msg, body, &seenby, &path, &new);

    lon_delete(&seenby);
    lon_delete(&path);
    lon_delete(&new);

    return ret;
}



/*
 * Add our ^AVia line
 */
void add_via(Textlist *list, Node *gate)
{
    tl_appendf(list, "\001Via FIDOGATE/%s %s, %s\r\n",
		     PROGRAM, node_to_asc(gate, FALSE),
		     date("%a %b %d %Y at %H:%M:%S %Z", NULL)  );
}



/*
 * Change address according to rewrite pattern
 */
void change_addr(Node *node, Node *newpat)
{
    if(newpat->zone  != -1)
	node->zone  = newpat->zone;
    if(newpat->net   != -1)
	node->net   = newpat->net;
    if(newpat->node  != -1)
	node->node  = newpat->node;
    if(newpat->point != -1)
	node->point = newpat->point;
}



/*
 * Perform REWRITE commands
 */
void do_rewrite(Message *msg)
{
    Rewrite *r;
    Node node;
    
    /* From */
    for(r=rewrite_first; r; r=r->next)
	if(node_match(&msg->node_from, &r->from))
	{
	    node = msg->node_from;
	    change_addr(&msg->node_from, &r->to);
	    log("rewrite: %s -> %s",
		node_to_asc(&node, TRUE), node_to_asc(&msg->node_from, TRUE) );
	    break;
	}

    /* To */
    for(r=rewrite_first; r; r=r->next)
	if(node_match(&msg->node_to, &r->from))
	{
	    node = msg->node_to;
	    change_addr(&msg->node_to, &r->to);
	    log("rewrite: %s -> %s",
		node_to_asc(&node, TRUE), node_to_asc(&msg->node_to, TRUE) );
	    break;
	}
}



/*
 * Perform REMAP commands
 *
 * Return == TRUE: remapped to 0:0/0.0, kill message
 */
int do_remap(Message *msg)
{
    Remap *r;
    Node node;
    int kill = FALSE;
    
    for(r=remap_first; r; r=r->next)
	if(
	   (r->type==CMD_REMAP_TO                    &&
	    node_match(&msg->node_to, &r->from)      &&
	    wildmatch(msg->name_to, r->name, TRUE)     )
	   ||
	   (r->type==CMD_REMAP_FROM                  &&
	    node_match(&msg->node_from, &r->from)    &&
	    wildmatch(msg->name_from, r->name, TRUE)   )
	   )
	{
	    node = msg->node_to;
	    change_addr(&msg->node_to, &r->to);

	    if(msg->node_to.zone==0 && msg->node_to.net==0 &&
	       msg->node_to.node==0 && msg->node_to.point==0)
		kill = TRUE;
	    
	    if(! node_eq(&node, &msg->node_to))
	    {
		if(r->type == CMD_REMAP_TO)
		    log("remapto: %s @ %s -> %s", msg->name_to,
			node_to_asc(&node, TRUE),
			!kill ? node_to_asc(&msg->node_to, TRUE) : "KILLED");
		else
		    log("remapfrom: %s @ %s -> %s", msg->name_from,
			node_to_asc(&msg->node_from, TRUE),
			!kill ? node_to_asc(&msg->node_to, TRUE) : "KILLED");
	    }
	    
	    break;
	}

    return kill;
}



/*
 * Check for empty NetMail
 *
 * An empty NetMail is a message comprising only ^A kludges and empty lines.
 */
int check_empty(MsgBody *body)
{
    Textline *pl;
    
    if(body->rfc.n)
	return FALSE;
    if(body->tear || body->origin)
	return FALSE;
    if(body->seenby.n)
	return FALSE;

    for(pl=body->body.first; pl; pl=pl->next)
	if(pl->line[0] && pl->line[0]!='\r')
	    return FALSE;
    
    return TRUE;
}



/*
 * Process NetMail message
 */
int do_netmail(Packet *pkt, Message *msg, MsgBody *body)
{
    FILE *fp;
    int flav;


    if(log_netmail)
	log("netmail: %s @ %s -> %s @ %s",
	    msg->name_from, node_to_asc(&msg->node_from, TRUE),
	    msg->name_to  , node_to_asc(&msg->node_to  , TRUE) );

    /*
     * Check for file attach
     */
    if(msg->attr & MSG_FILE)
    {
	log("file attach %s", msg->subject);
    }
    
    /*
     * Check for empty NetMail message addressed to one of our AKAs
     */
    if(kill_empty && check_empty(body))
    {
	if( is_local_addr(&msg->node_to, FALSE) )
	{
	    log("killing empty msg from %s @ %s",
		msg->name_from, node_to_asc(&msg->node_from, TRUE));
	    msgs_empty++;
	    
	    return OK;
	}
    }
    
    /*
     * Rewrite from/to addresses according to ROUTING rules
     */
    do_rewrite(msg);
    
    /*
     * Remap to address according to ROUTING rules
     */
    if(do_remap(msg))
    {
	/* Kill this message, remapped to 0:0/0.0 */
	return OK;
    }
    
    
    /*
     * Write to output packet
     */
    cf_set_zone(msg->node_to.zone);

    /* Get outbound flavor from msg attributes */
    flav = 'n';
    if(!s_flag)
    {
	if(msg->attr & MSG_HOLD)
	    flav = 'h';
	if(msg->attr & MSG_CRASH)
	    flav = 'c';
    }

    /* Open output packet */
    fp = outpkt_open(cf_addr(), &msg->node_to, g_flag, 'n', flav, FALSE);
    if(fp == NULL)
	return severe_error=ERROR;

    /* Add ftntoss ^AVia line */
    add_via(&body->via, cf_addr());

    /* Write message header and body */
    if( pkt_put_msg_hdr(fp, msg, TRUE) != OK )
	return severe_error=ERROR;
    if( msg_put_msgbody(fp, body, TRUE) != OK )
	return severe_error=ERROR;

    msgs_netmail++;
    
    return OK;
}



/*
 * Read and process FTN packets
 */
int unpack(FILE *pkt_file, Packet *pkt)
{
    Message msg;			/* Message header */
    Textlist tl;			/* Textlist for message body */
    MsgBody body;			/* Message body of FTN message */
    int type;
    
    /*
     * Initialize
     */
    tl_init(&tl);
    msg_body_init(&body);


    /*
     * Read packet
     */
    type = pkt_get_int16(pkt_file);
    if(type == ERROR)
    {
	if(feof(pkt_file))
	{
	    log("WARNING: premature EOF reading input packet");
	    return OK;
	}
	
	log("ERROR: reading input packet");
	return ERROR;
    }

    while(type == MSG_TYPE)
    {
	/*
	 * Read message header
	 */
	msg.node_from = pkt->from;
	msg.node_to   = pkt->to;
	if(pkt_get_msg_hdr(pkt_file, &msg) == ERROR)
	{
	    log("ERROR: reading input packet");
	    return ERROR;
	}
	
	/*
	 * Read message body
	 */
	type = pkt_get_body(pkt_file, &tl);
	if(type == ERROR)
	{
	    if(feof(pkt_file))
	    {
		log("WARNING: premature EOF reading input packet");
	    }
	    else
	    {
		log("ERROR: reading input packet");
		return ERROR;
	    }
	}
	msgs_in++;
	msg_count++;
	
	/*
	 * Parse message body
	 */
	if( msg_body_parse(&tl, &body) == -2 )
	    log("ERROR: parsing message body");
	/* Retrieve address information from kludges for NetMail */
	if(body.area == NULL)
	{
	    /* Retrieve complete address from kludges */
	    kludge_pt_intl(&body, &msg, TRUE);
	    msg.node_orig = msg.node_from;

	    debug(5, "NetMail: %s -> %s",
		  node_to_asc(&msg.node_from, TRUE),
		  node_to_asc(&msg.node_to  , TRUE) );
	    if(do_netmail(pkt, &msg, &body) == ERROR)
		return ERROR;
	}
	else 
	{
	    /* Retrieve address information from * Origin line */
	    if(msg_parse_origin(body.origin, &msg.node_orig) == ERROR)
		/* No * Origin line address, use header */
		msg.node_orig = msg.node_from;

	    debug(5, "EchoMail: %s -> %s",
		  node_to_asc(&msg.node_from, TRUE),
		  node_to_asc(&msg.node_to  , TRUE) );
	    if(do_echomail(pkt, &msg, &body) == ERROR)
		return ERROR;
	}

	/*
	 * Exit if signal received
	 */
	if(signal_exit)
	{
	    outpkt_close();
	    msg_count = 0;
	    return severe_error=ERROR;
	}
	
	/*
	 * Check for number of messages exceeding maxmsg
	 */
	if(maxmsg  &&  msg_count >= maxmsg)
	{
	    if(x_flag)
		must_exit = TRUE;
	    else
		outpkt_close();
	    msg_count = 0;
	}

    } /**while(type == MSG_TYPE)**/

    return OK;
}



/*
 * Rename .pkt -> .bad
 */
int rename_bad(char *name)
{
    char bad[MAXPATH];
    int len;
    
    strncpy0(bad, name, sizeof(bad));
    len = strlen(bad) - 4;
    if(len < 0)
	len = 0;
    strcpy(bad + len, ".bad");
    
    log("ERROR: bad packet renamed %s", bad);
    if(rename(name, bad) == ERROR)
    {
	log("$ERROR: can't rename %s -> %s", name, bad);
	return ERROR;
    }
    
    return OK;
}



/*
 * Unpack one packet file
 */
int unpack_file(char *pkt_name)
{
    Packet pkt;
    FILE *pkt_file;
    TIMEINFO ti;
    long pkt_size;
    
    /* Update time info for old messages */
    GetTimeInfo(&ti);
    now_sec = ti.time;
    max_sec = 24L * 3600L * max_history;
    exp_sec = now_sec - max_sec;
    if(exp_sec < 0)
	exp_sec = 0;
    debug(4, "now=%ld max=%ld, old < %ld", now_sec, max_sec, exp_sec);
    
    /* Open packet and read header */
    pkt_file = fopen(pkt_name, R_MODE);
    if(!pkt_file) {
	log("$ERROR: can't open packet %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }
    if(pkt_get_hdr(pkt_file, &pkt) == ERROR)
    {
	log("ERROR: reading header from %s", pkt_name);
	fclose(pkt_file);
	rename_bad(pkt_name);
	return OK;
    }
    
    /* Unpack it */
    pkt_size = check_size(pkt_name);
    log("packet %s (%ldb) from %s for %s", pkt_name, pkt_size,
	node_to_asc(&pkt.from, TRUE), node_to_asc(&pkt.to, TRUE) );
    pkts_in++;
    pkts_bytes += pkt_size;
    
    if(unpack(pkt_file, &pkt) == ERROR) 
    {
	log("ERROR: processing %s", pkt_name);
	fclose(pkt_file);
	rename_bad(pkt_name);
	return severe_error;
    }
    
    fclose(pkt_file);

    if (unlink(pkt_name)) {
	log("$ERROR: can't unlink %s", pkt_name);
	rename_bad(pkt_name);
	return ERROR;
    }

    return OK;
}



/*
 * Function called on SIGINT
 */
void prog_signal(int signum)
{
    char *name = "";

    signal_exit = TRUE;
    
    switch(signum)
    {
    case SIGHUP:
	name = " by SIGHUP";  break;
    case SIGINT:
	name = " by SIGINT";  break;
    case SIGQUIT:
	name = " by SIGQUIT"; break;
    default:
	name = "";            break;
    }

    log("KILLED%s: exit forced", name);
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] [packet ...]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] [packet ...]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -g --grade G                 processing grade\n\
         -I --in-dir name             set input packet directory\n\
         -O --out-dir name            set output packet directory\n\
         -l --lock-file               create lock file while processing\n\
         -t --insecure                insecure tossing (no AREAS.BBS check)\n\
         -n --toss-all                toss all EchoMail messages\n\
         -p --passthru                make all areas passthru\n\
         -r --routing-file NAME       read routing file\n\
         -s --strip-attribute         strip crash, hold message attribute\n\
         -m --maxmsg N                close output after N msgs\n\
         -x --maxmsg-exit             close output and exit after -m msgs\n\
         -M --maxopen N               set max # of open packet files\n\
         -b --areas-bbs NAME          use alternate AREAS.BBS\n\
\n\
	 -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config name             read config file (\"\" = none)\n\
	 -L --lib-dir name            set lib directory\n\
	 -S --spool-dir name          set spool directory\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");
    
    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c, ret;
    char *p;
    char *I_flag=NULL, *O_flag=NULL, *r_flag=NULL, *M_flag=NULL;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    char *pkt_name;
    char *areas_bbs = NULL;
    time_t toss_start, toss_delta;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "grade",        1, 0, 'g'},	/* grade */
	{ "in-dir",       1, 0, 'I'},	/* Set inbound packets directory */
	{ "lock-file",    0, 0, 'l'},	/* Create lock file while processing */
	{ "out-dir",      1, 0, 'O'},	/* Set packet directory */
	{ "insecure",     0, 0, 't'},	/* Insecure */
	{ "toss-all",     0, 0, 'n'},	/* Toss all EchoMail */
	{ "routing-file", 1, 0, 'r'},	/* Set routing file */
	{ "strip-attribute",0,0,'s'},	/* Strip attribute */
	{ "maxmsg",       1, 0, 'm'},	/* Close after N messages */
	{ "maxmsg-exit",  0, 0, 'x'},	/* Exit after maxmsg messages */
	{ "maxopen",      1, 0, 'M'},	/* Set max # open packet files */
        { "areas-bbs",	  1, 0, 'b'},
        { "passthru",	  0, 0, 'p'},

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    /* Start time */
    toss_start = time(NULL);

    /* Log name */
    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();

    /* Parse options */
    while ((c = getopt_long(argc, argv, "g:O:I:ltnr:sm:xM:b:pvhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftntoss options *****/
	case 'g':
	    g_flag = *optarg;
	    break;
	case 'I':
	    I_flag = optarg;
	    break;
        case 'l':
	    l_flag = TRUE;
            break;
	case 'O':
	    O_flag = optarg;
	    break;
	case 't':
	    t_flag = TRUE;
	    break;
	case 'n':
	    n_flag = TRUE;
	    break;
	case 'r':
	    r_flag = optarg;
	    break;
	case 's':
	    s_flag = TRUE;
	    break;
	case 'm':
	    maxmsg = atoi(optarg);
	    break;
	case 'x':
	    x_flag = TRUE;
	    break;
	case 'M':
	    M_flag = optarg;
	    break;
	case 'b':
	    areas_bbs = optarg;
	    break;
	case 'p':
	    p_flag = TRUE;
	    break;
	    
	/***** Common options *****/
	case 'v':
	    verbose++;
	    break;
	case 'h':
	    usage();
	    exit(0);
	    break;
	case 'c':
	    c_flag = optarg;
	    break;
	case 'S':
	    S_flag = optarg;
	    break;
	case 'L':
	    L_flag = optarg;
	    break;
	case 'a':
	    a_flag = optarg;
	    break;
	case 'u':
	    u_flag = optarg;
	    break;
	default:
	    short_usage();
	    exit(EX_USAGE);
	    break;
	}

    /*
     * Read config file
     */
    if(L_flag)				/* Must set libdir beforehand */
	cf_set_libdir(L_flag);
    cf_read_config_file(c_flag ? c_flag : CONFIG);

    /*
     * Process config options
     */
    if(L_flag)
	cf_set_libdir(L_flag);
    if(S_flag)
	cf_set_spooldir(S_flag);
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);

    cf_debug();
    
    /*
     * Process optional config statements
     */
    if(cf_get_string("KillEmpty", TRUE) || cf_get_string("KillBlank", TRUE))
    {
	debug(8, "config: killempty");
	kill_empty = TRUE;
    }
    if(cf_get_string("KillUnknown", TRUE))
    {
	debug(8, "config: killunknown");
	kill_unknown = TRUE;
    }
    if(cf_get_string("KillRouted", TRUE))
    {
	debug(8, "config: killrouted");
	kill_routed = TRUE;
    }
    if(cf_get_string("KillInsecure", TRUE))
    {
	debug(8, "config: killinsecure");
	kill_insecure = TRUE;
    }
    if(cf_get_string("KillCircular", TRUE))
    {
	debug(8, "config: killcircular");
	kill_circular = TRUE;
    }
    if(cf_get_string("LogNetMail", TRUE) || cf_get_string("Track", TRUE))
    {
	debug(8, "config: lognetmail");
	log_netmail = TRUE;
    }
    if(cf_get_string("CheckPath", TRUE))
    {
	debug(8, "config: checkpath");
	check_path = TRUE;
    }
    if(cf_get_string("DupeCheck", TRUE))
    {
	debug(8, "config: DupeCheck");
	dupe_check = TRUE;
    }
    if(cf_get_string("KillNoMSGID", TRUE))
    {
	debug(8, "config: KillNoMSGID");
	kill_nomsgid = TRUE;
    }
    if(cf_get_string("KillDupe", TRUE) || cf_get_string("KillDupes", TRUE))
    {
	debug(8, "config: KillDupe");
	kill_dupe = TRUE;
    }
    if(!maxmsg && (p = cf_get_string("MaxMsg", TRUE)))
    {
	debug(8, "config: MaxMsg %s", p);
	maxmsg = atoi(p);
    }
    if(!M_flag && (p = cf_get_string("MaxOpenFiles", TRUE)))
    {
	debug(8, "config: MaxOpenFiles %s", p);
	M_flag = p;
    }
    if(M_flag)
	outpkt_set_maxopen(atoi(M_flag));
    if(cf_get_string("KillOld", TRUE))
    {
	debug(8, "config: KillOld");
	kill_old = TRUE;
    }
    if( (p = cf_get_string("MaxHistory", TRUE)) )
    {
	max_history = atof(p);
	if(max_history < 0)
	    max_history = 0;
	debug(8, "config: MaxHistory %lg", max_history);
    }
    if( cf_get_string("EchoMail4D", TRUE) )
    {
	debug(8, "config: EchoMail4D");
	echomail4d = TRUE;
    }
    if( cf_get_string("NoEmptyPath", TRUE) )
    {
	debug(8, "config: NoEmptyPath");
	no_empty_path = TRUE;
    }

    zonegate_init();
    addtoseenby_init();

    /*
     * Process local options
     */
    if(I_flag)
	str_expand_name(in_dir, sizeof(in_dir), I_flag);
    else 
	BUF_COPY3(in_dir, cf_spooldir(), "/", TOSS_IN);
    if(O_flag)
    {
	pkt_outdir(O_flag, NULL);
	pkt_baddir(O_flag, NULL);
    }
    else
    {
	pkt_outdir(cf_spooldir(), TOSS_TMP);
	pkt_baddir(cf_spooldir(), TOSS_BAD);
    }
    
    /*
     * Get name of areas.bbs file from config file
     */
    if(!areas_bbs)
	if( (areas_bbs = cf_get_string("AreasBBS", TRUE)) )
	{
	    debug(8, "config: areasbbs %s", areas_bbs);
	}
    if(!areas_bbs)
    {
	fprintf(stderr, "%s: no areas.bbs specified\n", PROGRAM);
	exit(EX_USAGE);
    }
    
    routing_init(r_flag ? r_flag : ROUTING);
    areasbbs_init(areas_bbs);
    passwd_init();

    
    /* Install signal/exit handlers */
    signal(SIGHUP,  prog_signal);
    signal(SIGINT,  prog_signal);
    signal(SIGQUIT, prog_signal);

    
    ret = EXIT_OK;
    
    if(optind >= argc)
    {
	/* process packet files in directory */
	dir_sortmode(DIR_SORTMTIME);
	if(dir_open(in_dir, "*.pkt", TRUE) == ERROR)
	{
	    log("$ERROR: can't open directory %s", in_dir);
	    exit(EX_OSERR);
	}
    
	/* Lock file */
	if(l_flag)
	    if(lock_program(PROGRAM, FALSE) == ERROR)
		/* Already busy */
		exit(EXIT_BUSY);

	/* Open history */
	if(dupe_check)
	{
	    if(lock_program(LOCK_HISTORY, FALSE) == ERROR)
	    {
		/* Already busy, exit */
		if(l_flag)
		    unlock_program(PROGRAM);
		exit(EXIT_BUSY);
	    }
	    hi_init();
	}
	
	for(pkt_name=dir_get(TRUE); pkt_name; pkt_name=dir_get(FALSE))
	{
	    if(unpack_file(pkt_name) == ERROR)
	    {
		ret = EXIT_ERROR;
		break;
	    }
	    if(must_exit)
	    {
		ret = EXIT_CONTINUE;
		break;
	    }
	}

	/* Close history */
	if(dupe_check)
	{
	    unlock_program(LOCK_HISTORY);
	    hi_close();
	}
	
	dir_close();

	/* Lock file */
	if(l_flag)
	    unlock_program(PROGRAM);
    }
    else
    {
	/* Lock file */
	if(l_flag)
	    if(lock_program(PROGRAM, FALSE) == ERROR)
		/* Already busy */
		exit(EXIT_BUSY);
	
	/* Open history */
	if(dupe_check)
	{
	    if(lock_program(LOCK_HISTORY, FALSE) == ERROR)
	    {
		/* Already busy, exit */
		if(l_flag)
		    unlock_program(PROGRAM);
		exit(EXIT_BUSY);
	    }
	    hi_init();
	}
	
	/* Process packet files on command line */
	for(; optind<argc; optind++)
	{
	    if(unpack_file(argv[optind]) == ERROR)
	    {
		ret = EXIT_ERROR;
		break;
	    }
	    if(must_exit)
	    {
		ret = EXIT_CONTINUE;
		break;
	    }
	}
	
	/* Close history */
	if(dupe_check)
	{
	    unlock_program(LOCK_HISTORY);
	    hi_close();
	}
	
	/* Lock file */
	if(l_flag)
	    unlock_program(PROGRAM);
    }
    
    outpkt_close();

    /* Stop time */
    toss_delta = time(NULL) - toss_start;
    if(toss_delta <= 0)
	toss_delta = 1;
    
    if(pkts_in)
	log("pkts processed: %ld, %ld Kbyte in %ld s, %.2lf Kbyte/s",
	    pkts_in, pkts_bytes/1024, toss_delta,
	    (double)pkts_bytes/1024./toss_delta                      );
    
    if(msgs_in)
    {
	log("msgs processed: %ld in, %ld out (%ld mail, %ld echo)",
	    msgs_in, msgs_netmail+msgs_echomail, msgs_netmail, msgs_echomail);
	log("msgs processed: %ld in %ld s, %.2lf msgs/s",
	    msgs_in, toss_delta, (double)msgs_in/toss_delta);
    }
    
    if(msgs_unknown || msgs_routed || msgs_insecure || msgs_empty)
	log("msgs killed:    %ld empty, %ld unknown, %ld routed, %ld insecure",
	    msgs_empty, msgs_unknown, msgs_routed, msgs_insecure             );
    if(dupe_check && msgs_dupe)
	log("msgs killed:    %ld dupe", msgs_dupe);
    if(check_path && msgs_path)
	log("msgs killed:    %ld circular path", msgs_path);
    
    exit(ret);

    /**NOT REACHED**/
    return 1;
}
