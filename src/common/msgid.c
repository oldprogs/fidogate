/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: msgid.c,v 4.1 1996/04/22 14:31:12 mj Exp $
 *
 * MSGID <-> Message-ID conversion handling. See also ../doc/msgid.doc
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
 *****************************************************************************
 * As an exception to this rule you may freely use the functions
 * contained in THIS module to implement the FIDO-Gatebau '94 specs
 * for FIDO-Internet gateways without making the resulting program
 * subject to the GNU General Public License.
 *****************************************************************************/

#include "fidogate.h"
#include "shuffle.h"



/*
 * Prototypes
 */
static void  msgid_fts9_quote	(char *, char *, int);
#ifdef MSGID_NEW_FTNID
static void  msgid_mime_quote	(char *, char *, int);
static char *msgid_domain	(int);
#endif


#ifdef MSGID_NEW_MSGID
/*
 * Quote string containing <SPACE> according to FTS-0009
 */
static void msgid_fts9_quote(char *d, char *s, int n)
{
    int i=0;
    int must_quote=FALSE;
    
    must_quote = strchr(s, ' ') || strchr(s, '\"');	/* " */
    
    if(must_quote)
	d[i++] = '\"';					/* " */
    for(; i<n-3 && *s; i++, s++)
    {
	if(*s == '\"')					/* " */
	    d[i++] = '\"';				/* " */
	d[i] = *s;
    }
    if(must_quote)
	d[i++] = '\"';					/* " */
    d[i] = 0;
}
#endif /**MSGID_NEW_MSGID**/



#ifdef MSGID_NEW_FTNID
/*
 * Quote ^AMSGID string using MIME-style quoted-printable =XX
 */
static void msgid_mime_quote(char *d, char *s, int n)
{
    int i, c;

    for(i=0; i<n-4 && *s && *s!='\r'; i++, s++)
    {
	c = *s & 0xff;
	if(c == ' ')
	    d[i] = '_';
	else if(strchr("()<>@,;:\\\"[]/=_", c) || c>=0x7f || c<0x20)
	{
	    sprintf(d+i, "=%02X", c);
	    i += 2;
	}
	else
	    d[i] = c;
    }
    d[i] = 0;
}



/*
 * Return Message-ID domain for FTN zone
 */
static char *msgid_domain(int zone)
{
    if(zone>=1 && zone<=6)		/* FIDONET */
	return MSGID_FIDONET_DOMAIN + 1;		/* +1 = skip leading `.' */
    else
	return cf_zones_inet_domain(zone) + 1;
}
#endif /**MSGID_NEW_FTNID**/



/*
 * Convert FIDO ^AMSGID/REPLY to RFC Message-ID/References
 */
char *msgid_fido_to_rfc(char *msgid, int *pzone)
{
    char *save;
    char *origaddr, *serialno;
    char *p, *s;
    Node idnode;
#ifdef MSGID_NEW_FTNID
    int zone;
#endif
    
    save = strsave(msgid);
    
    /*
     * Retrieve `origaddr' part
     */
    if(*save == '\"')					/* " */
    {
	/*
	 * Quoted: "abc""def" -> abc"def
	 */
	origaddr = save;
	p = save;
	s = save+1;
	while(*s)
	{
	    if(*s == '\"')				/* " */
	    {
		if(*(s+1) == '\"')			/* " */
		    s++;
		else
		    break;
	    }
	    *p++ = *s++;
	}
	if(*s == '\"')					/* " */
	    s++;
	while(*s && is_space(*s))
	    s++;
	*p = 0;
	serialno = s;
    }
    else
    {
	/*
	 * Not quoted
	 */
	origaddr = save;
	for(p=save; *p && !is_space(*p); p++) ;
	s = p;
	while(*s && is_space(*s))
	    s++;
	*p = 0;
	serialno = s;
    }

    /*
     * Retrieve `serialno' part
     */
    for(p=serialno; *p && !is_space(*p); p++) ;
    *p = 0;
    

    SHUFFLEBUFFERS;

    /***** New-style converted RFC Message-ID *****/
    if(wildmat(origaddr, "<*@*>"))
    {
	strncpy0(tcharp, origaddr, MAX_CONVERT_BUFLEN);
	xfree(save);
	if(pzone)
	    *pzone = -2;
	return tcharp;
    }
    

    /***** FTN-style *****/

#ifdef MSGID_NEW_FTNID

    /*
     * Search for parsable FTN address in origaddr
     */
    for(p=origaddr; *p && !isdigit(*p); p++) ;
    for(s=p; *s && (isdigit(*s) || *s==':' || *s=='/' || *s=='.'); s++) ;
    *s = 0;
    if(asc_to_node(p, &idnode, TRUE) != ERROR)	/* Address found */
    {
	zone = idnode.zone;
	if(pzone)
	    *pzone = zone;
    }
    else
    {
	zone = cf_zone();
	if(pzone)
	    *pzone = -1;
    }
    
    /*
     * New-style FTN Message-IDs using MIME quoted-printable
     */
    strncpy0(tcharp, "<MSGID_", MAX_CONVERT_BUFLEN);
    msgid_mime_quote(tcharp + strlen(tcharp), msgid,
		     MAX_CONVERT_BUFLEN - strlen(tcharp));
    strncat0(tcharp, "@", MAX_CONVERT_BUFLEN);
    strncat0(tcharp, msgid_domain(zone), MAX_CONVERT_BUFLEN);
    strncat0(tcharp, ">", MAX_CONVERT_BUFLEN);
    
#else /**!MSGID_NEW_FTNID**/

    /*
     * Old-style Message-ID: <abcd1234%DOMAIN@p.f.n.z.fidonet.org>
     */
    while(*serialno == '0')		/* Skip leading `0's */
	serialno++;

    if(asc_to_node(origaddr, &idnode, TRUE) != ERROR)
    {
	if(idnode.domain[0])		    /* With DOMAIN part */
	    sprintf(tcharp, "<%s%%%s@%s%s>",
		    serialno, idnode.domain, node_to_pfnz(&idnode, TRUE),
		    MSGID_FIDONET_DOMAIN);
	else
	    sprintf(tcharp, "<%s@%s%s>",
		    serialno, node_to_pfnz(&idnode, TRUE),
		    MSGID_FIDONET_DOMAIN);
    }
    else
    {
	sprintf(tcharp, "<%s@%s>", serialno, origaddr);
    }

#endif /**MSGID_NEW_FTNID**/

    xfree(save);
    return tcharp;
}



/*
 * Generate ID for FIDO messages without ^AMSGID, using date and CRC over
 * From, To and Subject.
 */
char *msgid_default(Node *node, char *msg_from, char *msg_to, char *msg_subj, time_t msg_date)
{
    SHUFFLEBUFFERS;

    /*
     * Compute CRC for strings from, to, subject
     */
    crc32_init();
    crc32_compute(msg_from, strlen(msg_from));
    crc32_compute(msg_to  , strlen(msg_to  ));
    crc32_compute(msg_subj, strlen(msg_subj));

#ifdef MSGID_NEW_FTNID

    sprintf(tcharp, "<NOMSGID_%d=3A%d=2F%d.%d_%s_%08lx@%s>",
	    node->zone, node->net, node->node, node->point,
	    date("%y%m%d_%H%M%S", &msg_date), crc32_value(),
	    msgid_domain(node->zone)                          );
    
#else

    sprintf(tcharp, "<NOMSGID-%s-%08lx@%s%s>",
	    date("%y%m%d-%H%M%S", &msg_date),
	    crc32_value(), node_to_pfnz(node, TRUE), MSGID_FIDONET_DOMAIN);

#endif
    
    return tcharp;
}



/*
 * Convert RFC Message-ID/References to FIDO ^AMSGID/^AREPLY
 */
char *msgid_rfc_to_fido(int *origid_flag, char *message_id, int part, int split, char *area)
                     			/* Flag for ^AORIGID */
                     			/* Original RFC-ID */
             				/* part number */
              				/* != 0 # of parts */
               				/* FTN AREA */
{
    char *id, *host, *p;
    char *savep;
    Node node, *n;
    int hexflag, i;
    char hexid[16];
    unsigned long crc32;

    /****** Extract id and host from <id@host> *****/

    savep = strsave(message_id);
    /*
     * Format of message_id is "<identification@host.domain> ..."
     * We want the the last one in the chain, which is the message id
     * of the article replied to.
     */
    id = strrchr(savep, '<');
    if(!id)
    {
	xfree(savep);
	return NULL;
    }
    id++;
    host = strchr(id, '@');
    if(!host)
    {
	xfree(savep);
	return NULL;
    }
    *host++ = 0;
    p = strchr(host, '>');
    if(!p)  
    {
	xfree(savep);
	return NULL;
    }
    *p = 0;

    /*
     * Don't convert <funpack....@...> and <NOMSGID-...@...> IDs
     * generated by FIDOGATE
     */
    if(!strncmp(id, "funpack", 7) || !strncmp(id, "NOMSGID", 8))
    {
	xfree(savep);
	return NULL;
    }


    SHUFFLEBUFFERS;

    /***** Check for old style FTN Message-IDs <abcd1234%domain@p.f.n.z> *****/

    if(!split)
    {
	/*
	 * First check ID. A FIDO Message-ID is a hex number with an
	 * optional %Domain string. The hex number must not start with
	 * `0' and it's length must be no more then 8 digits.
	 */
	node.domain[0] = 0;
	p = id;
	hexflag = isxdigit(*p) && *p!='0';
	for(p++, i=0; i<7 && *p && *p!='%'; i++, p++)
	    if(!isxdigit(*p))
		hexflag = FALSE;
	if(hexflag && *p=='%')		/* Domain part follows */
	{
	    *p++ = 0;
	    strncpy0(node.domain, p, MAX_DOMAIN);
	}
	else if(*p)
	    hexflag = FALSE;
	if(hexflag) {
	    /* Pad with leading 0's */
	    strcpy(hexid, "00000000");
	    strcpy(hexid + 8 - strlen(id), id);
	    
	    /* host must be an FTN address */
	    if( (n = inet_to_ftn(host)) )
	    {
		/* YEP! This is an old-style FTN Message-ID!!! */
		node.zone  = n->zone;
		node.net   = n->net;
		node.node  = n->node;
		node.point = n->point;

		sprintf(tcharp, "%s %s", node_to_asc(&node, TRUE), hexid);

		xfree(savep);
		if(origid_flag)
		    *origid_flag = FALSE;
		return tcharp;
	    }
	}
    } /**if(!split)**/


    /***** Check for new-style <MSGID_mimeanything@domain> *****/

    if(!strncmp(id, "MSGID_", 6))
    {
	p = id + strlen("MSGID_");
	
	mime_dequote(tcharp, MAX_CONVERT_BUFLEN, p, MIME_QP|MIME_US);
	
	xfree(savep);
	if(origid_flag)
	    *origid_flag = FALSE;
	return tcharp;
    }


    /***** Generate ^AMSGID according to msgid.doc specs *****/

#ifdef MSGID_NEW_MSGID

    /*
     * New-style FIDO-Gatebau '94 ^AMSGID
     */
    xfree(savep);
    savep = strsave(message_id);
    id = strrchr(savep, '<');
    if(!id)
	id = savep;
    p = strchr(id, '>');
    if(!p)
	p = id;
    else
	p++;
    *p = 0;

    crc32_init();
    crc32_compute((unsigned char *)id, strlen(id));
    if(area)
	crc32_compute((unsigned char *)area, strlen(area));
    crc32 = crc32_value();
    if(split)
	crc32 += part - 1;
		  
    msgid_fts9_quote(tcharp, id, MAX_CONVERT_BUFLEN);
    sprintf(tcharp + strlen(tcharp), " %08lx", crc32);
    
#else /**!MSGID_NEW_MSGID**/

    /*
     * Old-style Gatebau ^AMSGID
     */
    crc32 = compute_crc32((unsigned char *)id, strlen(id));
    if(split)
	crc32 += part - 1;
    sprintf(tcharp, "%s %08lx", host, crc32);

#endif /**MSGID_NEW_MSGID**/

    xfree(savep);
    if(origid_flag)
	*origid_flag = TRUE;
    return tcharp;
}



/*
 * Generate string for ^AORIGID/^AORIGREF kludge
 */
char *msgid_rfc_to_origid(char *message_id, int part, int split)
                 			/* Original RFC-ID */
         				/* part number */
          				/* != 0 # of parts */
{
    char *id, *p;
    char *savep;

    savep = strsave(message_id);
    /*
     * Format of message_id is "<identification@host.domain> ..."
     * We want the the last one in the chain, which is the message id
     * of the article replied to.
     */
    id = strrchr(savep, '<');
    if(!id)
    {
	xfree(savep);
	return NULL;
    }
    p = strchr(id, '>');
    if(!p)  
    {
	xfree(savep);
	return NULL;
    }
    *++p = 0;

    SHUFFLEBUFFERS;
    
    if(split)
	sprintf(tcharp, "%s %d/%d", id, part, split);
    else
	sprintf(tcharp, "%s", id);

    xfree(savep);
    return tcharp;
}



/*
 * Extract Message-ID from ^AORIGID/^AORIGREF with special handling for
 * split messages (appended " i/n"). Returns NULL for invalid ^AORIGID.
 */
char *msgid_convert_origid(char *origid, int part_flag)
{
    char *s, *p, *id, *part;
    
    s    = strsave(origid);

    id   = s;
    part = NULL;
    p    = strrchr(s, '>');
    if(!p)
    {
	xfree(s);
	debug(1, "Invalid ^AORIGID: %s", origid);
	return NULL;
    }

    p++;
    if(is_space(*p))
    {
	/*
	 * Indication of splitted message " p/n" follows ...
	 */
	*p++ = 0;
	while(is_space(*p)) p++;
	part = p;
    }

    /*
     * Message-IDs must NOT contain white spaces
     */
    if(strchr(id, ' ') || strchr(id, '\t'))
    {
	xfree(s);
	debug(1, "Invalid ^AORIGID: %s", origid);
	return NULL;
    }

    SHUFFLEBUFFERS;
    sprintf(tcharp, "%s", id);
    
    xfree(s);
    
    return tcharp;
}
