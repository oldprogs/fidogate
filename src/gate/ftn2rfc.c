/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftn2rfc.c,v 4.0 1996/04/17 18:17:41 mj Exp $
 *
 * Convert FTN mail packet to RFC messages (mail and news batches)
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
#include "getopt.h"

#include <pwd.h>
#include <fcntl.h>
#include <sys/wait.h>



#define PROGRAM 	"ftn2rfc"
#define VERSION 	"$Revision: 4.0 $"
#define CONFIG		CONFIG_GATE



/*
 * Prototypes
 */
void	check_chrs		(char *);
char   *get_from		(Textlist *, Textlist *);
char   *get_reply_to		(Textlist *);
char   *get_to			(Textlist *);
char   *get_cc			(Textlist *);
char   *get_bcc			(Textlist *);
Area   *news_msg		(char *);
int	unpack			(FILE *, Packet *);
int	rename_bad		(char *);
int	unpack_file		(char *);

void	short_usage		(void);
void	usage			(void);



/*
 * Command line options
 */
int t_flag = FALSE;

char in_dir[MAXPATH];
char news_dir[MAXPATH];
char news_name[MAXPATH];
char news_tmp[MAXPATH];
FILE *news_file;


/*
 * X-FTN flags
 *    f    X-FTN-From
 *    t    X-FTN-To
 *    T    X-FTN-Tearline
 *    O    X-FTN-Origin
 *    V    X-FTN-Via
 *    D    X-FTN-Domain
 *    S    X-FTN-Seen-By
 *    P    X-FTN-Path
 */
int x_ftn_f = FALSE;
int x_ftn_t = FALSE;
int x_ftn_T = FALSE;
int x_ftn_O = FALSE;
int x_ftn_V = FALSE;
int x_ftn_D = FALSE;
int x_ftn_S = FALSE;
int x_ftn_P = FALSE;


/*
 * TrackerMail
 */
static char *tracker_mail_to = NULL;


/*
 * MSGID handling
 */
static int no_unknown_msgid_zones = FALSE;
static int no_messages_without_msgid = FALSE;


/*
 * Use * Origin line for Organization header
 */
static int use_origin_for_organization = FALSE;


/*
 * Don't bounce message from FTN address not listed in HOSTS
 */
static int ignore_hosts = FALSE;



/*
 * check_chrs() --- Check for ^ACHARSET / ^ACHRS
 */
void check_chrs(char *buf)
{
    /*
     * Look for `^ACHARSET ...' or `^ACHRS ...'
     */
    if(!strncmp(buf+1, "CHARSET: ", 9))
	charset_set(buf + 9);
    else if(!strncmp(buf+1, "CHRS: ", 6))
	charset_set(buf + 6);
}



/*
 * Get header for
 *   - From/Reply-To/UUCPFROM
 *   - To
 *   - Cc
 *   - Bcc
 */
char *get_from(Textlist *rfc, Textlist *kludge)
{
    char *p;
    
    p = rfcheader_get(rfc, "From");
    if(!p)
	p = kludge_get(kludge, "REPLYADDR", NULL);
    if(!p)
	p = rfcheader_get(rfc, "UUCPFROM");

    return p;
}    

char *get_reply_to(Textlist *tl)
{
    return rfcheader_get(tl, "Reply-To");
}    

char *get_to(Textlist *tl)
{
    return rfcheader_get(tl, "To");
}    

char *get_cc(Textlist *tl)
{
    return rfcheader_get(tl, "Cc");
}    

char *get_bcc(Textlist *tl)
{
    return rfcheader_get(tl, "Bcc");
}    



/*
 * Test for EchoMail, return Area entry.
 */
Area *news_msg(char *line)
{
    char *p;
    Area *pa;
    static Area area;
    
    if(line)
    {
	/* Message is FIDO EchoMail */
	strip_crlf(line);
	
	for(p=line+strlen("AREA:"); *p && is_space(*p); p++);
	debug(7, "FIDO Area: %s", p);
	pa = areas_lookup(p, NULL);
	if(pa)
	{
	    debug(7, "Found: %s %s Z%d", pa->area, pa->group, pa->zone);
	    return pa;
	}

	/* Area not found */
	area.next  = NULL;
	area.area  = p;
	area.group = NULL;
	area.zone  = 0;
	return &area;
    }

    /* Message is FIDO NetMail */
    return NULL;
}



/*
 * Read and convert FTN mail packet
 */
int unpack(FILE *pkt_file, Packet *pkt)
{
    Message msg;			/* Message header */
    RFCAddr addr_from, addr_to;
    Textlist tl;			/* Textlist for message body */
    MsgBody body;			/* Message body of FTN message */
    int lines;				/* Lines in message body */
    Area *area;				/* Area entry for EchoMail */
    int type;
    Textline *pl;
    char *p, *s;
    char *msgbody_rfc_from;		/* RFC header From */
    char *msgbody_rfc_reply_to;		/* RFC header Reply-To */
    char *msgbody_rfc_to;		/* RFC header To */
    char *msgbody_rfc_cc;		/* RFC header Cc */
    char *msgbody_rfc_bcc;		/* RFC header Bcc */
    char mail_to[MAXINETADDR];		/* Addressee of mail */
    char x_orig_to[MAXINETADDR];
    char *from_line, *to_line;		/* From:, To: */
    char *reply_to_line;		/* Reply-To: */
    char *id_line, *ref_line;		/* Message-ID:, References: */
    char *cc_line, *bcc_line;		/* Cc:, Bcc: */
    char *thisdomain, *uplinkdomain;	/* FQDN addr this node, uplink,  */
    char *origindomain;			/*           node in Origin line */
    Textlist theader;			/* RFC headers */
    Textlist tbody;    			/* RFC message body */
    int uucp_flag;			/* To == UUCP or GATEWAY */
    int ret;
    int rfc_lvl, rfc_lines;
    char *split_line;
    

    /*
     * Initialize
     */
    tl_init(&tl);
    tl_init(&theader);
    tl_init(&tbody);
    msg_body_init(&body);
    news_file = NULL;
    ret = OK;
    
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
	x_orig_to[0] = 0;
	tl_clear(&theader);
	tl_clear(&tbody);
	charset_reset();			/* Reset ^ACHRS char set */

	/*
	 * Read message header
	 */
	msg.node_from = pkt->from;
	msg.node_to   = pkt->to;
	if( pkt_get_msg_hdr(pkt_file, &msg) == ERROR )
	{
	    log("ERROR: reading input packet");
	    ret = ERROR;
	    break;
	}

	/* Strip spaces at end of line */
	strip_space(msg.name_from);
	strip_space(msg.name_to);
	strip_space(msg.subject);

	/* Replace empty subject */
	if (!*msg.subject)
	    strcpy(msg.subject, "(no subject)");
	
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
		ret = ERROR;
		break;
	    }
	}

	/*
	 * Parse message body
	 */
	if( msg_body_parse(&tl, &body) == -2 )
	    log("ERROR: parsing message body");
	/* Retrieve address information from kludges for NetMail */
	if(body.area == NULL)
	{
	    /* Retrieve complete address from kludges */
	    kludge_pt_intl(&body, &msg);
	    msg.node_orig = msg.node_from;
	}
	else 
	{
	    /* Retrieve address information from * Origin line */
	    if(msg_parse_origin(body.origin, &msg.node_orig) == ERROR)
		/* No * Origin line address, use header */
		node_invalid(&msg.node_orig);
	}
	debug(7, "FIDO sender (from/origin): %s",
	      node_to_asc(&msg.node_orig, TRUE));

	/*
	 * strip_crlf() all kludge and RFC lines
	 */
	for(pl=body.kludge.first; pl; pl=pl->next)
	    strip_crlf(pl->line);
	for(pl=body.rfc.first; pl; pl=pl->next)
	    strip_crlf(pl->line);
	
	/*
	 * X-Split header line
	 */
	split_line = NULL;
	if( (p = kludge_get(&body.kludge, "SPLIT", NULL)) )
	{
	    split_line = p;
	}
	else if( (p = kludge_get(&body.body, "SPLIT", NULL)) )
	{
	    strip_crlf(p);
	    split_line = p;
	}

	/*
	 * Remove empty first line after RFC headers and empty last line
	 */
	if( body.rfc.first && (pl = body.body.first) )
	{
	    if(pl->line[0] == '\r')
		tl_delete(&body.body, pl);
	}
	if( (pl = body.body.last) )
	{
	    if(pl->line[0] == '\r')
		tl_delete(&body.body, pl);
	}
	
	/*
	 * Convert message body
	 */
	lines = 0;

	if( (p = kludge_get(&body.kludge, "CHRS", NULL)) )
	    charset_set(p);
	else if( (p = kludge_get(&body.kludge, "CHARSET", NULL)) )
	    charset_set(p);

	rfc_lvl   = 0;
	rfc_lines = FALSE;
	if( (p = kludge_get(&body.kludge, "RFC", NULL)) )
	{
	    s = strtok(p, " \t");
	    if(s)
		rfc_lvl = atoi(s);
	    s = strtok(NULL, " \t");
	    if(s && !stricmp(s, "lines"))
		rfc_lines = TRUE;
	}
	    
	for(pl=body.body.first; pl; pl=pl->next)
	{
	    p = pl->line;
	    if(*p == '\001')			/* Kludge in message body */
		check_chrs(p);			/* ^ACHRS / ^ACHARSET */
	    else				/* Normal line */
	    {
		msg_xlate_line(buffer, sizeof(buffer), p);
		if(rfc_lines)
		{
		    tl_append(&tbody, buffer);
		    lines++;
		}
		else
		    lines += msg_format_buffer(buffer, &tbody);
	    }
	}

	/*
	 * Check for mail or news.
	 *
	 * area == NULL  -  NetMail
	 * area != NULL  -  EchoMail
	 */
	if( (area = news_msg(body.area)) )
	{
	    /*
	     * Set AKA according to area's zone
	     */
	    cf_set_zone(area->zone);
	    
	    if(!news_file)
	    {
		long n = sequencer(SEQ_NEWS);

		sprintf(news_tmp,  "%s/%08ld.tmp", news_dir, n);
		sprintf(news_name, "%s/%08ld.msg", news_dir, n);
		news_file = fopen(news_tmp, W_MODE);
		if(!news_file)
		{
		    log("$ERROR: can't create output file %s", news_tmp);
		    ret = ERROR;
		    break;
		}
	    }
	}
	else
	{
	    /*
	     * Set AKA according to sender's zone
	     */
	    cf_set_zone(msg.node_orig.zone!=-1 
			? msg.node_orig.zone
			: msg.node_from.zone  );
	}
	

	/*
	 * Convert FTN from/to addresses to RFCAddr struct
	 */
	addr_from = rfcaddr_from_ftn(msg.name_from, &msg.node_orig);
	addr_to   = rfcaddr_from_ftn(msg.name_to,   &msg.node_to  );

	uucp_flag = FALSE;
	if(!stricmp(addr_to.real, "UUCP")    ||
	   !stricmp(addr_to.real, "GATEWAY")   )
	{
	    /* Don't output UUCP or GATEWAY as real name */
	    uucp_flag = TRUE;
	}

	/*
	 * RFC address headers from text body
	 */
	msgbody_rfc_from     = get_from    (&body.rfc, &body.kludge);
	msgbody_rfc_to       = get_to      (&body.rfc);
	msgbody_rfc_reply_to = get_reply_to(&body.rfc);
	msgbody_rfc_cc       = get_cc      (&body.rfc);
	msgbody_rfc_bcc      = get_bcc     (&body.rfc);

	/*
	 * If -g flag is set for area and message seems to comme from
	 * another gateway, skip it.
	 */
	if(area && (area->flags & AREA_NOGATE))
	{
	    if(msgbody_rfc_from)
	    {
		log("skipping message from gateway, origin=%s",
		    node_to_asc(&msg.node_orig, TRUE));
		continue;
	    }
	
	    /* GIGO */
	    if( (p = kludge_get(&body.kludge, "PID", NULL))  &&
	       !strnicmp(p, "GIGO", 4)                         )
	    {
		log("skipping message from gateway (GIGO), origin=%s",
		    node_to_asc(&msg.node_orig, TRUE));
		continue;
	    }
	}

	/*
	 * Do alias checking on both from and to names
	 */
	if(!msgbody_rfc_from)
	{
	    Alias *a;

	    debug(7, "Checking for alias: %s",
		  rfcaddr_to_asc(&addr_from, TRUE));
	    a = alias_lookup(&msg.node_orig, NULL, addr_from.real);
	    if(a)
	    {
		debug(7, "Alias found: %s %s %s", a->username,
		      node_to_asc(&a->node, FALSE), a->fullname);
		strncpy0(addr_from.user, a->username, sizeof(addr_from.user));
#ifdef ALIASES_ARE_LOCAL
		strncpy0(addr_from.addr, cf_fqdn(), sizeof(addr_from.user));
#endif
	    }
	}
	if(!msgbody_rfc_to)
	{
	    Alias *a;
	    
	    debug(7, "Checking for alias: %s",
		  rfcaddr_to_asc(&addr_to, TRUE));
	    a = alias_lookup(cf_addr(), NULL, addr_to.real);
	    if(a)
	    {
		debug(7, "Alias found: %s %s %s", a->username,
		      node_to_asc(&a->node, FALSE), a->fullname);
		BUF_COPY(mail_to, a->username);
	    }
	    else
		BUF_COPY(mail_to, addr_to.user);
	}

	/*
	 * Special handling for -t flag (insecure):
	 *   Messages with To line will be bounced
	 */
	if(area==NULL && t_flag && msgbody_rfc_to)
	{
	    debug(1, "Insecure message with To line");
	    log("BOUNCE: insecure mail from %s",
		rfcaddr_to_asc(&addr_from, TRUE));
	    bounce_mail("insecure", &addr_from, &msg, msgbody_rfc_to, &tbody);
	    continue;
	}
	    
	/*
	 * There are message trackers out there in FIDONET. Obviously
	 * they can't handling addressing the gateway so we send mail
	 * from "MsgTrack..." etc. to TrackerMail.
	 */
	if(tracker_mail_to)
	    if(   !strnicmp(addr_from.user, "MsgTrack", 8)
	       || !strnicmp(addr_from.user, "Reflex_Netmail_Policeman", 24)
	       || !strnicmp(addr_from.user, "TrackM", 6)
	       || !strnicmp(addr_from.user, "ITrack", 6)
	       || !strnicmp(addr_from.user, "O/T-Track", 9)
	       /* || whatever ... */		                            )
	    {
		debug(1, "Mail from FIDO message tracker");
		BUF_COPY(x_orig_to, mail_to);
		BUF_COPY(mail_to  , tracker_mail_to);
		/* Reset uucp_flag to avoid bouncing of these messages */
		uucp_flag = FALSE;
	    }


	thisdomain   = ftn_to_inet(cf_addr(),      TRUE);
	uplinkdomain = ftn_to_inet(&msg.node_from, TRUE);
	origindomain = msg.node_orig.zone != -1
	    ? ftn_to_inet(&msg.node_orig, TRUE) : FTN_INVALID_DOMAIN;

#ifdef HOSTS_RESTRICTED
	/*
	 * Bounce mail from nodes not registered in HOSTS,
	 * but allow mail to local users.
	 */
	if(!ignore_hosts &&
	   area==NULL && msgbody_rfc_to && !addr_is_domain(msgbody_rfc_to))
	{
	    Host *h;
	    
	    /* Lookup host */
	    if( (h = hosts_lookup(&msg.node_orig, NULL)) == NULL )
	    {
		/* Not registered in HOSTS */
		debug(1, "Not a registered node: %s",
		      node_to_asc(&msg.node_orig, FALSE));
		log("BOUNCE: mail from unregistered %s",
		    rfcaddr_to_asc(&addr_from, TRUE));
		bounce_mail("restricted", &addr_from, &msg,
			    msgbody_rfc_to, &tbody);
		continue;
	    }

	    /* Bounce, if host is down */
	    if(h->flags & HOST_DOWN)
	    {
		debug(1, "Registered node is down: %s",
		      node_to_asc(&msg.node_orig, FALSE));
		log("BOUNCE: mail from down %s",
		    rfcaddr_to_asc(&addr_from, TRUE));
		bounce_mail("down", &addr_from, &msg,
			    msgbody_rfc_to, &tbody);
		continue;
	    }
	}	    
#endif /**HOSTS_RESTRICTED**/

	/*
	 * Check mail messages' user name
	 */
	if(area==NULL && !msgbody_rfc_to &&
	   !strchr(mail_to, '@') && !strchr(mail_to, '%') &&
	   !strchr(mail_to, '!')			    )
	{
	    if(uucp_flag) 
	    {
		/*
		 * Addressed to `UUCP' or `GATEWAY', but no To: line
		 */
		debug(1, "Message to `UUCP' or `GATEWAY' without To line");
		log("BOUNCE: mail from %s without To line",
		    rfcaddr_to_asc(&addr_from, TRUE));
		bounce_mail("noto", &addr_from, &msg, msgbody_rfc_to, &tbody);
		continue;
	    }
	    /*
	     * Add `@host.domain' to local address
	     */
	    str_append(mail_to, sizeof(mail_to), "@");
	    str_append(mail_to, sizeof(mail_to), cf_fqdn());
	}

	/*
	 * Construct string for From: header line
	 */
	if(msgbody_rfc_from) {
	    RFCAddr rfc;
	    
	    rfc = rfcaddr_from_rfc(msgbody_rfc_from);
	    if(!rfc.real[0])
		strcpy(rfc.real, addr_from.real);
	    
	    addr_from = rfc;
	}
	from_line = rfcaddr_to_asc(&addr_from, TRUE);

	/*
	 * Construct Reply-To line
	 */
	reply_to_line = msgbody_rfc_reply_to;
	
	/*
	 * Construct string for To:/X-Comment-To: header line
	 */
	if(msgbody_rfc_to) {
	    if(strchr(msgbody_rfc_to, '(') || strchr(msgbody_rfc_to, '<') ||
	       !*addr_to.real || uucp_flag                                   )
		to_line = buf_sprintf("%s", msgbody_rfc_to);
	    else
		to_line = buf_sprintf("%s (%s)", msgbody_rfc_to, addr_to.real);
	}
	else {
	    if(area)
	    {
		if(!strcmp(addr_to.user, "All")  ||
		   !strcmp(addr_to.user, "Alle") ||
		   !strcmp(addr_to.user, "*.*")  ||
		   !strcmp(addr_to.user, "*")      )
		    to_line = NULL;
		else
		    to_line = buf_sprintf("(%s)", addr_to.real);
	    }
	    else
		to_line = buf_sprintf("%s", mail_to);
	}

	/*
	 * Construct Cc/Bcc header lines
	 */
	cc_line  = msgbody_rfc_cc;
	bcc_line = msgbody_rfc_bcc;
	
	/*
	 * Construct Message-ID and References header lines
	 */
	id_line  = NULL;
	ref_line = NULL;
	
	if( (p = kludge_get(&body.kludge, "ORIGID", NULL)) )
	    id_line = msgid_convert_origid(p, FALSE);
	else if( (p = kludge_get(&body.kludge, "Message-Id", NULL)) )
	    id_line = msgid_convert_origid(p, FALSE);
	else if( (p = kludge_get(&body.kludge, "RFC-Message-ID", NULL)) )
	    id_line = msgid_convert_origid(p, FALSE);
	if(!id_line)
	{
	    int id_zone;
	    
	    if( (p = kludge_get(&body.kludge, "MSGID", NULL)) )
	    {
		id_line = msgid_fido_to_rfc(p, &id_zone);
		if(no_unknown_msgid_zones)
		    if(id_zone>=-1 && !cf_zones_check(id_zone))
		    {
			log("MSGID %s: unknown zone, not gated", p);
			continue;
		    }
	    }
	    else
	    {
		if(no_messages_without_msgid)
		{
		    log("MSGID: none, not gated");
		    continue;
		}
		id_line = msgid_default(&msg.node_orig, msg.name_from,
					msg.name_to,
					msg.subject, msg.date);
	    }
	}
	/* Can't happen, but who knows ... ;-) */
	if(!id_line)
	{
	    log("ERROR: id_line==NULL, strange.");
	    continue;
	}
	
	if( (p = kludge_get(&body.kludge, "ORIGREF", NULL)) )
	    ref_line = msgid_convert_origid(p, FALSE);
	if(!ref_line)
	    if( (p = kludge_get(&body.kludge, "REPLY", NULL)) )
		ref_line = msgid_fido_to_rfc(p, NULL);


	/*
	 * Output RFC mail/news header
	 */

	/* Different header for mail and news */
	if(area==NULL) {			/* Mail */
	    log("%s -> %s", from_line, to_line);
	    
	    if(mail_open() == ERROR)
	    {
		ret = ERROR;
		break;
	    }

	    tl_appendf(&theader,
			     "From %s %s\n", rfcaddr_to_asc(&addr_from, FALSE),
			     date("%a %b %d %H:%M:%S %Y", NULL) );
	    tl_appendf(&theader,
		"Received: by %s (FIDOGATE %s)\n",
		thisdomain, version_global()                            );
	    tl_appendf(&theader,
		"\tid AA%05d; %s\n",
		getpid(), date(NULL, NULL) );
	}
	else 					/* News */
	{
	    if(!strcmp(thisdomain, uplinkdomain))	/* this == uplink */
		tl_appendf(&theader,
			   "Path: %s!%s!not-for-mail\n",
			   thisdomain, origindomain );
	    else
		tl_appendf(&theader,
			   "Path: %s!%s!%s!not-for-mail\n",
			   thisdomain, uplinkdomain, origindomain );
	}

	/* Common header */
	tl_appendf(&theader, "Date: %s\n", date(NULL, &msg.date));
	tl_appendf(&theader, "From: %s\n", from_line);
	if(reply_to_line)
	    tl_appendf(&theader, "Reply-To: %s\n", reply_to_line);
	msg_xlate_line(buffer, sizeof(buffer), msg.subject);
	tl_appendf(&theader, "Subject: %s\n", buffer);
	tl_appendf(&theader, "Message-ID: %s\n", id_line);
	
	/* Different header for mail and news */
	if(area==NULL) {			/* Mail */
	    if(ref_line)
		tl_appendf(&theader, "In-Reply-To: %s\n", ref_line);
	    tl_appendf(&theader, "To: %s\n", to_line);
	    if(cc_line)
		tl_appendf(&theader, "Cc: %s\n", cc_line);
	    if(bcc_line)
		tl_appendf(&theader, "Bcc: %s\n", bcc_line);
	    if(*x_orig_to)
		tl_appendf(&theader, "X-Orig-To: %s\n", x_orig_to);
#ifdef ERRORS_TO
	    tl_appendf(&theader, "Errors-To: %s\n", ERRORS_TO);
#endif
	    /* FTN ReturnReceiptRequest -> Return-Receipt-To */
	    if(msg.attr & MSG_RRREQ)
		tl_appendf(&theader, "Return-Receipt-To: %s\n",
				 rfcaddr_to_asc(&addr_from, FALSE)   );
	}
	else 					/* News */
	{
	    if(ref_line)
		tl_appendf(&theader, "References: %s\n", ref_line);
	    tl_appendf(&theader, "Newsgroups: %s\n",
			     area->group ? area->group : FTN_JUNK);
	    if(!area->group)
		tl_appendf(&theader, "X-FTN-Area: %s\n", area->area);
	    if(area->distribution)
		tl_appendf(&theader, "Distribution: %s\n",
				 area->distribution               );
	    if(to_line)
		tl_appendf(&theader, "X-Comment-To: %s\n", to_line);
	}
	
	/* Common header */
	if(use_origin_for_organization && body.origin)
	{
	    strip_crlf(body.origin);
	    msg_xlate_line(buffer, sizeof(buffer), body.origin);
	    if((p = strrchr(buffer, '(')))
		*p = 0;
	    strip_space(buffer);
	    p = buffer + strlen(" * Origin: ");
	    while(is_blank(*p))
		p++;
	    tl_appendf(&theader, "Organization: %s\n", p);
	}
	else
	{
	    tl_appendf(&theader, "Organization: %s\n", cf_organization() );
	}
	tl_appendf(&theader, "Lines: %d\n", lines);
	/**FIXME: convert ^AGATEWAY kludges, too**/
	tl_appendf(&theader, "X-Gateway: FIDO .. %s [FIDOGATE %s]\n",
		   cf_fqdn(), version_global()                       );

	if(area==NULL)
	{
	    if(x_ftn_f)
		tl_appendf(&theader, "X-FTN-From: %s @ %s\n",
			   addr_from.real, node_to_asc(&msg.node_orig,TRUE));
	    if(x_ftn_t)
		tl_appendf(&theader, "X-FTN-To: %s @ %s\n",
			   addr_to.real, node_to_asc(&msg.node_to,TRUE));
	}

	if(x_ftn_T  &&  body.tear && !strncmp(body.tear, "--- ", 4))
	{
	    strip_crlf(body.tear);
	    msg_xlate_line(buffer, sizeof(buffer), body.tear);
	    tl_appendf(&theader, "X-FTN-Tearline: %s\n", buffer+4);
	}
	if(!use_origin_for_organization  &&  x_ftn_O  &&  body.origin)
	{
	    strip_crlf(body.origin);
	    msg_xlate_line(buffer, sizeof(buffer), body.origin);
	    p = buffer + strlen(" * Origin: ");
	    while(is_blank(*p))
		p++;
	    tl_appendf(&theader, "X-FTN-Origin: %s\n", p);
	}
	if(x_ftn_V)
	    for(pl=body.via.first; pl; pl=pl->next)
	    {
		p = pl->line;
		strip_crlf(p);
		msg_xlate_line(buffer, sizeof(buffer), p+1);
		tl_appendf(&theader, "X-FTN-Via: %s\n", buffer+4);
	    }
	if(x_ftn_D)
	    tl_appendf(&theader, "X-FTN-Domain: Z%d@%s\n",
		       cf_zone(), cf_zones_ftn_domain(cf_zone()));
	if(x_ftn_S)
	    for(pl=body.seenby.first; pl; pl=pl->next)
	    {
		p = pl->line;
		strip_crlf(p);
		tl_appendf(&theader, "X-FTN-Seen-By: %s\n", p + 9);
	    }
	if(x_ftn_P)
	    for(pl=body.path.first; pl; pl=pl->next)
	    {
		p = pl->line;
		strip_crlf(p);
		tl_appendf(&theader, "X-FTN-Path: %s\n", p + 7);
	    }

	if(split_line)
	    tl_appendf(&theader, "X-SPLIT: %s\n", split_line);

	/*
	 * Add extra headers
	 */
	if(area)
	    for(pl=area->x_hdr.first; pl; pl=pl->next)
		tl_appendf(&theader, "%s\n", pl->line);
	    
	tl_appendf(&theader, "\n");

	/*
	 * Write header and message body to output file
	 */
	if(area) {
	    /* News batch */
	    fprintf(news_file, "#! rnews %ld\n", tl_size(&theader) +
						 tl_size(&tbody)	);
	    tl_print(&theader, news_file);
	    tl_print(&tbody,   news_file);
	}
	else
	{
	    /* Mail message */
	    tl_print(&theader, mail_file());
	    tl_print(&tbody,   mail_file());
	    /* Close mail */
	    mail_close();
	}

    } /**while(type == MSG_TYPE)**/

    if(news_file) 
    {
	/* Close news file */
	fclose(news_file);
	/* Rename .tmp -> .msg */
	if(rename(news_tmp, news_name) == -1)
	    log("$ERROR: can't rename %s to %s", news_tmp, news_name);
    }
    
    return ret;
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

    debug(1, "Processing FTN packet %s", pkt_name);

    /*
     * Open packet and read header
     */
    pkt_file = fopen(pkt_name, R_MODE);
    if(!pkt_file) {
	log("$ERROR: can't open packet %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }
    if(pkt_get_hdr(pkt_file, &pkt) == ERROR)
    {
	log("ERROR: reading header from %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }
    
    /*
     * Unpack it
     */
    if(unpack(pkt_file, &pkt) == -1) 
    {
	log("ERROR: processing %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }
    
    fclose(pkt_file);
    
    if (unlink(pkt_name)) {
	log("$ERROR: can't unlink packet %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }

    return OK;
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
options: -i --ignore-hosts            do not bounce unknown host\n\
	 -t --insecure                process insecure packets\n\
         -I --in-dir name             set input packet directory\n\
         -l --lock-file               create lock file while processing\n\
         -x --exec-program name       exec program after processing\n\
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
    int c;
    char *execprog = NULL;
    int l_flag=FALSE;
    char *I_flag=NULL;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    char *pkt_name;
    char *p;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "ignore-hosts", 0, 0, 'i'},	/* Do not bounce unknown hosts */
	{ "insecure",     0, 0, 't'},	/* Toss insecure packets */
	{ "in-dir",       1, 0, 'I'},	/* Set inbound packets directory */
	{ "lock-file",    0, 0, 'l'},	/* Create lock file while processing */
	{ "exec-program", 1, 0, 'x'},	/* Exec program after processing */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "itI:lx:vhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftn2rfc options *****/
	case 'i':
	    /* Don't bounce unknown hosts */
	    ignore_hosts = TRUE;
	    break;
	case 't':
	    /* Insecure */
	    t_flag = TRUE;
	    break;
	case 'I':
	    /* Inbound packets directory */
	    I_flag = optarg;
	    break;
        case 'l':
            /* Lock file */
	    l_flag = TRUE;
            break;
        case 'x':
            /* Exec program after unpack */
            execprog = optarg;
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
     * Process local options
     */
    if(I_flag)
    {
	BUF_COPY (in_dir  , I_flag);
	BUF_COPY3(mail_dir, I_flag, "/", INDIR_MAIL);
	BUF_COPY3(news_dir, I_flag, "/", INDIR_NEWS);
    }
    else 
    {
	BUF_COPY3(in_dir  , cf_spooldir(), "/", t_flag ? INSECUREDIR : INDIR);
	BUF_COPY5(mail_dir, cf_spooldir(), "/", INDIR, "/", INDIR_MAIL);
	BUF_COPY5(news_dir, cf_spooldir(), "/", INDIR, "/", INDIR_NEWS);
    }
    
    /*
     * Process optional config statements
     */
    if(cf_get_string("DotNames", TRUE))
    {
	debug(8, "config: DotNames");
	rfcaddr_dot_names(TRUE);
    }
    if( (p = cf_get_string("X-FTN", TRUE)) )
    {
	debug(8, "config: X-FTN %s", p);
	while(*p)
	{
	    switch(*p)
	    {
	    case 'f':    x_ftn_f = TRUE;  break;
	    case 't':    x_ftn_t = TRUE;  break;
	    case 'T':    x_ftn_T = TRUE;  break;
	    case 'O':    x_ftn_O = TRUE;  break;
	    case 'V':    x_ftn_V = TRUE;  break;
	    case 'D':    x_ftn_D = TRUE;  break;
	    case 'S':    x_ftn_S = TRUE;  break;
	    case 'P':    x_ftn_P = TRUE;  break;
	    }
	    p++;
	}
    }
    if( (p = cf_get_string("BounceCCMail", TRUE)) )
    {
	debug(8, "config: BounceCCMail %s", p);
	bounce_set_cc(p);
    }
    if( (p = cf_get_string("TrackerMail", TRUE)) )
    {
	debug(8, "config: TrackerMail %s", p);
	tracker_mail_to = p;
    }
    if(cf_get_string("NoUnknownMSGIDZones", TRUE))
    {
	debug(8, "config: NoUnknownMSGIDZones");
	no_unknown_msgid_zones = TRUE;
    }
    if(cf_get_string("NoMessagesWithoutMSGID", TRUE))
    {
	debug(8, "config: NoMessagesWithoutMSGID");
	no_messages_without_msgid = TRUE;
    }
    if(cf_get_string("UseOriginForOrganization", TRUE))
    {
	debug(8, "config: UseOriginForOrganzation");
	use_origin_for_organization = TRUE;
    }
    
    /*
     * Init various modules
     */
    areas_init();
    hosts_init();
    alias_init();

    /*
     * If called with -l lock option, try to create lock FILE
     */
    if(l_flag)
	if(lock_program(PROGRAM, NOWAIT) == ERROR)
	    exit(EXIT_BUSY);

    if(optind >= argc)
    {
	/* process packet files in input directory */
	dir_sortmode(DIR_SORTMTIME);
	if(dir_open(in_dir, "*.pkt", TRUE) == ERROR)
	{
	    log("$ERROR: can't open directory %s", in_dir);
	    if(l_flag)
		unlock_program(PROGRAM);
	    exit(EX_OSERR);
	}

	for(pkt_name=dir_get(TRUE); pkt_name; pkt_name=dir_get(FALSE))
	    unpack_file(pkt_name);

	dir_close();
    }
    else
    {
	/*
	 * Process packet files on command line
	 */
	for(; optind<argc; optind++)
	    unpack_file(argv[optind]);
    }
    

    /*
     * Execute given command, if option -x set.
     */
    if(execprog)
    {
	int ret;

	BUF_COPY3(buffer, cf_libdir(), "/", execprog);
	debug(4, "Command: %s", buffer);
	ret = (system(buffer) >> 8) & 0xff;
	debug(4, "Exit code=%d", ret);
    }
    
    if(l_flag)
	unlock_program(PROGRAM);
    
    exit(EX_OK);

    /**NOT REACHED**/
    return 1;
}
