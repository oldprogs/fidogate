/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: config.c,v 4.7 1996/12/02 19:51:25 mj Exp $
 *
 * Configuration data and functions
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
 * Current line in config file
 */
static int scf_line;



/*
 * Directories
 */
static char scf_libdir[MAXPATH];
static char scf_spooldir[MAXPATH];
static char scf_logdir[MAXPATH];

/*
 * Hostname / domainname
 */
static char scf_hostname[MAXPATH];
static char scf_domainname[MAXPATH];
static char scf_hostsdomain[MAXPATH];
static char scf_fqdn[MAXPATH];


/*
 * Configured zones and addresses
 */
struct st_addr
{
    int zone;				/* Zone for this address set */
    Node addr;				/* Our own main address */
    Node uplink;			/* Uplink address */
};

static struct st_addr scf_addr[MAXADDRESS];
static int scf_naddr = 0;		/* # of addresses */
static int scf_zone  = 0;		/* Current zone */
static int scf_index = 0;		/* Index to current zone scf_addr[] */

static Node scf_c_addr   = { -1, -1, -1, -1, "" };	/* Current address */
static Node scf_c_uplink = { -1, -1, -1, -1, "" };	/* Current uplink  */

static int scf_ia = 0;			/* Index for Address */
static int scf_ir = 0;			/* Index for Uplink */



/*
 * Zones, domains, and outbound directories
 */
struct st_zones
{
    int zone;				/* Zone */
    char *inet_domain;			/* Internet domain */
    char *ftn_domain;			/* FTN domain */
    char *out;				/* Outbound subdir */
};

static struct st_zones scf_zones[MAXADDRESS];
static int scf_nzones = 0;		/* # of zones */
static int scf_izones = 0;		/* Index for cf_zones_trav() */


/*
 * Configured DOS drive -> UNIX path translation
 */
struct st_dos
{
    char *drive;			/* MSDOS drive */
    char *path;				/* UNIX path */
};

static struct st_dos scf_dos[MAXDOSDRIVE];
static int scf_ndos = 0;		/* # of DOS drives */


/*
 * FTN-Internet gateway
 */
static Node scf_gateway;


/*
 * All other, unknown config lines stored in linked list
 */
struct st_cflist
{
    char *key;
    char *string;
    struct st_cflist *next;
};

static struct st_cflist *scf_list_first = NULL;
static struct st_cflist *scf_list_last  = NULL;



/*
 * Check for corresponding "Address" and "Uplink" entries (config.gate)
 */
void cf_check_gate(void)
{
    if(scf_ir == 0)
    {
	log("ERROR: config: no uplink");
	if(!verbose)
	    fprintf(stderr, "ERROR: config: no uplink");
	exit(EX_USAGE);
    }
    
    if(scf_ia != scf_ir)
    {
	log("WARNING: config: %d address != %d uplink");
    }
}



/*
 * debug output of configuration
 */
void cf_debug(void)
{
    int i;

    debug(8, "config: libdir=%s  spooldir=%s  logdir=%s",
	  scf_libdir, scf_spooldir, scf_logdir);
    debug(8, "config: fqdn=%s", scf_fqdn);
    
    for(i=0; i<scf_naddr; i++)
	debug(8, "config: address Z%-4d: %s  Uplink: %s",
	      scf_addr[i].zone,
	      node_to_asc(&scf_addr[i].addr, TRUE),
	      node_to_asc(&scf_addr[i].uplink, TRUE)          );

    for(i=0; i<scf_nzones; i++)
	debug(8, "config: zone %-4d: %s  %s  %s", scf_zones[i].zone,
	      scf_zones[i].inet_domain, scf_zones[i].ftn_domain,
	      scf_zones[i].out                                 );

    debug(8, "config: gateway=%s", node_to_asc(&scf_gateway, TRUE));
}



/*
 * Return main AKA (independent of current zone setting)
 */
Node *cf_main_addr(void)
{
    return &scf_addr[0].addr;
}

Node cf_n_main_addr(void)
{
    return scf_addr[0].addr;
}



/*
 * Return current main/fakenet/uplink FTN address
 */
Node *cf_addr(void)
{
    return &scf_c_addr;
}

Node *cf_uplink(void)
{
    return &scf_c_uplink;
}

Node cf_n_addr(void)
{
    return scf_c_addr;
}

Node cf_n_uplink(void)
{
    return scf_c_uplink;
}



/*
 * Set current zone
 */
void cf_set_zone(int zone)
{
    int i;
    
    if(scf_naddr == 0) 
    {
	fprintf(stderr, "No FTN addresses configured.\n");
	exit(1);
    }

    scf_zone = zone;
    for(i=0; i<scf_naddr; i++)
	if(zone == scf_addr[i].zone)
	{
	    scf_index    = i;
	    scf_c_addr   = scf_addr[i].addr;
	    scf_c_uplink = scf_addr[i].uplink;
	    debug(9, "Select Z%d AKA: %s  Uplink: %s",
		  scf_addr[i].zone,
		  node_to_asc(&scf_addr[i].addr, TRUE),
		  node_to_asc(&scf_addr[i].uplink, TRUE)          );
	    return;
	}
    
    scf_index    = i = 0;
    scf_c_addr   = scf_addr[i].addr;
    scf_c_uplink = scf_addr[i].uplink;
    debug(9, "Select default AKA: %s  Uplink: %s",
	  node_to_asc(&scf_addr[i].addr, TRUE),
	  node_to_asc(&scf_addr[i].uplink, TRUE)          );
}


/*
 * Set current address
 */
void cf_set_curr(Node *node)
{
    cf_set_zone(node->zone);
    scf_c_addr = *node;
}



/*
 * Return current/default zone
 */
int cf_zone(void)
{
    return scf_zone;
}

int cf_defzone(void)
{
    return scf_addr[0].zone;
}



/*
 * Read line from config file. Strip `\n', leading spaces,
 * comments (starting with `#'), and empty lines. cf_getline() returns
 * a pointer to the first non-whitespace in buffer.
 */
char *cf_getline(char *buffer, int len, FILE *fp)
{
    char *p;

    scf_line = 0;
    while (fgets(buffer, len, fp)) {
	scf_line++;
	strip_crlf(buffer);
	if((p = strchr(buffer, '#')))		/* Strip comments */
	    *p = 0;
	for(p=buffer; *p && is_space(*p); p++) ;	/* Skip white spaces */
	if (*p)
	    return p;
    }
    return NULL;
}



/*
 * Process line from config file
 */
void cf_do_line(char *line)
{
    char *p, *keyword;

    keyword = xstrtok(line, " \t");
    if(!keyword)
	return;

    /***** include ******************************************************/
    if(!stricmp(keyword, "include"))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing include file");
	    return;
	}
	cf_read_config_file(p);
    }
    /***** hostname *****************************************************/
    else if (!stricmp(keyword, "hostname"))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing hostname");
	    return;
	}
	strncpy0(scf_hostname, p, MAXPATH);
    }
    /***** domain *******************************************************/
    else if (!stricmp(keyword, "domain"  ))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing domainname");
	    return;
	}
	if(p[0] != '.')
	{
	    strncpy0(scf_domainname, ".", sizeof(scf_domainname));
	    strncat0(scf_domainname, p  , sizeof(scf_domainname));
	}
	else
	    strncpy0(scf_domainname, p  , sizeof(scf_domainname));
	/* This is also the default for "HostsDomain" */
	strncpy0(scf_hostsdomain, scf_domainname, sizeof(scf_hostsdomain));
    }
    /***** hostsdomain **************************************************/
    else if (!stricmp(keyword, "hostsdomain"  ))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing domainname");
	    return;
	}
	if(p[0] != '.')
	{
	    strncpy0(scf_hostsdomain, ".", sizeof(scf_hostsdomain));
	    strncat0(scf_hostsdomain, p  , sizeof(scf_hostsdomain));
	}
	else
	    strncpy0(scf_hostsdomain, p  , sizeof(scf_hostsdomain));
    }
    /***** libdir *******************************************************/
    else if (!stricmp(keyword, "libdir"))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing libdir");
	    return;
	}
	strncpy0(scf_libdir, p, MAXPATH);
    }
    /***** spooldir *****************************************************/
    else if (!stricmp(keyword, "spooldir"))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing spooldir");
	    return;
	}
	strncpy0(scf_spooldir, p, MAXPATH);
    }
    /***** logdir *******************************************************/
    else if (!stricmp(keyword, "logdir"))
    {
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing logdir");
	    return;
	}
	strncpy0(scf_logdir, p, MAXPATH);
    }
    /***** address ******************************************************/
    else if (!stricmp(keyword, "address" ))
    {
	Node a;
	    
	/* address */
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing address");
	    return;
	}
	if( asc_to_node(p, &a, FALSE) == ERROR )
	{
	    log("config: illegal address %s", p);
	    return;
	}

	if(scf_ia < MAXADDRESS)
	{
	    scf_addr[scf_ia].zone   = a.zone;
	    scf_addr[scf_ia].addr   = a;
	    scf_ia++;
	}
	else
	    log("config: too many addresses");
    }
    /***** uplink *******************************************************/
    else if (!stricmp(keyword, "uplink" ))
    {
	Node a;
	    
	/* Main address */
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing address");
	    return;
	}
	if( asc_to_node(p, &a, FALSE) == ERROR )
	{
	    log("config: illegal address %s", p);
	    return;
	}

	if(scf_ir < MAXADDRESS)
	{
	    scf_addr[scf_ir].uplink = a;
	    scf_ir++;
	}
	else
	    log("config: too many addresses");
    }
    /***** zone *********************************************************/
    else if (!stricmp(keyword, "zone" ))
    {
	int zone;
	char *inet, *ftn, *out;

	if(scf_nzones >= MAXADDRESS)
	{
	    log("config: too many zones");
	    return;
	}
	    
	if(! (p = xstrtok(NULL, " \t")) )
	{
	    log("config: missing zone");
	    return;
	}
	if(!stricmp(p, "default"))
	    zone = 0;
	else 
	{
	    zone = atoi(p);
	    if(!zone)
	    {
		log("config: illegal zone value %s", p);
		return;
	    }
	}
	    
	if(! (inet = xstrtok(NULL, " \t")) )
	{
	    log("config: missing Internet domain");
	    return;
	}

	if(! (ftn = xstrtok(NULL, " \t")) )
	{
	    log("config: missing FTN domain");
	    return;
	}

	if(! (out = xstrtok(NULL, " \t")) )
	{
	    log("config: missing outbound directory");
	    return;
	}

	scf_zones[scf_nzones].zone        = zone;
	scf_zones[scf_nzones].inet_domain = strsave(inet);
	scf_zones[scf_nzones].ftn_domain  = strsave(ftn);
	scf_zones[scf_nzones].out         = strsave(out);
	scf_nzones++;
    }
    /***** dosdrive *****************************************************/
    else if (!stricmp(keyword, "dosdrive" ))
    {
	char *drive, *path;
	    
	if(scf_ndos >= MAXDOSDRIVE)
	{
	    log("config: too many DOS drives");
	    return;
	}
	if(! (drive = xstrtok(NULL, " \t")) )
	{
	    log("config: missing DOS drive");
	    return;
	}
	if(! (path = xstrtok(NULL, " \t")) )
	{
	    log("config: missing UNIX path");
	    return;
	}

	scf_dos[scf_ndos].drive = strsave(drive);
	scf_dos[scf_ndos].path  = strsave(path);
	scf_ndos++;
    }
    /***** gateway ******************************************************/
    else if (!stricmp(keyword, "gateway" ))
    {
	Node a;
	    
	/* Main address */
	p = xstrtok(NULL, " \t");
	if(!p) 
	{
	    log("config: missing address");
	    return;
	}
	if( asc_to_node(p, &a, FALSE) == ERROR )
	{
	    log("config: illegal address %s", p);
	    return;
	}
	scf_gateway = a;
    }
    /***** U n k n o w n ************************************************/
    else 
    {
	struct st_cflist *pl;
	
	p = xstrtok(NULL, "\n");
	if(p)
	    while(is_blank(*p)) p++;
	    
	pl = (struct st_cflist *)xmalloc(sizeof(struct st_cflist));

	pl->key    = strsave(keyword);
	pl->string = p ? strsave(p) : "";
	pl->next   = NULL;
	    
	if(scf_list_first)
	    scf_list_last->next = pl;
	else
	    scf_list_first = pl;
	scf_list_last = pl;
    }
}



/*
 * Read config file
 */
void cf_read_config_file(char *name)
{
    FILE *cf;
    char *line;
    
    if(!name || !*name)			/* Empty string -> no config file */
	return;

    cf = xfopen(name, R_MODE_T);
    
    while((line = cf_getline(buffer, BUFFERSIZE, cf)))
	cf_do_line(line);

    scf_naddr 	 = scf_ia;
    scf_zone  	 = scf_addr[0].zone;
    scf_index 	 = 0;
    scf_c_addr   = scf_addr[0].addr;
    scf_c_uplink = scf_addr[0].uplink;
    
    strncpy0(scf_fqdn, scf_hostname,   MAXPATH);
    strncat0(scf_fqdn, scf_domainname, MAXPATH);
    
    fclose(cf);
}




/*
 * Init configuration
 */
void cf_initialize(void)
{
    strncpy0(scf_libdir,   LIBDIR,   MAXPATH);
    strncpy0(scf_spooldir, SPOOLDIR, MAXPATH);
    strncpy0(scf_logdir,   LOGDIR,   MAXPATH);

    /*
     * Check for real uid != effective uid, setuid installed FIDOGATE
     * programs, and disable debug() output (-v on command line) in
     * this case.
     */
    if(getuid() != geteuid())
	no_debug = TRUE;
}



/*
 * Set FIDO address */
void cf_set_addr(char *addr)
{
    Node node;
    
    if( asc_to_node(addr, &node, FALSE) == ERROR )
    {
	Node *n = inet_to_ftn(addr);
	if(!n)
	{
	    fprintf(stderr, "Illegal FIDO address %s\n", addr);
	    exit(EX_USAGE);
	}
	node = *n;
    }

    scf_naddr        = 1;
    scf_addr[0].zone = node.zone;
    scf_addr[0].addr = node;
    scf_zone         = node.zone;
    scf_index        = 0;
    scf_c_addr       = scf_addr[0].addr;
    scf_c_uplink     = scf_addr[0].uplink;
}



/*
 * Set uplink FIDO address
 */
void cf_set_uplink(char *addr)
{
    Node node;
    
    if( asc_to_node(addr, &node, FALSE) == ERROR )
    {
	Node *n = inet_to_ftn(addr);
	if(!n)
	{
	    fprintf(stderr, "Illegal FIDO address %s\n", addr);
	    exit(EX_USAGE);
	}
	node = *n;
    }
    
    scf_naddr          = 1;
    scf_addr[0].uplink = node;
    scf_zone           = scf_addr[0].zone;
    scf_index          = 0;
    scf_c_addr         = scf_addr[0].addr;
    scf_c_uplink       = scf_addr[0].uplink;
}



/*
 * Set/get lib/spool/log directory
 */
void cf_set_libdir(char *dir)
{
    strncpy0(scf_libdir, dir, MAXPATH);
}

void cf_set_spooldir(char *dir)
{
    strncpy0(scf_spooldir, dir, MAXPATH);
}

void cf_set_logdir(char *dir)
{
    strncpy0(scf_logdir, dir, MAXPATH);
}

char *cf_libdir(void)
{
    return scf_libdir;
}

char *cf_spooldir(void)
{
    return scf_spooldir;
}

char *cf_logdir(void)
{
    return scf_logdir;
}



/*
 * Return hostname / domain name / fully qualified domain name
 */
char *cf_hostname(void)
{
    return scf_hostname;
}

char *cf_domainname(void)
{
    return scf_domainname;
}

char *cf_hostsdomain(void)
{
    return scf_hostsdomain;
}

char *cf_fqdn(void)
{
    return scf_fqdn;
}



/***** Stuff for processing zone info ****************************************/

/*
 * Return Internet domain for a FIDO zone
 */
char *cf_zones_inet_domain(int zone)
{
    int i;
    
    /*
     * Search zone
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone == zone)
	    return scf_zones[i].inet_domain;

    /*
     * Search default domain
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone == 0)
	    return scf_zones[i].inet_domain;

    return FTN_INVALID_DOMAIN;
}



/*
 * Check for valid zone
 */
int cf_zones_check(int zone)
{
    int i;
    
    /*
     * Search zone
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone && scf_zones[i].zone==zone)
	    return TRUE;

    return FALSE;    
}



/*
 * Traverse Internet domains
 */
char *cf_zones_trav(int first)
{
    if(first)
	scf_izones = 0;

    return scf_izones < scf_nzones
	   ? scf_zones[scf_izones++].inet_domain
	   : NULL;
}



/*
 * Return outbound directory for a FIDO zone
 */
char *cf_zones_out(int zone)
{
    int i;
    
    /*
     * Search zone
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone == zone)
	    return scf_zones[i].out;

    return NULL;
}



/*
 * Return FTN domain name for a FTN zone
 */
char *cf_zones_ftn_domain(int zone)
{
    int i;
    
    /*
     * Search zone
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone == zone)
	    return scf_zones[i].ftn_domain;

    /*
     * Search default domain
     */
    for(i=0; i<scf_nzones; i++)
	if(scf_zones[i].zone == 0)
	    return scf_zones[i].ftn_domain;

    return "fidonet";
}



/*
 * Traverse FTN addresses
 */
Node *cf_addr_trav(int first)
{
    static int iaddr;
    
    if(first)
	iaddr = 0;

    if(iaddr >= scf_naddr)		/* End of addresses */
	return NULL;

    return &scf_addr[iaddr++].addr;
}



/*
 * UNIX to MSDOS file name translation enabled
 */
int cf_dos(void)
{
    return scf_ndos != 0;
}



/*
 * Convert UNIX path on server to MSDOS path on client
 */
char *cf_dos_xlate(char *name)
{
    static char buf[MAXPATH];
    int i;
    char *s;
    int len;
    
    for(i=0; i<scf_ndos; i++)
    {
	len = strlen(scf_dos[i].path);
	if(!strncmp(name, scf_dos[i].path, len))
	{
	    BUF_COPY2(buf, scf_dos[i].drive, name+len);
	    for(s=buf; *s; s++)
		switch(*s)
		{
		case '/':	*s = '\\';  break;
		}
	    return buf;
	}
    }
    
    return NULL;
}



/*
 * Convert MSDOS path back to UNIX path
 */
char *cf_unix_xlate(char *name)
{
    static char buf[MAXPATH];
    int i;
    char *s;
    int len;
    
    for(i=0; i<scf_ndos; i++)
    {
	len = strlen(scf_dos[i].drive);
	if(!strncmp(name, scf_dos[i].drive, len))
	{
	    BUF_COPY2(buf, scf_dos[i].path, name+len);
	    for(s=buf; *s; s++)
		switch(*s)
		{
		case '\\':	*s = '/';  break;
		}
	    return buf;
	}
    }
    
    return NULL;
}



/*
 * Address of FTN-Internet gateway
 */
Node cf_gateway(void)
{
    return scf_gateway;
}



/*
 * Get config statement(s) string(s)
 */
char *cf_get_string(char *name, int first)
{
    static struct st_cflist *last_listp = NULL;
    char *string;
    
    if(first)
	last_listp = scf_list_first;
    
    while(last_listp)
    {
	if(!stricmp(last_listp->key, name))		/* Found */
	{
	    string     = last_listp->string;
	    last_listp = last_listp->next;
	    return string;
	}
	last_listp = last_listp->next;
    }

    return NULL;
}



/*
 * Get config values for Aliases, Areas, Hosts, Passwd, Packing, Routing
 * config files.
 */
char *cf_p_aliases(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Aliases", TRUE)) )
	    pval = DEF_ALIASES;
	debug(8, "config: Aliases %s", pval);
    }

    return pval;
}

char *cf_p_areas(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Areas", TRUE)) )
	    pval = DEF_AREAS;
	debug(8, "config: Areas %s", pval);
    }

    return pval;
}

char *cf_p_hosts(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Hosts", TRUE)) )
	    pval = DEF_HOSTS;
	debug(8, "config: Hosts %s", pval);
    }

    return pval;
}

char *cf_p_passwd(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Passwd", TRUE)) )
	    pval = DEF_PASSWD;
	debug(8, "config: Passwd %s", pval);
    }

    return pval;
}

char *cf_p_packing(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Packing", TRUE)) )
	    pval = DEF_PACKING;
	debug(8, "config: Packing %s", pval);
    }

    return pval;
}

char *cf_p_routing(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Routing", TRUE)) )
	    pval = DEF_ROUTING;
	debug(8, "config: Routing %s", pval);
    }

    return pval;
}

char *cf_p_history(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("History", TRUE)) )
	    pval = DEF_HISTORY;
	debug(8, "config: History %s", pval);
    }

    return pval;
}



/*
 * Get config values for Inbound, PInbound, UUInbound, Outbound
 */
char *cf_p_inbound(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Inbound", TRUE)) )
	    pval = INBOUND;
	debug(8, "config: Inbound %s", pval);
    }

    return pval;
}

static char *cf_p_s_pinbound(char *s)
{
    static char *pval;

    if(s)
	pval = strsave(s);
    
    if(! pval)
    {
	if( ! (pval = cf_get_string("PInbound", TRUE)) )
	    if( ! (pval = cf_get_string("Inbound", TRUE)) )
		pval = PINBOUND;
	debug(8, "config: PInbound %s", pval);
    }

    return pval;
}
    
char *cf_p_pinbound(void)
{
    return cf_p_s_pinbound(NULL);
}

char *cf_s_pinbound(char *s)
{
    return cf_p_s_pinbound(s);
}

char *cf_p_uuinbound(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("UUInbound", TRUE)) )
	    if( ! (pval = cf_get_string("Inbound", TRUE)) )
		pval = INBOUND;
	debug(8, "config: UUInbound %s", pval);
    }

    return pval;
}

static char *cf_p_s_outbound(char *s)
{
    static char *pval;

    if(s)
	pval = strsave(s);
    
    if(! pval)
    {
	if( ! (pval = cf_get_string("Outbound", TRUE)) )
	    pval = OUTBOUND;
	debug(8, "config: Outbound %s", pval);
    }

    return pval;
}
    
char *cf_p_outbound(void)
{
    return cf_p_s_outbound(NULL);
}

char *cf_s_outbound(char *s)
{
    return cf_p_s_outbound(s);
}



/*
 * Get config values for Organization, Origin
 */
char *cf_p_organization(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Organization", TRUE)) )
	    pval = "FIDOGATE";
	debug(8, "config: Organization %s", pval);
    }

    return pval;
}

char *cf_p_origin(void)
{
    static char *pval;

    if(! pval)
    {
	if( ! (pval = cf_get_string("Origin", TRUE)) )
	    pval = "FIDOGATE";
	debug(8, "config: Origin %s", pval);
    }

    return pval;
}
