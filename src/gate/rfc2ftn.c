/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX <-> FIDO
 *
 * $Id: rfc2ftn.c,v 4.35 1998/01/18 17:49:12 mj Exp $
 *
 * Read mail or news from standard input and convert it to a FIDO packet.
 *
 *****************************************************************************
 * Copyright (C) 1990-1998
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

#include <pwd.h>




#define PROGRAM 	"rfc2ftn"
#define VERSION 	"$Revision: 4.35 $"
#define CONFIG		DEFAULT_CONFIG_GATE



/*
 * MIME (RFC 1521) header info
 */
typedef struct st_mimeinfo
{
    char *version;		/* MIME-Version */
    char *type;			/* Content-Type */
    char *encoding;		/* Content-Transfer-Encoding */
}
MIMEInfo;



/*
 * Prototypes
 */
void	addr_set_mausdomain	(char *);
void	addr_set_mausgate	(char *);
char   *get_name_from_body	(void);
MIMEInfo *get_mime		(void);
void	sendback		(const char *, ...);
void	rfcaddr_init		(RFCAddr *);
RFCAddr rfc_sender		(void);
int	rfc_is_local		(void);
int	rfc_is_domain		(void);
int	rfc_parse		(RFCAddr *, char *, Node *, int);
int	rfc_isfido		(void);
void	cvt_user_name		(char *);
char   *receiver		(char *, Node *);
char   *mail_receiver		(RFCAddr *, Node *);
time_t	mail_date		(void);
int	snd_mail		(RFCAddr, long);
int	snd_message		(Message *, Area *, RFCAddr, RFCAddr, char *,
				 long, char *, int, MIMEInfo *);
int	print_tear_line		(FILE *);
int	print_origin		(FILE *, char *);
int	print_local_msgid	(FILE *);
int	print_via		(FILE *);
int	sendmail_t		(long);
void	short_usage		(void);
void	usage			(void);



static char *o_flag = NULL;		/* -o --out-packet-file		    */
static char *w_flag = NULL;		/* -w --write-outbound 		    */
static int   W_flag = FALSE;		/* -W --write-crash    		    */
static int   i_flag = FALSE;		/* -i --ignore-hosts   		    */

static int default_rfc_level = 0;	/* Default ^ARFC level for areas    */

static int no_from_line	= FALSE;	/* config.gate: NoFromLine          */
static int no_fsc_0035 = FALSE;		/* config.gate: NoFSC0035           */
static int no_fsc_0047 = FALSE;		/* config.gate: NoFSC0047           */
static int echomail4d = FALSE;		/* config.gate: EchoMail4d          */ 
static int x_flags_policy = 0;		/* config.gate: XFlagsPolicy        */
static int dont_use_reply_to = FALSE;	/* config.gate: DontUseReplyTo      */
static int replyaddr_ifmail_tx = FALSE;	/* config.gate: ReplyAddrIfmailTX   */


/*
 * Use Organization header for * Origin line
 */
static int use_organization_for_origin = FALSE;
static char *organization = NULL;



/* Private mail (default) */
int private = TRUE;

/* News-article */
int newsmode = FALSE;



/*
 * Global Textlist to save message body
 */
Textlist body = { NULL, NULL };



/*
 * MAUS address stuff
 */
static char *maus_domain = NULL;
static Node  maus_gate   = { -1, -1, -1, -1, "" };

/*
 * Set
 */
void addr_set_mausdomain(char *s)
{
    maus_domain = s;
}

void addr_set_mausgate(char *s)
{
    Node node;
    
    if(asc_to_node(s, &node, FALSE) == ERROR)
	log("illegal MAUSGate node address %s", s);
    else
	maus_gate = node;
}



/*
 * Get name of recipient from quote lines inserted by news readers
 */
char *get_name_from_body(void)
{
    static char buf[MAXINETADDR];
    char *p, *line1;
    int found = FALSE;
    int i;
    
    /* First line of message body */
    if(body.first == NULL)
	return NULL;
    line1 = body.first->line;
    if(line1 == NULL)
	return NULL;

    debug(9, "body 1st line: %s", line1);
    
    /*
     * nn-style quote:
     *   user@do.main (User Name) writes:
     *   User Name <user@do.main> writes:
     *   user@do.main writes:
     * or "wrote".
     */
    if(wildmatch(line1, "[a-z0-9]*@* (*) writes:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" writes:\n") ] = 0;
	found = TRUE;
    }
    if(wildmatch(line1, "[a-z0-9\"]* <*@*> writes:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" writes:\n") ] = 0;
	found = TRUE;
    }
    if(wildmatch(line1, "[a-z0-9]*@* writes:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" writes:\n") ] = 0;
	found = TRUE;
    }

    if(wildmatch(line1, "[a-z0-9]*@* (*) wrote:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" wrote:\n") ] = 0;
	found = TRUE;
    }
    if(wildmatch(line1, "[a-z0-9\"]* <*@*> wrote:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" wrote:\n") ] = 0;
	found = TRUE;
    }
    if(wildmatch(line1, "[a-z0-9]*@* wrote:\n", TRUE))
    {
	BUF_COPY(buf, line1);
	buf[ strlen(buf) - strlen(" wrote:\n") ] = 0;
	found = TRUE;
    }

    /*
     * In article <id@do.main> user@do.main writes:
     * In article <id@do.main>, user@do.main writes:
     */
    if(wildmatch(line1, "In article <*@*> * writes:\n", TRUE) ||
       wildmatch(line1, "In article <*@*>, * writes:\n", TRUE)  )
    {
	p = line1;
	while(*p && *p!='>') p++;
	if(*p == '>') p++;
	if(*p == ',') p++;
	if(*p == ' ') p++;
	BUF_COPY(buf, p);
	i = strlen(buf) - strlen(" writes:\n");
	if(i >= 0)
	    buf[i] = 0;
	found = TRUE;
    }

    if(found)
    {
	debug(9, "body name    : %s", buf);
	return buf;
    }
    
    return NULL;
}

    

/*
 * Return MIME header
 */
MIMEInfo *get_mime(void)
{
    static MIMEInfo mime;
    
    mime.version  = header_getcomplete("MIME-Version");
    mime.type     = header_getcomplete("Content-Type");
    mime.encoding = header_getcomplete("Content-Transfer-Encoding");

    return &mime;
}



/*
 * In case of error print message (mail returned by MTA)
 */
void sendback(const char *fmt, ...)
{
    va_list args;
    
    va_start(args, fmt);

    fprintf(stderr, "Internet -> FIDO gateway / FIDOGATE %s @ %s\n",
	    version_global(), cf_fqdn()                              );
    fprintf(stderr, "   ----- ERROR -----\n");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");

    va_end(args);
}



/*
 * Initialize RFCAddr
 */
void rfcaddr_init(RFCAddr *rfc)
{
    rfc->user[0] = 0;
    rfc->addr[0] = 0;
    rfc->real[0] = 0;
    rfc->flags   = 0;
}



/*
 * Return message sender as RFCAddr struct
 */
RFCAddr rfc_sender(void)
{
    RFCAddr rfc, rfc1;
    char *from, *reply_to, *p;
    struct passwd *pwd;

    from     = header_getcomplete("From");
    reply_to = header_getcomplete("Reply-To");

    rfcaddr_init(&rfc);
    rfcaddr_init(&rfc1);
    
    /*
     * Use From or Reply-To header
     */
    if(from || reply_to)
    {
	if(from)
	{
	    debug(5, "RFC From:     %s", from);
	    rfc = rfcaddr_from_rfc(from);
	}
	if(reply_to)
	{
	    debug(5, "RFC Reply-To: %s", reply_to);
	    rfc1 = rfcaddr_from_rfc(reply_to);
	    /* No From, use Reply-To */
	    if(!from)
		rfc = rfc1;
	    else if(!dont_use_reply_to)
	    {
		/* If Reply-To contains only an address which is the same as
		 * the one in From, don't replace From RFCAddr */
		if( ! ( rfc1.real[0]==0               &&
			 !stricmp(rfc.user, rfc1.user) &&
			 !stricmp(rfc.addr, rfc1.addr)    ) )
		    rfc = rfc1;
	    }
	}
    }
    /*
     * Use user id and passwd entry
     */
    else if((pwd = getpwuid(getuid())))
    {
	BUF_COPY(rfc.real, pwd->pw_gecos);
	if( (p = strchr(rfc.real, ',')) )
	    /* Kill stuff after ',' */
	    *p = 0;
	if(!rfc.real[0])
	    /* Empty, use user name */
	    BUF_COPY(rfc. real, pwd->pw_name);
	BUF_COPY(rfc.user, pwd->pw_name);
	BUF_COPY(rfc.addr, cf_fqdn());
    }
    /*
     * No sender ?!?
     */
    else 
    {
	BUF_COPY(rfc.user, "nobody");
	BUF_COPY(rfc.real, "Unknown User");
	BUF_COPY(rfc.addr, cf_fqdn());
    }

    debug(5, "RFC Sender:   %s", rfcaddr_to_asc(&rfc, TRUE));
    return rfc;
}



/*
 * Check for local RFC address, i.e. "user@HOSTNAME.DOMAIN (Full Name)"
 * or "user (Full Name)"
 */
int rfc_is_local(void)
{
    return addr_is_local( header_getcomplete("From") );
}



/*
 * Check for From address in local domain, i.e. "user@*DOMAIN (Full Name)"
 * or "user (Full Name)"
 */
int rfc_is_domain(void)
{
    return addr_is_domain( header_getcomplete("From") );
}



/*
 * Parse RFCAddr as FTN address, return name and node
 */
static int rfc_isfido_flag = FALSE;

int rfc_parse(RFCAddr *rfc, char *name, Node *node, int gw)
{
    char *p;
    int len, ret=OK;
    Node nn;
    Node *n;
    Host *h;
    
    rfc_isfido_flag = FALSE;

    debug(3, "    Name:     %s", rfc->user);
    debug(3, "    Address:  %s", rfc->addr);

    /*
     * Remove quotes "..." and copy to name[] arg
     */
    if(name)
    {
	if(rfc->real[0])
	    p = rfc->real;
	else
	    p = rfc->user;
	if(*p == '\"')			/* " makes C-mode happy */
	{
	    p++;
	    len = strlen(p);
	    if(p[len-1] == '\"')	/* " makes C-mode happy */
	    p[len-1] = 0;
	}
	mime_deheader(name, MSG_MAXNAME, p, 0);
    }

    /*
     * Special handling for addresses `*.maus.de'. These adresses are
     * converted to the form suitable for the FIDO<->MAUS gateway.
     */
    if(maus_domain)
    {
	int i, dlen, diff;

	len  = strlen(rfc->addr);
	dlen = strlen(maus_domain);
	diff = len - dlen;
	if(len > dlen                          &&
	   !strcmp(rfc->addr+diff, maus_domain)   )  /* Got it! */
	{
	    debug(3, "    is MAUS:  %s", rfc->addr);

	    if(name)
	    {
		str_append(name, MSG_MAXNAME, "_%_");
		for(i=strlen(name), p=rfc->addr;
		    i<MSG_MAXNAME-1 && *p && *p!='.';
		    i++, p++                          )
		    name[i] = toupper(*p);
		name[i] = 0;
		debug(3, "    cvt to:   %s", name);
	    }
	    if(node)
		*node = maus_gate;

	    rfc_isfido_flag = TRUE;
	    return OK;
	}
    }

    if(!node)
	return OK;
    
    n = inet_to_ftn(rfc->addr);
    if(!n)
    {
	/* Try as Z:N/F.P */
	if(asc_to_node(rfc->addr, &nn, FALSE) == OK)
	    n = &nn;
    }
    
    if(n)
    {
	*node = *n;
	rfc_isfido_flag = TRUE;
	ret = OK;
	debug(3, "    FTN node: %s", node_to_asc(n, TRUE));

	/*
	 * Look up in HOSTS
	 */
	if( (h = hosts_lookup(n, NULL)) )
	{
	    if( (h->flags & HOST_DOWN) )
	    {
		if(!rfc_is_domain())
		{
		    /* Node is down, bounce mail */
		    sprintf(address_error,
			    "FTN address %s: currently down, unreachable",
			    node_to_asc(n, TRUE));
		    ret = ERROR;
		}
	    }
	}
	/*
	 * Bounce mail to nodes not registered in HOSTS
	 */
	else if(addr_is_restricted() && !i_flag)
	{
	    sprintf(address_error,
		    "FTN address %s: not registered for this domain",
		    node_to_asc(n, TRUE));
	    ret = ERROR;
	}

	/*
	 * Check for supported zones (zone statement in CONFIG)
	 */
	if(!cf_zones_check(n->zone))
	{
	    sprintf(address_error,
		    "FTN address %s: zone %d not supported",
		    node_to_asc(n, TRUE), n->zone);
	    ret = ERROR;
	}
	
    }
    else if(gw && cf_gateway().zone)
    {
	/*
	 * If Gateway is set in config file, insert address of
	 * FIDO<->Internet gateway for non-FIDO addresses
	 */
	*node = cf_gateway();
	if(name)
	    strcpy(name, "UUCP");
	
	ret = OK;
    }
    else
	ret = ERROR;

    return ret;
}


int rfc_isfido(void)
{
    return rfc_isfido_flag;
}



/*
 * cvt_user_name() --- Convert RFC user name to FTN name:
 *                       - capitalization
 *                       - '_' -> SPACE
 *                       - '.' -> SPACE
 */
void cvt_user_name(char *s)
{
    int c, convert_flag, us_flag;
    
    /*
     * All '_' characters are replaced by space and all words
     * capitalized.  If no '_' chars are found, '.' are converted to
     * spaces (User.Name@p.f.n.z.fidonet.org addressing style).
     */
    convert_flag = isupper(*s) ? -1 : 1;
    us_flag      = strchr(s, '_') || strchr(s, ' ') || strchr(s, '@');
    
    for(; *s; s++) {
	c = *s;
	switch(c) {
	case '_':
	case '.':
	    if( c=='_' || (!us_flag && c=='.') )
		c = ' ';
	    *s = c;
	    if(!convert_flag)
		convert_flag = 1;
	    break;
	case '%':
	    if(convert_flag != -1)
		convert_flag = 2;
	    /**Fall thru**/
	default:
	    if(convert_flag > 0) {
		*s = islower(c) ? toupper(c) : c;
		if(convert_flag == 1)
		    convert_flag = 0;
	    }
	    else
		*s = c;
	    break;
	}
    }
}



/*
 * receiver() --- Check for aliases and beautify name
 */
char *receiver(char *to, Node *node)
{
    static char name[MSG_MAXNAME];
    Alias *alias;
    
    /*
     * Check for name alias
     */
    debug(5, "Name for alias checking: %s", to);

    if((alias = alias_lookup(node, to, NULL)))
    {
	debug(5, "Alias found: %s %s %s", alias->username,
	      node_to_asc(&alias->node, TRUE), alias->fullname);
	strcpy(name, alias->fullname);

	/*
	 * Store address from ALIASES into node, this will reroute
	 * the message to the point specified in ALIASES, if the message
	 * addressed to node without point address.
	 */
	*node = alias->node;
	
	return name;
    }
    
    /*
     * Alias not found. Return the the original receiver processed by
     * convert_user_name().
     */
    BUF_COPY(name, to);
    cvt_user_name(name);

    /**FIXME: implement a generic alias with pattern matching**/
    /*
     * Convert "postmaster" to "sysop"
     */
    if(!stricmp(name, "postmaster"))
	BUF_COPY(name, "Sysop");
    
    debug(5, "No alias found: return %s", name);

    return name;
}



/*
 * Return from field for FIDO message.
 * Alias checking is done by function receiver().
 */
char *mail_receiver(RFCAddr *rfc, Node *node)
{
    char *to;
    char name[MSG_MAXNAME];
    RFCAddr h;
    
    if(rfc->user[0]) {
	/*
	 * Address is argument
	 */
	if(rfc_parse(rfc, name, node, TRUE) == ERROR) {
	    log("BOUNCE: <%s>, %s", rfcaddr_to_asc(rfc, TRUE),
		(*address_error ? address_error : "unknown")  );
	    return NULL;
	}
    }
    else {
	/*
	 * News/EchoMail: address is echo feed
	 */
	*node = cf_n_uplink();
	BUF_COPY(name, "All");
    
	/*
	 * User-defined header line X-Comment-To for gateway software
	 * (can be patched into news reader)
	 */
	if( (to = header_get("X-Comment-To")) )
	{
	    h = rfcaddr_from_rfc(to);
	    rfc_parse(&h, name, NULL, FALSE);
	}
	else if( (to = get_name_from_body()) )
	{
	    h = rfcaddr_from_rfc(to);
	    rfc_parse(&h, name, NULL, FALSE);
	}
    }

    return receiver(name, node);
}



/*
 * Get date field for FIDO message. Look for `Date:' header or use
 * current time.
 */
time_t mail_date(void)
{
    time_t timevar = -1;
    char *header_date;

    if((header_date = header_get("Date"))) {
	/* try to extract date and other information from it */
	debug(5, "RFC Date: %s", header_date);
	timevar = parsedate(header_date, NULL);
	if(timevar == -1)
	    debug(5, "          can't parse this date string");
    }

    return timevar;
}



/*
 * Mail sender name and node
 */
char *mail_sender(RFCAddr *rfc, Node *node)
{
    static char name[MSG_MAXNAME];
    Node n;
    int ret;

    *name = 0;
    *node = cf_n_addr();
    ret = rfc_parse(rfc, name, &n, FALSE);
    
#ifdef PASSTHRU_NETMAIL
    /*
     * If the from address is an FTN address, convert and pass it via
     * parameter node. This may cause problems when operating different
     * FTNs.
     */
    if(ret==OK && rfc_isfido())
	*node = n;
#endif /**PASSTHRU_NETMAIL**/

    /*
     * If no real name, apply name conversion
     */
    if(!rfc->real[0])
	cvt_user_name(name);
    
    return name;
}



/*
 * Process mail/news message
 */
int snd_mail(RFCAddr rfc_to, long size)
{
    char groups[BUFSIZ];
    Node node_from, node_to;
    RFCAddr rfc_from;
    char *p;
    char subj[MSG_MAXSUBJ];
    int status, fido;
    Message msg;
    char *flags = NULL;
    MIMEInfo *mime;
    int from_is_local = FALSE;
    long limitsize;


    node_clear(&node_from);
    node_clear(&node_to);
    
    if(rfc_to.user[0])
	debug(3, "RFC To:       %s", rfcaddr_to_asc(&rfc_to, TRUE));

    /*
     * From RFCAddr
     */
    rfcaddr_init(&rfc_from);
    rfc_from = rfc_sender();
    
    /*
     * To name/node
     */
    p = mail_receiver(&rfc_to, &node_to);
    if(!p) {
	if(*address_error)
	    sendback("Address %s:\n  %s",
		     rfcaddr_to_asc(&rfc_to, TRUE), address_error);
	else
	    sendback("Address %s:\n  address/host is unknown",
		     rfcaddr_to_asc(&rfc_to, TRUE)               );
	return(EX_NOHOST);
    }
    BUF_COPY(msg.name_to, p);
    fido = rfc_isfido();

    cf_set_zone(node_to.zone);

    /*
     * From name/node
     */
    p = mail_sender(&rfc_from, &node_from);
    BUF_COPY(msg.name_from, p);
	
    /*
     * Subject
     */
    if( (p = header_get("Subject")) )
    {
	mime_deheader(subj, MSG_MAXSUBJ, p, 0);
    }
    else
	BUF_COPY(subj, "(no subject)");

    /*
     * Date
     */
    msg.date = mail_date();
    msg.cost = 0;
    msg.attr = 0;

    /*
     * MIME header
     */
    mime = get_mime();

    /*
     * X-Flags
     */
    flags = header_get("X-Flags");
    if(flags)
	str_lower(flags);
    
    if(private)
    {
	/* Check message size limit */
	limitsize = areas_get_limitmsgsize();
	if(limitsize>0 && size>limitsize)
	{
	    /* Too large, don't gate it */
	    log("message too big (%ldb, limit %ldb) for mail %s -> %s",
		size, limitsize, rfcaddr_to_asc(&rfc_from, TRUE),
		rfcaddr_to_asc(&rfc_to, TRUE)                            );
	    sendback("Address %s:\n  message too big (%ldb, limit %ldb)",
		     rfcaddr_to_asc(&rfc_to, TRUE), size, limitsize      );
	    return(EX_UNAVAILABLE);
	}
	
	msg.attr |= MSG_PRIVATE;

	from_is_local = rfc_is_local();

	if(x_flags_policy > 0) 
	{
	    char *hf = header_getcomplete("From");
	    char *hr = header_getcomplete("Reply-To");
		
	    if(x_flags_policy == 1) 
	    {
		/* Allow only local users to use the X-Flags header */
		if(from_is_local && header_hops() <= 1)
		    debug(5, "true local address - o.k.");
		else
		{
		    if(flags)
			log("NON-LOCAL From: %s, Reply-To: %s, X-Flags: %s",
			    hf ? hf : "<>", hr ? hr : "<>", flags           );
		    flags = p = NULL;
		}
	    }
	    /* Let's at least log what's going on ... */
	    if(flags)
		log("X-Flags: %s, From: %s", flags, hf ? hf : "<>");
	    
	    p = flags;
	    if(p)
	    {
		while(*p)
		    switch(*p++)
		    {
		    case 'c':
			msg.attr |= MSG_CRASH;
			break;
		    case 'p':
			msg.attr |= MSG_PRIVATE;
			break;
		    case 'h':
			msg.attr |= MSG_HOLD;
			break;
		    case 'f':
			msg.attr |= MSG_FILE;
			break;
		    case 'r':
			msg.attr |= MSG_RRREQ;
			break;
		    }
	    }	
	}
	else
	{
	    char *hf = header_getcomplete("From");

	    /* Log what's going on ... */
	    if(flags)
		log("FORBIDDEN X-Flags: %s, From: %s", flags, hf ? hf : "<>");
	    flags = NULL;
	}
	
	
	/*
	 * Return-Receipt-To -> RRREQ flag
	 */
	if( (p = header_get("Return-Receipt-To")) )
	   msg.attr |= MSG_RRREQ;
    }
    
    if(newsmode) {
	Area *pa;
	int xpost_flag;

#ifdef NO_CONTROL
	/*
	 * Check for news control message
	 */
	if((p = header_get("Control")))
	{
	    debug(3, "Skipping Control: %s", p);
	    return EX_OK;
	}
#endif

	/*
	 * News message: get newsgroups and convert to FIDO areas
	 */
	p = header_get("Newsgroups");
	if(!p) {
	    sendback("No Newsgroups header in news message");
	    return(EX_DATAERR);
	}
	BUF_COPY(groups, p);
	debug(3, "RFC Newsgroups: %s", groups);

	xpost_flag = strchr(groups, ',') != NULL;
	
	for(p=strtok(groups, ","); p; p=strtok(NULL, ","))
	{
	    debug(5, "Look up newsgroup %s", p);
	    pa = areas_lookup(NULL, p);
	    if(!pa)
		debug(5, "No FTN area");
	    else
	    {
		debug(5, "Found: %s %s Z%d", pa->area, pa->group, pa->zone);

		if( xpost_flag && (pa->flags & AREA_NOXPOST) )
		{
		    debug(5, "No Xpostings allowed - skipped");
		    continue;
		}
		if( xpost_flag && (pa->flags & AREA_LOCALXPOST) )
		{
		    if(from_is_local)
		    {
			debug(5, "Local Xposting - o.k.");
		    }
		    else
		    {
			debug(5, "No non-local Xpostings allowed - skipped");
			continue;
		    }
		}

		/* Check message size limit */
		limitsize = pa->limitsize;
		if(limitsize>0 && size>limitsize)
		{
		    /* Too large, don't gate it */
		    log("message too big (%ldb, limit %ldb) for area %s",
			size, limitsize, pa->area                        );
		    continue;
		}
		
		/* Set address or zone aka for this area */
		if(pa->addr.zone != -1)
		    cf_set_curr(&pa->addr);
		else
		    cf_set_zone(pa->zone);

		msg.area      = pa->area;
		msg.node_from = cf_n_addr();
		msg.node_to   = cf_n_uplink();
		status = snd_message(&msg, pa, rfc_from, rfc_to,
				     subj, size, flags, fido, mime);
		if(status)
		    return status;
	    }
	}
    }
    else {
	/*
	 * NetMail message
	 */
	log("MAIL: %s -> %s",
	    rfcaddr_to_asc(&rfc_from, TRUE), rfcaddr_to_asc(&rfc_to, TRUE));
	msg.area      = NULL;
	msg.node_from = node_from;
	msg.node_to   = node_to;
	return snd_message(&msg, NULL, rfc_from, rfc_to,
			   subj, size, flags, fido, mime);
    }
    
    return EX_OK;
}


int snd_message(Message *msg, Area *parea,
		RFCAddr rfc_from, RFCAddr rfc_to, char *subj,
		long size, char *flags, int fido, MIMEInfo *mime)
    /*
     * msg   	 FTN nessage structure
     * parea 	 area/newsgroup description structure
     * rfc_from  Internet sender
     * rfc_to    Internet recipient
     * subj  	 Internet Subject line
     * size      Message size
     * flags 	 X-Flags header
     * fido  	 TRUE: recipient is FTN address
     * mime  	 MIME stuff
     */
{
    static int last_zone = -1;		/* Zone address of last packet */
    static FILE *sf;			/* Packet file */
    char *header;
    int part = 1, line = 1, split;
    long maxsize;
    long lsize;
    Textline *p;
    char *id;
    int flag, add_empty;
    time_t time_split = -1;
    long seq          = 0;
    int mime_qp = 0;			/* quoted-printable flag */
    int rfc_level = default_rfc_level;
    int x_flags_n=FALSE, x_flags_m=FALSE, x_flags_f=FALSE;

    /*
     * X-Flags settings
     */
    x_flags_f = flags && strchr(flags, 'f');
    x_flags_m = flags && strchr(flags, 'm');
    x_flags_n = flags && strchr(flags, 'n');

    /*
     * ^ARFC level
     */
    if(parea && parea->rfc_lvl!=-1)
	rfc_level = parea->rfc_lvl;
    
    /*
     * MIME info
     */
    debug(6, "RFC MIME-Version:              %s",
	  mime->version  ? mime->version  : "-NONE-");
    debug(6, "RFC Content-Type:              %s",
	  mime->type     ? mime->type     : "-NONE-");
    debug(6, "RFC Content-Transfer-Encoding: %s",
	  mime->encoding ? mime->encoding : "-NONE-");

    if(mime->encoding && !stricmp(mime->encoding, "QUOTED-PRINTABLE"))
    {
	if( (header = header_get("Content-Transfer-Encoding")) )
	    str_copy(header, strlen(header)+1, "8BIT");
	mime_qp = MIME_QP;
    }
    
    /*
     * Open output packet
     */
    if(!o_flag && cf_zone()!=last_zone)
	pkt_close();
    if(!pkt_isopen())
    {
	int   crash = msg->attr & MSG_CRASH;
	char *p;

	if(w_flag || (W_flag && crash))
	{
	    char *flav  = crash ? "Crash"      : w_flag;
	    Node  nn    = crash ? msg->node_to : cf_n_uplink();

	    /* Crash mail for a point via its boss */
	    nn.point = 0;
	    if( (sf = pkt_open(o_flag, &nn, flav, TRUE)) == NULL )
		return EX_CANTCREAT;

	    /* File attach message, subject is file name */
	    if(msg->attr & MSG_FILE)
	    {
		/* Attach */
		if(bink_attach(&nn, 0, subj, flav, FALSE) != OK)
		    return EX_CANTCREAT;
		/* New subject is base name of file attach */
		if((p=strrchr(subj, '/')) || (p=strrchr(subj, '\\')))
		    subj = p + 1;
	    }
	}
	else
	{
	    if( (sf = pkt_open(o_flag, NULL, NULL, FALSE)) == NULL )
		return EX_CANTCREAT;
	}
    }
    
    last_zone = cf_zone();

    
    /*
     * Compute number of split messages if any
     */
    maxsize = parea ? parea->maxsize : areas_get_maxmsgsize();
 
    if(maxsize > 0)
    {
	split = 1;
	lsize = 0;
	p     = body.first;
	while(p)
	{
	    /* Decode all MIME-style quoted printables */
	    mime_dequote(buffer, sizeof(buffer), p->line, mime_qp);
	    lsize += strlen(buffer) + 1 /* additional CR */;
	    if(lsize > maxsize) {
		split++;
		lsize = 0;
	    }
	    p = p->next;
	}

	if(split == 1)
	    split = 0;
	
	if(split)
	    debug(5, "Must split message: size=%ld max=%ld parts=%d",
		  size, maxsize, split);
    }
    else
	split = 0;
    

    /*
     * Set pointer to first line in message body
     */
    p = body.first;
    
 again:

    /* Subject with split part indication */
    if(split && part>1)
    {
	sprintf(msg->subject, "%02d: ", part);
	BUF_APPEND(msg->subject, subj);
    }
    else
	BUF_COPY(msg->subject, subj);

    /* Header */
    pkt_put_msg_hdr(sf, msg, TRUE);


    /***** ^A kludges *******************************************************/

    /* Add kludges for MSGID / REPLY and ORIGID / ORIGREF */
    if(!x_flags_m)				/* ! X-Flags: m */
    {
	if((header = header_getcomplete("Message-ID")))
	{
	    if((id = msgid_rfc_to_fido(&flag, header, part, split, msg->area)))
	    {
		fprintf(sf, "\001MSGID: %s\r\n", id);
#ifdef MSGID_ORIGID
		if(flag && (id = msgid_rfc_to_origid(header, part, split)))
		    fprintf(sf, "\001ORIGID: %s\r\n", id);
#endif
	    }
	}	
	else
	    print_local_msgid(sf);
	
	if((header = header_getcomplete("References")) ||
	   (header = header_getcomplete("In-Reply-To")))
	{
	    if((id = msgid_rfc_to_fido(&flag, header, 0, 0, msg->area)))
	    {
		fprintf(sf, "\001REPLY: %s\r\n", id);
#ifdef MSGID_ORIGID
		if(flag && (id = msgid_rfc_to_origid(header, 0, 0)))
		    fprintf(sf, "\001ORIGREF: %s\r\n", id);
#endif
	    }
	}
    }
    else
	print_local_msgid(sf);

    if(!no_fsc_0035)
	if(!x_flags_n)
	{
	    /* Generate FSC-0035 ^AREPLYADDR, ^AREPLYTO */
	    if(replyaddr_ifmail_tx)
		fprintf(sf, "\001REPLYADDR <%s>\r\n",
			rfcaddr_to_asc(&rfc_from, FALSE));
	    else
		fprintf(sf, "\001REPLYADDR %s\r\n",
			rfcaddr_to_asc(&rfc_from, TRUE));
	    fprintf(sf, "\001REPLYTO %s %s\r\n",
		    node_to_asc(cf_addr(), FALSE),
		    msg->name_from);
	}

    if(x_flags_f)
    {
	/*
	 * Generate ^AFLAGS KFS
	 * Indicates that FrontDoor deletes the file-attach,
	 * after it has been sent. 
	 */
	fprintf(sf, "\001FLAGS KFS\r\n");
    }

    /*
     * Add ^ACHRS kludge indicating that this messages uses the
     * ISO-8859-1 charset. This may be not the case, but most Usenet
     * messages with umlauts use this character set and FIDOGATE
     * doesn't support MIME completely yet.  (^ACHRS is documented in
     * FSC-0054)
     */
    fprintf(sf, "\001CHRS: LATIN-1 2\r\n");

    if(!x_flags_n)
    {
	/* Add ^ARFC header lines */
	fprintf(sf, "\001RFC: %d 0\r\n", rfc_level);
	header_ca_rfc(sf, rfc_level);
	/* Add ^AGATEWAY header */
	if( (header = header_getcomplete("X-Gateway")) )
	    fprintf(sf, "\001GATEWAY: RFC1036/822 %s [FIDOGATE %s], %s\r\n",
		    cf_fqdn(), version_global(), header                     );
	else	
	    fprintf(sf, "\001GATEWAY: RFC1036/822 %s [FIDOGATE %s]\r\n",
		    cf_fqdn(), version_global()                         );
    }
    
    add_empty = FALSE;

    
    /***** Text header*******************************************************/

    /*
     * If Gateway is set in config file, add To line for addressing
     * FIDO<->Internet gateway
     */
    if(cf_gateway().zone && rfc_to.user[0] && !fido)
    {
	fprintf(sf, "To: %s\r\n", rfcaddr_to_asc(&rfc_to, TRUE));
	add_empty = TRUE;
    }

    if(!x_flags_n)
    {
	if(!no_from_line)
	{
	    /* From, Reply-To */
	    if( (header = header_getcomplete("From")) )
		fprintf(sf, "From: %s\r\n", header);
	    if( (header = header_getcomplete("Reply-To")) )
		fprintf(sf, "Reply-To: %s\r\n", header);

	    /* Sender, To, Cc (only for mail) */
	    if(private)
	    {
		if( (header = header_getcomplete("Sender")) )
		    fprintf(sf, "Sender: %s\r\n", header);
		else if( (header = header_getcomplete("Resent-From")) )
		    fprintf(sf, "Sender: %s\r\n", header);

		/* If Sender/Resent-From is present also include To, Cc */
		if(header)
		{
		    if( (header = header_getcomplete("To")) )
			fprintf(sf, "Header-To: %s\r\n", header);
		    if( (header = header_getcomplete("Cc")) )
			fprintf(sf, "Header-Cc: %s\r\n", header);
		}
	    }

	    add_empty = TRUE;
	}
    }
    if(add_empty)
	fprintf(sf, "\r\n");

    if(!no_fsc_0047)
    {
	/*
	 * Add ^ASPLIT kludge according to FSC-0047 for multi-part messages.
	 * Format:
	 *     ^ASPLIT: date      time     @net/node    nnnnn pp/xx +++++++++++
	 * e.g.
	 *     ^ASPLIT: 30 Mar 90 11:12:34 @494/4       123   02/03 +++++++++++
	 */
	if(split)
	{
	    char buf[20];
	    
	    if(part == 1)
	    {
		time_split = time(NULL);
		seq        = sequencer(SEQ_SPLIT) % 100000;/* Max. 5 digits */
	    }
	    
	    sprintf(buf, "%d/%d", cf_addr()->net, cf_addr()->node);
	    
	    fprintf(sf,
		"\001SPLIT: %-18s @%-11.11s %-5ld %02d/%02d +++++++++++\r\n",
		date("%d %b %y %H:%M:%S", &time_split), buf, seq, part, split);
	}
    }
    else
    {
	/*
	 * Add line indicating split message
	 */
	if(split)
	    fprintf(sf,
		" * Large message split by FIDOGATE: part %02d/%02d\r\n\r\n",
		    part, split						    );
    }


    /***** Message body *****************************************************/

    lsize = 0;
    while(p)
    {
	/* Decode all MIME-style quoted printables */
	mime_dequote(buffer, sizeof(buffer), p->line, mime_qp);
	pkt_put_line(sf, buffer);
	lsize += strlen(buffer) + 1 /* additional CR */;
	if(split && lsize > maxsize) {
	    print_tear_line(sf);
	    if(newsmode)
	    {
		char *origin;

		if(parea && parea->origin)
		    origin = parea->origin;
		else if(use_organization_for_origin && organization)
		    origin = organization;
		else
		    origin = cf_p_origin();
		print_origin(sf, origin);
	    }
	    /* End of message */
	    putc(0, sf);
	    part++;
	    p = p->next;
	    goto again;
	}
	p = p->next;
	line++;
    }

    /*
     * If message is for echo mail (-n flag) then add
     * tear, origin, seen-by and path line.
     */
    print_tear_line(sf);
    if(newsmode)
    {
	char *origin;
	
	if(parea && parea->origin)
	    origin = parea->origin;
	else if(use_organization_for_origin && organization)
	    origin = organization;
	else
	    origin = cf_p_origin();
	print_origin(sf, origin);
    }
    else
	print_via(sf);

    /* End of message */
    putc(0, sf);
    return EX_OK;
}



/*
 * Output tear line
 */
int print_tear_line(FILE *fp)
{
#ifdef PASSTHRU_ECHOMAIL
    char *p;

    if( (p = header_get("X-FTN-Tearline")) )
	fprintf(fp, "\r\n--- %s\r\n", p);
    else
#endif
	fprintf(fp, "\r\n--- FIDOGATE %s\r\n", version_global());

    return ferror(fp);
}



/*
 * Generate origin, seen-by and path line
 */
int print_origin(FILE *fp, char *origin)
{
    char buf[80];
    char bufa[30];
    int len;
#ifdef PASSTHRU_ECHOMAIL
    char *p;
#endif

    /*
     * Origin line
     */
    BUF_COPY(buf , " * Origin: ");
    BUF_COPY(bufa, node_to_asc(cf_addr(), TRUE));

#ifdef PASSTHRU_ECHOMAIL
    if( (p = header_get("X-FTN-Origin")) )
	BUF_APPEND(buf, p);
    else
#endif
    {
	/* Max. allowed length of origin line is 79 (80 - 1) chars, 3
	 * are used by " ()".  */
	len = 80 - strlen(bufa) - 3;
	
	/* Add origin text */
	str_append(buf, len, origin);
	/* Add address */
	BUF_APPEND(buf, " (");
	BUF_APPEND(buf, bufa);
	BUF_APPEND(buf, ")" );
    }
    fprintf(fp, "%s\r\n", buf);

    /*
     * SEEN-BY
     */
#ifdef PASSTHRU_ECHOMAIL
    /* Additional SEEN-BYs from X-FTN-Seen-By headers, ftntoss must be
       run afterwards to sort / compact SEEN-BY */
    for( p = header_geth("X-FTN-Seen-By", TRUE);
	 p;
	 p = header_geth("X-FTN-Seen-By", FALSE) )
	fprintf(fp, "SEEN-BY: %s\r\n", p);
#endif
    if(cf_addr()->point)		/* Generate 4D addresses */
    {
	fprintf(fp, "SEEN-BY: %d/%d",
		cf_addr()->net, cf_addr()->node);
	if(echomail4d)
	    fprintf(fp, ".%d", cf_addr()->point);
	fprintf(fp,"\r\n");
    }
    else				/* Generate 3D addresses */
    {
	fprintf(fp, "SEEN-BY: %d/%d",
		cf_addr()->net, cf_addr()->node );
	if(cf_uplink()->zone && cf_uplink()->net)
	{
	    if(cf_uplink()->net != cf_addr()->net)
		fprintf(fp, " %d/%d", cf_uplink()->net, cf_uplink()->node);
	    else if(cf_uplink()->node != cf_addr()->node)
		fprintf(fp," %d", cf_uplink()->node);
	}
	fprintf(fp,"\r\n");
    }

    /*
     * ^APATH
     */
#ifdef PASSTHRU_ECHOMAIL
    /* Additional ^APATHs from X-FTN-Path headers, ftntoss must be
       run afterwards to compact ^APATH */
    for( p = header_geth("X-FTN-Path", TRUE);
	 p;
	 p = header_geth("X-FTN-Path", FALSE) )
	fprintf(fp, "\001PATH: %s\r\n", p);
#endif
    if(cf_addr()->point)		/* Generate 4D addresses */
    {
	fprintf(fp, "\001PATH: %d/%d",
		cf_addr()->net, cf_addr()->node);
	if(echomail4d)
	    fprintf(fp, ".%d", cf_addr()->point);
	fputs("\r\n", fp);
    }
    else				/* Generate 3D addresses */
    {
	fprintf(fp, "\001PATH: %d/%d\r\n",
		cf_addr()->net, cf_addr()->node);
    }
    
    return ferror(fp);
}



/*
 * Generate local `^AMSGID:' if none is found in message header
 */
int print_local_msgid(FILE *fp)
{
    long msgid;

    msgid = sequencer(SEQ_MSGID);
    fprintf(fp, "\001MSGID: %s %08ld\r\n",
	    node_to_asc(cf_addr(), FALSE), msgid);

    return ferror(fp);
}



/*
 * Generate "^AVia" line for NetMail
 */
int print_via(FILE *fp)
{
    fprintf(fp, "\001Via FIDOGATE/%s %s, %s\r\n",
	    PROGRAM, node_to_asc(cf_addr(), FALSE),
	    date("%a %b %d %Y at %H:%M:%S %Z", NULL)  );

    return ferror(fp);
}



/*
 * Send mail to addresses taken from To, Cc, Bcc headers
 */
int sendmail_t(long int size)
{
    char *header, *p;
    int status=EX_OK, st;
    RFCAddr rfc_to;
    
    /*
     * To:
     */
    for(header=header_get("To"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	{
	    rfc_to = rfcaddr_from_rfc(p);
	    if( (st = snd_mail(rfc_to, size)) != EX_OK )
		status = st;
	}

    /*
     * Cc:
     */
    for(header=header_get("Cc"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	{
	    rfc_to = rfcaddr_from_rfc(p);
	    if( (st = snd_mail(rfc_to, size)) != EX_OK )
		status = st;
	}

    /*
     * Bcc:
     */
    for(header=header_get("Bcc"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	{
	    rfc_to = rfcaddr_from_rfc(p);
	    if( (st = snd_mail(rfc_to, size)) != EX_OK )
		status = st;
	}

    return status;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] [user ...]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] [user ...]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -b --news-batch              process news batch\n\
         -B --binkley NAME            set Binkley outbound directory\n\
         -i --ignore-hosts            do not bounce unknown host\n\
	 -n --news-mode               set news mode\n\
	 -o --out-packet-file  NAME   set outbound packet file name\n\
	 -O --out-packet-dir   NAME   set outbound packet directory\n\
         -t --to                      get recipient from To, Cc, Bcc\n\
         -w --write-outbound FLAV     write directly to Binkley .?UT packet\n\
         -W --write-crash             write crash directly to Binkley .CUT\n\
\n\
	 -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config name             read config file (\"\" = none)\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");
    
    exit(0);
}



/***** main ******************************************************************/
int main(int argc, char **argv)
{
    RFCAddr rfc_to;
    int i, c;
    int status=EX_OK, st;
    long size, nmsg;
    char *p;
    int b_flag=FALSE, t_flag=FALSE;
    char *B_flag=NULL;
    char *O_flag=NULL;
    char *c_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    int option_index;
    static struct option long_options[] =
    {
	{ "news-batch",   0, 0, 'b'},	/* Process news batch */
	{ "binkley",      1, 0, 'B'},	/* Binkley outbound base dir */
	{ "out-packet-file",1,0,'o'},	/* Set packet file name */
	{ "news-mode",    0, 0, 'n'},	/* Set news mode */
	{ "ignore-hosts", 0, 0, 'i'},	/* Do not bounce unknown hosts */
	{ "out-dir",      1, 0, 'O'},	/* Set packet directory */
	{ "write-outbound",1,0, 'w'},	/* Write to Binkley outbound */
	{ "write-crash",  0, 0, 'w'},	/* Write crash to Binkley outbound */
	{ "to",           0, 0, 't'},	/* Get recipient from To, Cc, Bcc */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();
    
    newsmode = FALSE;

    

    while ((c = getopt_long(argc, argv, "bB:o:niO:w:Wtvhc:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** rfc2ftn options *****/
	case 'b':
	    /* News batch flag */
	    b_flag   = TRUE;
	    newsmode = TRUE;
	    private  = FALSE;
	    break;
	case 'B':
	    B_flag = optarg;
	    break;
	case 'o':
	    /* Set packet file name */
	    o_flag = optarg;
	    break;
	case 'n':
	    /* Set news-mode */
	    newsmode = TRUE;
	    private  = FALSE;
	    break;
	case 'i':
	    /* Don't bounce unknown hosts */
	    i_flag = TRUE;
	    break;
	case 'O':
	    /* Set packet dir */
	    O_flag = optarg;
	    break;
	case 'w':
	    w_flag = optarg;
	    break;
	case 'W':
	    W_flag = TRUE;
	    break;
	case 't':
	    t_flag = TRUE;
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
    cf_read_config_file(c_flag ? c_flag : CONFIG);
    cf_check_gate();
    
    /*
     * Process config options
     */
    if(B_flag)
	cf_s_btbasedir(B_flag);
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);

    cf_debug();

    /*
     * Process optional config statements
     */
    if(cf_get_string("NoFromLine", TRUE))
    {
	debug(8, "config: NoFromLine");
	no_from_line = TRUE;
    }
    if(cf_get_string("NoFSC0035", TRUE))
    {
	debug(8, "config: NoFSC0035");
	no_fsc_0035 = TRUE;
    }
    if(cf_get_string("NoFSC0047", TRUE))
    {
	debug(8, "config: NoFSC0047");
	no_fsc_0047 = TRUE;
    }
    if( (p = cf_get_string("MaxMsgSize", TRUE)) )
    {
	long sz;
	
	debug(8, "config: MaxMsgSize %s", p);
	sz = atol(p);
	if(sz <= 0)
	    log("WARNING: illegal MaxMsgSize value %s", p);
	else
	    areas_maxmsgsize(sz);
    }
    if( (p = cf_get_string("LimitMsgSize", TRUE)) )
    {
	long sz;
	
	debug(8, "config: LimitMsgSize %s", p);
	sz = atol(p);
	if(sz <= 0)
	    log("WARNING: illegal LimitMsgSize value %s", p);
	else
	    areas_limitmsgsize(sz);
    }
    if( (p = cf_get_string("MAUSDomain", TRUE)) )
    {
	debug(8, "config: MAUSdomain %s", p);
	addr_set_mausdomain(p);
    }
    if( (p = cf_get_string("MAUSGate", TRUE)) )
    {
	debug(8, "config: MAUSgate %s", p);
	addr_set_mausgate(p);
    }
    if( cf_get_string("EchoMail4D", TRUE) )
    {
	debug(8, "config: EchoMail4D");
	echomail4d = TRUE;
    }
    if(cf_get_string("HostsRestricted", TRUE))
    {
	debug(8, "config: HostsRestricted");
	addr_restricted(TRUE);
    }
    if( (p = cf_get_string("RFCLevel", TRUE)) )
    {
	debug(8, "config: RFCLevel %s", p);
	default_rfc_level = atoi(p);
    }
    if(cf_get_string("UseOrganizationForOrigin", TRUE))
    {
	debug(8, "config: UseOrganizationForOrigin");
	use_organization_for_origin = TRUE;
    }
    if( (p = cf_get_string("XFlagsPolicy", TRUE)) )
    {
	switch(*p) 
	{
	case 'n': case 'N': case '0':
	    x_flags_policy = 0;			/* No X-Flags */
	    break;
	case 's': case 'S': case '1':
	    x_flags_policy = 1;			/* "Secure" X-Flags (local) */
	    break;
	case 'a': case 'A': case '2':
	    x_flags_policy = 2;			/* Open X-Flags (all!!!) */
	    break;
	}
	debug(8, "config: XFlagsPolicy %d", x_flags_policy);
    }
    if(cf_get_string("DontUseReplyTo", TRUE))
    {
	debug(8, "config: DontUseReplyTo");
	dont_use_reply_to = TRUE;
    }
    if( (p = cf_get_string("RFCAddrMode", TRUE)) )
    {
	int m = 0;

	switch(*p) 
	{
	case '(': case 'p': case '0':
	    m = 0;				/* user@do.main (Real Name) */
	    break;
	case '<': case 'a': case '1':
	    m = 1;				/* Real Name <user@do.main> */
	    break;
	}
	rfcaddr_mode(m);
	debug(8, "config: RFCAddrMode %d", m);
    }
    if(cf_get_string("ReplyAddrIfmailTX", TRUE))
    {
	debug(8, "config: ReplyAddrIfmailTX");
	replyaddr_ifmail_tx = TRUE;
    }
    

    /*
     * Process local options
     */
    if(newsmode)
	pkt_outdir(OUTPKT_NEWS, NULL);
    else
	pkt_outdir(OUTPKT_MAIL, NULL);
    if(O_flag)
	pkt_outdir(O_flag, NULL);

    /*
     * Init various modules
     */
    if(newsmode)
	areas_init();
    hosts_init();
    alias_init();
    passwd_init();

    /*
     * Switch stdin to binary for reading news batches
     */
#ifdef OS2
    if(b_flag)
	_fsetmode(stdin, "b");
#endif
#ifdef MSDOS
    /* ??? */
#endif
    
    /**
     ** Main loop: read message(s) from stdin, batches if -b
     **/
    nmsg = 0;
    while(TRUE)
    {
	if(b_flag)
	{
	    size = read_rnews_size(stdin);
	    if(size == -1)
		log("ERROR: reading news batch");
	    if(size <= 0)
		break;
	    nmsg++;

	    debug(3, "Batch: message #%ld size %ld", nmsg, size);
	}
	
	/*
	 * Read message header from stdin
	 */
	header_delete();
	header_read(stdin);

	/*
	 * Get Organization header
	 */
	if(use_organization_for_origin)
	{
	    organization = header_getcomplete("Organization");
	}
	
	/*
	 * Read message body from stdin and count size
	 */
	size = 0;
	tl_clear(&body);
	while(read_line(buffer, BUFFERSIZE, stdin)) {
	    tl_append(&body, buffer);
	    size += strlen(buffer) + 1;	    /* `+1' for additional CR */
	}
	debug(7, "Message body size %ld (+CR!)", size);

	rfcaddr_init(&rfc_to);
	
	if(newsmode)
	    /*
	     * Send mail to echo feed for news messages
	     */
	    status = snd_mail(rfc_to, size);
	else
	    if(t_flag)
	    {
		/*
		 * Send mail to addresses from headers
		 */
		status = sendmail_t(size);
	    }
	    else
	    {
		/*
		 * Send mail to addresses from command line args
		 */
		for(i = optind; i < argc; i++)
		{
		    rfc_to = rfcaddr_from_rfc(argv[i]);
		    if( (st = snd_mail(rfc_to, size)) != EX_OK )
			status = st;
		}
	    }
	
	if(!b_flag)
	    break;
    }

    pkt_close();
    
    exit(status);

    /**NOT REACHED**/
    return 1;
}
