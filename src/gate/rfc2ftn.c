/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX <-> FIDO
 *
 * $Id: rfc2ftn.c,v 4.4 1996/05/03 19:18:44 mj Exp $
 *
 * Read mail or news from standard input and convert it to a FIDO packet.
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




#define PROGRAM 	"rfc2ftn"
#define VERSION 	"$Revision: 4.4 $"
#define CONFIG		CONFIG_GATE



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
char   *get_name_from_body	(void);
MIMEInfo *get_mime		(void);
void	sendback		(const char *, ...);
char   *mail_sender		(Node *);
char   *mail_address		(void);
char   *receiver		(char *, Node *);
char   *mail_receiver		(char *, Node *);
time_t	mail_date		(void);
char   *estrtok			(char *, char *);
int	snd_mail		(char *, char *, long);
int	snd_message		(Message *, Area *, char *, char *, char *,
				 long, char *, int, MIMEInfo *);
int	print_tear_line		(FILE *);
int	print_origin		(FILE *, char *);
int	print_local_msgid	(FILE *);
int	print_via		(FILE *);
int	sendmail_t		(char *, long);
void	short_usage		(void);
void	usage			(void);



static char *o_flag = NULL;		/* -o --out-packet-file option */
static char *w_flag = NULL;		/* -w --write-outbound  option */
static int   W_flag = FALSE;		/* -W --write-crash     option */

static int default_rfc_level = 0;	/* Default ^ARFC level for areas */

static int no_from_line	= FALSE;	/* config: NoFromLine */
static int no_fsc_0035 = FALSE;		/* config: NoFSC0035 */
static int no_fsc_0047 = FALSE;		/* config: NoFSC0047 */
static int echomail4d = FALSE;		/* config.gate: EchoMail4d */ 
 


/* Private mail (default) */
int private = TRUE;

/* News-article */
int newsmode = FALSE;



/*
 * Global Textlist to save message body
 */
Textlist body = { NULL, NULL };



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

    fprintf(stderr, "Internet -> FIDO gateway / FIDOGATE %s\n",
	    version_global()                                   );
    fprintf(stderr, "   ----- ERROR -----\n");
    vfprintf(stderr, fmt, args);
    fprintf(stderr, "\n");
}



/*
 * Return sender's full name. This is taken from
 *   - Reply-To header
 *   - From header
 *   - user id and password file
 *   - environment LOGNAME or USER
 */

char *mail_sender(Node *node)
{
    struct passwd *pwd;
    static char buffer[BUFSIZ];
#ifdef PASSTHRU_NETMAIL
    static char name[MSG_MAXNAME];
    Node n;
#endif /**PASSTHRU_NETMAIL**/
    char *from;
    char *p;
    
    /*
     * Default sender node is gateway address
     */
    *node = cf_n_addr();
    
    /*
     * Look up name in Reply-To or From header of message
     */
    if((from = header_getcomplete("Reply-To")) == NULL)
	from = header_getcomplete("From");

    if(from)
    {
	/*
	 * Found Reply-To or From header
	 */
	debug(5, "RFC From: %s", from);

#ifdef PASSTHRU_NETMAIL
	/*
	 * If the from address is an FTN address, convert and pass it via
	 * parameter node. This may cause problems when operating different
	 * FTNs.
	 */
	if(addr_from_rfcaddr(from, buffer))
	    if(parse_address(buffer, name, &n)!=ERROR && isfido())
	    {
		*node = n;
		debug(5, "    is FTN address %s", node_to_asc(node, TRUE));
	    }
#endif /**PASSTHRU_NETMAIL**/

	if(!name_from_rfcaddr(from, buffer))
	    username_from_rfcaddr(from, buffer);
	debug(5, "RFC Full name: %s", buffer);
	return buffer;
    }

    /*
     * Try user id and passwort file entry
     */
    if((pwd = getpwuid(getuid())))
    {
	strcpy(buffer, pwd->pw_gecos);
	if((p = strchr(buffer, ',')))
	    /*
	     * Kill stuff after ','
	     */
	    *p = 0;
	if(!*buffer)
	    /*
	     * Empty, use user name
	     */
	    strcpy(buffer, pwd->pw_name);
	debug(5, "passwd Full name: %s", buffer);
	return buffer;
    }

    /*
     * Use user name from environment as a last resort
     */
    if((p = getenv("LOGNAME")))
    {
	strcpy(buffer, p);
	debug(5, "Env LOGNAME Full name: %s", buffer);
	return buffer;
    }
    if((p = getenv("USER")))
    {
	strcpy(buffer, p);
	debug(5, "Env USER Full name: %s", buffer);
	return buffer;
    }

    debug(5, "No name for sender found");
    strcpy(buffer, "Unknown User");
    return buffer;
}



/*
 * Return sender's mail address. This is taken from
 *   - Reply-To header
 *   - From header
 *   - user id and password file
 *   - environment LOGNAME or USER
 */

char *mail_address(void)
{
    struct passwd *pwd;
    char *p;
    
    /*
     * Look up name in Reply-To or From header of message
     */
    if((p = header_getcomplete("Reply-To")) == NULL)
	p = header_getcomplete("From");

    if(p)
	return buf_copy(p);

    /*
     * Try user id and passwort file entry
     */
    if((pwd = getpwuid(getuid())))
	return buf_sprintf("%s@%s", pwd->pw_name, cf_fqdn() );

    /*
     * Use user name from environment as a last resort
     */
    if((p = getenv("LOGNAME")))
	return buf_sprintf("%s@%s", p, cf_fqdn() );
    if((p = getenv("USER")))
	return buf_sprintf("%s@%s", p, cf_fqdn() );

    return NULL;
}



/*
 * receiver() --- Check for aliases and beautify name
 */

char *receiver(char *to, Node *node)
{
    static char name[MSG_MAXNAME];
    int i, c, convert_flag;
    Alias *alias;
    int us_flag;
    
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
     * Alias not found. Return the the original receiver with all
     * '_' characters replaced by space and all words capitalized.
     * If no '_' chars are found, '.' are converted to spaces
     * (User.Name@p.f.n.z.fidonet.org addressing style).
     */
    convert_flag = isupper(*to) ? -1 : 1;
    us_flag      = strchr(to, '_') != NULL;
    
    for(i=0; *to && i<35; i++, to++) {
	c = *to;
	switch(c) {
	case '_':
	case '.':
	    if( c=='_' || (!us_flag && c=='.') )
		c = ' ';
	    name[i] = c;
	    if(!convert_flag)
		convert_flag = 1;
	    break;
	case '%':
	    if(convert_flag != -1)
		convert_flag = 2;
	    /**Fall thru**/
	default:
	    if(convert_flag > 0) {
		name[i] = islower(c) ? toupper(c) : c;
		if(convert_flag == 1)
		    convert_flag = 0;
	    }
	    else
		name[i] = c;
	    break;
	}
    }
    name[i] = 0;

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
 * Alias checking is done via receiver().
 */

char *mail_receiver(char *address, Node *node)
{
    char *to;
    char name[MSG_MAXNAME];
    char realname[128];

    realname[0] = 0;

    if(address) {
	/*
	 * Address is argument
	 */
	if(parse_address(address, name, node)) {
	    log("BOUNCE: address <%s>", address);
	    return NULL;
	}
    }
    else {
	/*
	 * News/EchoMail: address is echo feed
	 */
	*node = cf_n_uplink();
	strcpy(name, "All");
    
	/*
	 * User-defined header line X-Comment-To for gateway software
	 * (can be patched into news reader)
	 */
	if( (to = header_get("X-Comment-To")) )
	{
	    if(name_from_rfcaddr(to, realname) && *name)
		strncpy0(name, realname, sizeof(name));
	}
	else if( (to = get_name_from_body()) )
	{
	    if(name_from_rfcaddr(to, realname) && *name)
		strncpy0(name, realname, sizeof(name));
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
 * Process mail/news message
 */
int snd_mail(char *from, char *to, long int size)
    /* from --- address from */
    /* to   --- address to send to (news = NULL) */
{
    char groups[BUFSIZ];
    Node node_from, node_to;
    char *p, *subj;
    int status, fido;
    Message msg;
    char *flags = NULL;
    MIMEInfo *mime;
    
    node_from.domain[0] = 0;
    node_to  .domain[0] = 0;
    
    if(to)
	debug(3, "RFC To: %s", to);

    /*
     * To name/node
     */
    p = mail_receiver(to, &node_to);
    if(!p) {
	if(*address_error)
	    sendback("Address %s:\n  %s", to, address_error);
	else
	    sendback("Address %s:\n  address/host is unknown", to);
	return(EX_NOHOST);
    }
    BUF_COPY(msg.name_to, p);
    fido = isfido();

    cf_set_zone(node_to.zone);

    /*
     * From name/node
     */
    p = mail_sender(&node_from);
    if(!p)
	p = "Gateway";
    BUF_COPY(msg.name_from, p);

    /*
     * Subject
     */
    subj = header_get("Subject");
    if(!subj)
	subj = "(no subject)";

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

    if(private)
    {
	msg.attr |= MSG_PRIVATE;

	p = flags;
	
	/*
	 * Allow only true local users to use the X-Flags header
	 */
	if(addr_is_local(from) && header_hops() <= 1)
	    debug(5, "true local address - o.k.");
	else
	{
	    if(flags)
		log("non-local %s, X-Flags: %s", from, flags);
	    flags = p = NULL;
	}
	    
	if(p)
	{
	    while(*p)
		switch(*p++)
		{
		case 'c': case 'C':
		    msg.attr |= MSG_CRASH;
		    break;
		case 'p': case 'P':
		    msg.attr |= MSG_PRIVATE;
		    break;
		case 'h': case 'H':
		    msg.attr |= MSG_HOLD;
		    break;
		case 'f': case 'F':
		    msg.attr |= MSG_FILE;
		    break;
		case 'r': case 'R':
		    msg.attr |= MSG_RRREQ;
		    break;
		}
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
		    if(addr_is_local(from))
		    {
			debug(5, "Local Xposting - o.k.");
		    }
		    else
		    {
			debug(5, "No non-local Xpostings allowed - skipped");
			continue;
		    }
		}

		/* Set address or zone aka for this area */
		if(pa->addr.zone != -1)
		    cf_set_curr(&pa->addr);
		else
		    cf_set_zone(pa->zone);

		msg.area      = pa->area;
		msg.node_from = cf_n_addr();
		msg.node_to   = cf_n_uplink();
		status = snd_message(&msg, pa, from, NULL,
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
	log("%s -> %s", from, to);
	msg.area      = NULL;
	msg.node_from = node_from;
	msg.node_to   = node_to;
	return snd_message(&msg, NULL, from, to,
			   subj, size, flags, fido, mime);
    }
    
    return EX_OK;
}


int snd_message(Message *msg, Area *parea,
		char *from, char *to, char *subj,
		long int size, char *flags, int fido, MIMEInfo *mime)
    /* msg   --- FTN nessage structure */
    /* parea --- area/newsgroup description structure */
    /* from  --- Internet sender */
    /* to    --- Internet recipient */
    /* subj  --- Internet Subject line */
    /* flags --- X-Flags header */
    /* fido  --- TRUE: recipient is FTN address */
    /* mime  --- MIME stuff */
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
	mime_qp = MIME_QP;
    
    /*
     * Set pointer to first line in message body
     */
    p = body.first;
    
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
	split = size > maxsize ? size / maxsize + 1 : 0;
	if(split)
	    debug(5, "Must split message: size=%ld max=%ld parts=%d",
		  size, maxsize, split);
    }
    else
	split = 0;
    

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


    if(!flags || !strchr(flags, 'M'))		/* X-Flags: M */
    {
	/*
	 * Add kludges for MSGID / REPLY and ORIGID / ORIGREF
	 */
	if((header = header_get("Message-ID")))
	{
	    if((id = msgid_rfc_to_fido(&flag, header, part, split, msg->area)))
	    {
		fprintf(sf, "\001MSGID: %s\r\n", id);
#ifdef MSGID_ORIGID
		if(flag && (id = msgid_rfc_to_origid(header, part, split)))
		    fprintf(sf, "\001ORIGID: %s\r\n", id);
#endif
	    }
	    else
		print_local_msgid(sf);
	}	
	else
	    print_local_msgid(sf);
	
	if((header = header_get("References")) ||
	   (header = header_get("In-Reply-To")))
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
	if(!flags || !strchr(flags, 'N'))
	{
	    /*
	     * Generate FSC-0035 ^AREPLYADDR, ^AREPLYTO
	     */
	    if(from) 
	    {
		fprintf(sf, "\001REPLYADDR %s\r\n", from);
		fprintf(sf, "\001REPLYTO %s %s\r\n",
			node_to_asc(cf_addr(), FALSE),
			msg->name_from);
	    }
	}

    if(flags && strchr(flags, 'F'))
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

    /*
     * Add ^ARFC header lines according to FIDO-Gatebau '94 specs
     */
    fprintf(sf, "\001RFC: %d 0\r\n", rfc_level);
    header_ca_rfc(sf, rfc_level);

    add_empty = FALSE;
    
    /*
     * If Gateway is set in config file, add To line for addressing
     * FIDO<->Internet gateway
     */
    if(cf_gateway().zone && to && !fido)
    {
	fprintf(sf, "To: %s\r\n", to);
	add_empty = TRUE;
    }

    if(!flags || !strchr(flags, 'N'))
    {
	if(!no_from_line)
	{
	    /*
	     * Add From line with return address
	     */
	    if(from)
	    {
		fprintf(sf, "From: %s\r\n", from);
		add_empty = TRUE;
	    }
	}

#ifdef 0 /**Replaced with ^ARFC-Xxxx kludges**********************************/
	/*
	 * Add empty line before " * " info lines, because otherwise
	 * Eugene Crosser's ifgate gets confused and treats them as
	 * RFC continuation headers.
	 */
	if(add_empty)
	    fprintf(sf, "\r\n");
	add_empty = FALSE;

	if(!split || part==1)		/* Info lines only in 1st part */
	{
	    /*
	     * Add some header lines as info for user
	     */
	    if((header = header_get("From")))
	    {
		if(strcmp(from, header)) 
		{
		    /*
		     * From header line is not equal to reply address, output
		     * From and Reply-To header line.
		     */
		    fprintf(sf, " * From: %s\r\n", header);
		    while((header = header_getnext()))
			fprintf(sf, " *       %s\r\n", header);
		    if((header = header_get("Reply-To")))
		    {
			fprintf(sf, " * Reply-To: %s\r\n", header);
			while((header = header_getnext()))
			    fprintf(sf, " *           %s\r\n", header);
		    }
		    add_empty = TRUE;
		}		    
	    }
	    if((header = header_get("To")))
	    {
		fprintf(sf, " * To: %s\r\n", header);
		while((header = header_getnext()))
		    fprintf(sf, " *     %s\r\n", header);
		add_empty = TRUE;
	    }
	    if((header = header_get("Cc")))
	    {
		fprintf(sf, " * Cc: %s\r\n", header);
		while((header = header_getnext()))
		    fprintf(sf, " *     %s\r\n", header);
		add_empty = TRUE;
	    }
	    if((header = header_get("Newsgroups")))
		if(strchr(header, ',') || !newsmode)
		{
		    /* Posted to multiple groups or mail */
		    fprintf(sf, " * Newsgroups: %s\r\n", header);
		    add_empty = TRUE;
		}
	}
#endif /**********************************************************************/
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
	 * Add line indicating splitted message
	 */
	if(split)
	    fprintf(sf,
		" * Large message split by FIDOGATE: part %02d/%02d\r\n\r\n",
		    part, split						    );
    }

    /*
     * Copy message body
     */
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
		print_origin(sf,
			     parea && parea->origin ? parea->origin
			     : cf_origin() );
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
	print_origin(sf,
		     parea && parea->origin ? parea->origin : cf_origin() );
    else
	print_via(sf);

    /*
     * Done
     */
    /* End of message */
    putc(0, sf);
    return EX_OK;
}



/*
 * Output tear line
 */
int print_tear_line(FILE *fp)
{
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
    
    strncpy0(buf, " * Origin: ", sizeof(buf));
    strncpy0(bufa, node_to_asc(cf_addr(), TRUE), sizeof(bufa));
    
    /*
     * Max. allowed length of origin line is 79 (80 - 1) chars,
     * 3 chars for additional " ()".
     */
    len = 80 - strlen(bufa) - 3;

    /* Add origin text */
    strncat0(buf, origin, len);
    /* Add address */
    strncat0(buf, " (", sizeof(buf));
    strncat0(buf, bufa, sizeof(buf));
    strncat0(buf, ")" , sizeof(buf));
    
    /* Origin */
    fprintf(fp, "%s\r\n", buf);

    if(cf_addr()->point)		/* Generate 4D addresses */
    {
	/* SEEN-BY */
	fprintf(fp, "SEEN-BY: %d/%d",
		cf_addr()->net, cf_addr()->node);
	if(echomail4d)
	    fprintf(fp, ".%d", cf_addr()->point);
	/* PATH */
	fprintf(fp, "\r\n\001PATH: %d/%d",
		cf_addr()->net, cf_addr()->node);
	if(echomail4d)
	    fprintf(fp, ".%d", cf_addr()->point);
	fputs("\r\n", fp);
    }
    else				/* Generate 3D addresses */
    {
	/* SEEN-BY */
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
	/* PATH */
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
int sendmail_t(char *from, long int size)
{
    char addr[MAXINETADDR];
    char *header, *p;
    int status=EX_OK, st;
    
    /*
     * To:
     */
    for(header=header_get("To"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	    if(addr_from_rfcaddr(p, addr))
		if((st = snd_mail(from, addr, size)) != EX_OK)
		    status = st;

    /*
     * Cc:
     */
    for(header=header_get("Cc"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	    if(addr_from_rfcaddr(p, addr))
		if((st = snd_mail(from, addr, size)) != EX_OK)
		    status = st;

    /*
     * Bcc:
     */
    for(header=header_get("Bcc"); header; header=header_getnext())
	for(p=addr_token(header); p; p=addr_token(NULL))
	    if(addr_from_rfcaddr(p, addr))
		if((st = snd_mail(from, addr, size)) != EX_OK)
		    status = st;

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
	 -L --lib-dir name            set lib directory\n\
	 -S --spool-dir name          set spool directory\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");
    
    exit(0);
}



/***** main ******************************************************************/
int main(int argc, char **argv)
{
    int cnt, c;
    int status=EX_OK, st;
    long size, nmsg;
    char *from, *p;
    int b_flag=FALSE, t_flag=FALSE;
    char *B_flag=NULL;
    char *O_flag=NULL;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
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
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();
    
    newsmode = FALSE;

    

    while ((c = getopt_long(argc, argv, "bB:o:niO:w:Wtvhc:L:S:a:u:",
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
	    addr_ignore(TRUE);
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
    cf_check_gate();
    
    /*
     * Process config options
     */
    if(B_flag)
	cf_set_outbound(B_flag);
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
	
	debug(8, "config: maxmsgsize %s", p);
	sz = atol(p);
	if(sz <= 0)
	    log("WARNING: illegal maxmsgsize value %s", p);
	else
	    areas_maxmsgsize(sz);
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
    

    /*
     * Process local options
     */
    if(O_flag)
	pkt_outdir(O_flag, NULL);
    else
	pkt_outdir(cf_spooldir(), OUTDIR);

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
	 * Read message body from stdin and count size
	 */
	size = 0;
	tl_clear(&body);
	while(read_line(buffer, BUFFERSIZE, stdin)) {
	    tl_append(&body, buffer);
	    size += strlen(buffer) + 1;	    /* `+1' for additional CR */
	}
	debug(1, "Message body size %ld (+CR!)", size);

	from = mail_address();		/* From address */

	if(newsmode)
	    /*
	     * Send mail to echo feed for news messages
	     */
	    status = snd_mail(from, NULL, size);
	else
	    if(t_flag)
	    {
		/*
		 * Send mail to addresses from headers
		 */
		status = sendmail_t(from, size);
	    }
	    else
	    {
		/*
		 * Send mail to addresses from command line args
		 */
		for(cnt = optind; cnt < argc; cnt++)
		    if( (st = snd_mail(from, argv[cnt], size)) != EX_OK )
			status = st;
	    }
	
	if(!b_flag)
	    break;
    }

    pkt_close();
    
    exit(status);

    /**NOT REACHED**/
    return 1;
}
