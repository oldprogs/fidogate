/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX <-> FIDO
 *
 * $Id: address.c,v 4.0 1996/04/17 18:17:38 mj Exp $
 *
 * Parsing and conversion for FIDO and RFC addresses
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
#include "shuffle.h"



/*
 * Local prototypes
 */
static int try_pfnz		(Node *, char *, char *, char *);



/*
 * -i flag: don't bounce mails to unknown FTN addresses (not in HOSTS)
 */
int i_flag = FALSE;

/*
 * Address parsing error message
 */
char address_error[256];



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
 * int parse_address()	---  parse fidonet address to name, net/node
 *
 * Parameters:	char *address	--- address string
 *		char *name	--- pointer to return name
 *		Node *node	--- pointer to return node address
 *
 * Return:	0 = o.k., -1 = error
 */

static int is_fido_flag;

int parse_address(char *address, char *name, Node *node)
{
    RFCAddr rfc;
    char *p;
    int len, ret;
    Node *n;
    Host *h;
    int in_domain;
    
    is_fido_flag = FALSE;

    rfc       = rfcaddr_from_rfc(address);
    in_domain = addr_is_domain(address);
    
    debug(3, "Name    %s", rfc.user);
    debug(3, "Address %s %s", rfc.addr, in_domain ? "(local domain)" : "");

    /*
     * Remove quotes "..." and copy to name[] arg
     */
    p = rfc.user;
    if(*p == '\"')			/* " makes C-mode happy */
    {
	p++;
	len = strlen(p);
	if(p[len-1] == '\"')		/* " makes C-mode happy */
	    p[len-1] = 0;
    }
    strncpy0(name, p, MSG_MAXNAME);

    /*
     * Special handling for addresses `*.maus.de'. These adresses are
     * converted to the form suitable for the FIDO<->MAUS gateway.
     */
    if(maus_domain)
    {
	int i, dlen, diff;

	len  = strlen(rfc.addr);
	dlen = strlen(maus_domain);
	diff = len - dlen;
	if(len > dlen                          &&
	   !strcmp(rfc.addr+diff, maus_domain)   )  /* Got it! */
	{
	    debug(2, "MAUS address %s", rfc.addr);

	    strncat0(name, "_%_", MSG_MAXNAME);
	    for(i=strlen(name), p=rfc.addr;
		i<MSG_MAXNAME-1 && *p && *p!='.';
		i++, p++                          )
		name[i] = toupper(*p);
	    name[i] = 0;
	    debug(3, "     converted to %s", name);

	    *node = maus_gate;

	    is_fido_flag = TRUE;
	    return OK;
	}
    }


    n = inet_to_ftn(rfc.addr);
    if(!n)
    {
	/* Try as Z:N/F.P */
	if(asc_to_node(rfc.addr, node, FALSE) == OK)
	    n = node;
    }
    
    if(n)
    {
	*node = *n;
	is_fido_flag = TRUE;
	ret = OK;
	debug(3, "FTN Node %s", node_to_asc(node, TRUE));

	/*
	 * Look up in HOSTS
	 */
	if( (h = hosts_lookup(node, NULL)) )
	{
	    if(!in_domain && (h->flags & HOST_DOWN))
	    {
		/* Node is down, bounce mail */
		sprintf(address_error,
			"FTN address %s: currently down, unreachable",
			node_to_asc(node, TRUE));
		ret = ERROR;
	    }
	}
#ifdef HOSTS_RESTRICTED
	/*
	 * Bounce mail to nodes not registered in HOSTS
	 */
	else if(!i_flag)
	{
	    if(!hosts_lookup(node, NULL))
	    {
		sprintf(address_error,
			"FTN address %s: not registered for this domain",
			node_to_asc(node, TRUE));
		ret = ERROR;
	    }
	}
#endif /**HOSTS_RESTRICTED**/

	/*
	 * Check for supported zones (zone statement in CONFIG)
	 */
	if(!cf_zones_check(node->zone))
	{
	    sprintf(address_error,
		    "FTN address %s: zone %d not supported",
		    node_to_asc(node, TRUE), node->zone);
	    ret = ERROR;
	}
	
    }
    else if(cf_gateway().zone)
    {
	/*
	 * If Gateway is set in config file, insert address of
	 * FIDO<->Internet gateway for non-FIDO addresses
	 */
	*node = cf_gateway();
	strcpy(name, "UUCP");
	
	ret = OK;
    }
    else
	ret = ERROR;

    return ret;
}



/***** isfido() --- Check for FIDO Internet address **************************/

int isfido(void)
{
    return is_fido_flag;
}



/*
 * ftn_to_inet(): convert FTN address to Internet address
 *
 * force_flag==TRUE: generate `pX.' point address even if `-p' is not
 *                   specified in HOSTS.
 */

char *ftn_to_inet(Node *node, int force_flag)
{
    Host *h;
    int point_flag;

    SHUFFLEBUFFERS;

    h = hosts_lookup(node, NULL);

    if(h)				/* Address found in HOSTS */
    {
	point_flag = h->flags & HOST_POINT ? TRUE : force_flag;

	if(h->name)
	{
	    if(point_flag && node->point && !h->node.point)
		sprintf(tcharp, "p%d.%s", node->point, h->name);
	    else
		sprintf(tcharp, "%s", h->name);
	}
	else
	    sprintf(tcharp, "%s%s", node_to_pfnz(node, FALSE),
		    cf_hostsdomain()                          );
    }
    else
	sprintf(tcharp, "%s%s", node_to_pfnz(node, FALSE),
		cf_zones_inet_domain(node->zone));

    return tcharp;
}



/*
 * inet_to_ftn(): convert Internet address to FTN address
 */
static int try_pfnz(Node *node, char *addr, char *dot, char *domain)
{
    char dom[MAXINETADDR];
    char adr[MAXINETADDR];

    int len  = strlen(addr);
    int dlen = strlen(dot) + strlen(domain);
    
    strncpy0(adr, addr  , sizeof(adr));
    strncpy0(dom, dot   , sizeof(dom));
    strncat0(dom, domain, sizeof(dom));
    
    if(len > dlen  &&  !stricmp(adr+len-dlen, dom))
    {
	adr[len - dlen] = 0;
	if(pfnz_to_node(adr, node, FALSE) == OK)
	    return TRUE;
    }
    
    return FALSE;
}


Node *inet_to_ftn(char *addr)
{
    char buf[MAXINETADDR];
    static Node node;
    Host *host;
    char *p, *pn;
    int point = -1;

    /*
     * Look for optional point addressing in front of host name
     */
    p = addr;
    if(*p=='p' || *p=='P') {
	p++;
	pn = p;
	while(*p && isdigit(*p))
	    p++;
	if(*p == '.') {				/* Must end with '.' */
	    p++;
	    point = atoi(pn);			/* Point number */
	}
	else
	    p = addr;				/* No pX. */
    }

    /*
     * 1. Lookup in HOSTS
     */
    if( (host = hosts_lookup(NULL, p)) )
    {
	node = host->node;
	if(!node.point && point!=-1)
	    node.point = point;
	return &node;
    }
    
    /*
     * 2. Add domainname and lookup in HOSTS
     */
    strncpy0(buf, p              , sizeof(buf));
    strncat0(buf, cf_domainname(), sizeof(buf));
    if( (host = hosts_lookup(NULL, buf)) )
    {
	node = host->node;
	if(!node.point && point!=-1)
	    node.point = point;
	return &node;
    }

    /*
     * 2a. Add hosts domainname and lookup in HOSTS
     */
    strncpy0(buf, p               , sizeof(buf));
    strncat0(buf, cf_hostsdomain(), sizeof(buf));
    if( (host = hosts_lookup(NULL, buf)) )
    {
	node = host->node;
	if(!node.point && point!=-1)
	    node.point = point;
	return &node;
    }

    strncpy0(buf, addr, sizeof(buf));
    
    /*
     * 3. Try p.f.n.z
     */
    if( try_pfnz(&node, buf, "", "") )
	return &node;
    
    /*
     * 4. Try p.f.n.z.HOSTNAME
     */
    if( try_pfnz(&node, buf, ".", cf_hostname()) )
	return &node;
    
    /*
     * 5. Try p.f.n.z.HOSTNAME.DOMAIN
     */
    if( try_pfnz(&node, buf, ".", cf_fqdn()) )
	return &node;
    
    /*
     * 6. Try p.f.n.z.DOMAIN
     */
    if( try_pfnz(&node, buf, "", cf_domainname()) )
	return &node;
    
    /*
     * 6a. Try p.f.n.z.HOSTSDOMAIN
     */
    if( try_pfnz(&node, buf, "", cf_hostsdomain()) )
	return &node;
    
    /*
     * 7. Try FTN domains from CONFIG
     */
    for (p=cf_zones_trav(TRUE); p; p=cf_zones_trav(FALSE))
	if( try_pfnz(&node, buf, "", p) )
	    return &node;
	
    /*
     * Everything failed - not an FTN address
     */
    return NULL;
}



/*
 * Check for local RFC address, i.e. "user@HOSTNAME.DOMAIN (Full Name)"
 * or "user (Full Name)"
 */
int addr_is_local(char *addr)
{
    RFCAddr rfc;
    
    rfc = rfcaddr_from_rfc(addr);
    
    return  rfc.addr[0] == '\0'  ||  stricmp(rfc.addr, cf_fqdn()) == 0;
}



/*
 * Check for address in local domain, i.e. "user@*DOMAIN (Full Name)"
 * or "user (Full Name)"
 */
int addr_is_domain(char *addr)
{
    RFCAddr rfc;
    char *d;
    int l, ld;
    
    rfc = rfcaddr_from_rfc(addr);
    
    if(rfc.addr[0] == '\0')
	return TRUE;
    
    d  = cf_domainname();
    ld = strlen(d);
    l  = strlen(rfc.addr);

    if(ld > l)
	return FALSE;
    
    /* user@DOMAIN */
    if(*d == '.' && stricmp(rfc.addr, d) == 0)
	return TRUE;
    /* user@*.DOMAIN */
    return stricmp(rfc.addr + l - ld, d) == 0;
}
