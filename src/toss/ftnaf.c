/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: ftnaf.c,v 4.13 1997/11/09 16:37:46 mj Exp $
 *
 * Areafix-like AREAS.BBS EchoMail distribution manager. Commands somewhat
 * conforming to FSC-0057.
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

#include <signal.h>



#define PROGRAM		"ftnaf"
#define VERSION		"$Revision: 4.13 $"
#define CONFIG		CONFIG_MAIN



#define MY_NAME_AF	"Areafix"
#define MY_CONTEXT_AF	"af"
#define MY_AREASBBS_AF	"AreasBBS"

#define MY_NAME_FF	"Filefix"
#define MY_CONTEXT_FF	"ff"
#define MY_AREASBBS_FF	"FAreasBBS"

#define MY_NAME		my_name
#define MY_CONTEXT	my_context

#define MAILER		"/usr/lib/sendmail -t"



/*
 * Number of old AREAS.BBS to keep as AREAS.Onn
 */
#define N_HISTORY	5



/*
 * Prototypes
 */
int	is_wildcard		(char *);
FILE   *mailer_open		(char *);
int     mailer_close		(FILE *);
int	rewrite_areas_bbs	(void);
int	do_mail			(void);
int	do_command		(Node *, char *);
int	cmd_create		(Node *, char *);
int	cmd_vacation		(Node *, char *);
int	cmd_list		(Node *);
int	cmd_listall		(Node *);
int	cmd_query		(Node *);
int	cmd_unlinked		(Node *);
int	cmd_add			(Node *, char *);
int	cmd_remove		(Node *, char *);
int	cmd_help		(Node *);
int	cmd_passwd		(Node *, char *);

void	short_usage		(void);
void	usage			(void);



/*
 * Areafix (TRUE) / Filefix (FALSE) flag
 */
static int   areafix = TRUE;


/*
 * Command line options
 */
static int   m_flag = FALSE;
static int   r_flag = FALSE;
static int   n_flag = FALSE;

static int   areas_bbs_changed = FALSE;

static int   authorized     	= FALSE;
static int   authorized_lvl 	= 1;
static char *authorized_key 	= "";
static char *authorized_name    = "Sysop";
static int   authorized_cmdline = FALSE;
static int   authorized_create  = FALSE;

static FILE *output = NULL;


/*
 * Program name, context, config verb name
 */
static char *my_name	= MY_NAME_AF;
static char *my_context = MY_CONTEXT_AF;
static char *my_areasbbs= MY_AREASBBS_AF;


/*
 * Name of areas.bbs file
 */
static char *areas_bbs = NULL;



/*
 * Check for wildcard char in area name
 */
int is_wildcard(char *s)
{
    return
	strchr(s, '*') ||
	strchr(s, '?') ||
	strchr(s, '[')    ;
}



/*
 * Open mailer for sending reply
 */
FILE *mailer_open(char *to)
{
    FILE *fp;
    char *cc;
    
    fp = popen(MAILER, W_MODE);
    if(!fp)
    {
	log("$ERROR: can't open pipe to %s", MAILER);
	return NULL;
    }
    
    fprintf(fp, "From: %s-Daemon@%s (%s Daemon)\n",
	    MY_NAME, cf_fqdn(), MY_NAME);
    fprintf(fp, "To: %s\n", to);
    if( (cc = cf_get_string("CCMail", TRUE)) )
	fprintf(fp, "Cc: %s\n", cc);
    fprintf(fp, "Subject: Your %s request\n", MY_NAME);
    fprintf(fp, "\n");
    
    return fp;
}



/*
 * Close mailer
 */
int mailer_close(FILE *fp)
{
    return pclose(fp);
}



/*
 * Rewrite AREAS.BBS if changed
 */
int rewrite_areas_bbs(void)
{
    char old[MAXPATH], new[MAXPATH];
    int i, ovwr;
    FILE *fp;
    
    if(!areas_bbs_changed)
    {
	debug(2, "AREAS.BBS not changed");
	return OK;
    }

    /*
     * Base name
     */
    str_expand_name(buffer, MAXPATH, areas_bbs);
    ovwr = strlen(buffer) - 3;		/* 3 = extension "bbs" */
    if(ovwr < 0)			/* Just to be sure */
	ovwr = 0;

    /*
     * Write new one as AREAS.NEW
     */
    strcpy(new, buffer);
    strcpy(new+ovwr, "new");
    debug(2, "Writing %s", new);
    if(!n_flag)
    {
	if( (fp = fopen(new, W_MODE)) == NULL )
	{
	    log("$ERROR: can't open %s for writing AREAS.BBS", new);
	    return ERROR;
	}
	if( areasbbs_print(fp) == ERROR )
	{
	    log("$ERROR: writing to %s", new);
	    fclose(fp);
	    unlink(new);
	    return ERROR;
	}
	if( fclose(fp) == ERROR )
	{
	    log("$ERROR: closing %s", new);
	    unlink(new);
	    return ERROR;
	}
    }

    /*
     * Renumber saved AREAS.Onn
     */
    strcpy(old, buffer);
    sprintf(old+ovwr, "o%02d", N_HISTORY);
    debug(2, "Removing %s", old);
    if(!n_flag)
	unlink(old);
    for(i=N_HISTORY-1; i>=1; i--)
    {
	strcpy(old, buffer);
	sprintf(old+ovwr, "o%02d", i);
	strcpy(new, buffer);
	sprintf(new+ovwr, "o%02d", i+1);
	debug(2, "Renaming %s -> %s", old, new);
	if(!n_flag)
	    rename(old, new);
    }
    
    /*
     * Rename AREAS.BBS -> AREAS.O01
     */
    strcpy(old, buffer);
    strcpy(old+ovwr, "bbs");
    strcpy(new, buffer);
    strcpy(new+ovwr, "o01");
    debug(2, "Renaming %s -> %s", old, new);
    if(!n_flag)
	rename(old, new);
    
    /*
     * Rename AREAS.NEW -> AREAS.BBS
     */
    strcpy(old, buffer);
    strcpy(old+ovwr, "new");
    strcpy(new, buffer);
    strcpy(new+ovwr, "bbs");
    debug(2, "Renaming %s -> %s", old, new);
    if(!n_flag)
	rename(old, new);
    
    return OK;
}



/*
 * Process request message in stdin
 */
int do_mail(void)
{
    RFCAddr from;
    char *pfrom, *p, *q;
    char *passwd;
    Node node, *n;
    Passwd *pwd;
    int q_flag=FALSE, l_flag=FALSE;
    
    node_invalid(&node);
    
    /*
     * Read message header from stdin
     */
    header_delete();
    header_read(stdin);

    pfrom = header_get("From");
    if(!pfrom)
	return EX_UNAVAILABLE;
    debug(3, "From: %s", pfrom);

    /*
     * Open mailer
     */
    if(r_flag)
    {
	output = mailer_open(pfrom);
	if(!output)
	    return EX_OSERR;
    }
    else
        output = stdout;

    /*
     * Check From / X-FTN-From for FTN address
     */
    n = NULL;
    if((p = header_get("X-FTN-From")))
    {
	debug(3, "X-FTN-From: %s", p);
	if((p = strchr(p, '@')))
	{
	    p++;
	    while(*p && is_space(*p))
		p++;
	    if(asc_to_node(p, &node, FALSE) == OK)
		n = &node;
	}
    }
    else
    {
	from = rfcaddr_from_rfc(pfrom);
	n    = inet_to_ftn(from.addr);
    }
    
    if(n)
    {
	debug(3, "FTN address: %s", node_to_asc(n, TRUE));
	node = *n;
	cf_set_zone(n->zone);
    }
    else
	node_invalid(&node);

    /*
     * Check password in Subject and process options
     */
    if( (p = header_get("Subject")) )
    {
	passwd = strtok(p, " \t");
	debug(3, "Subject passwd: %s", passwd);
	pwd    = passwd_lookup(MY_CONTEXT, &node);
	debug(3, "passwd entry: %s", pwd ? pwd->passwd : "-NONE-");
	
	if(passwd && pwd && !stricmp(passwd, pwd->passwd))
	{
	    debug(3, "passwd check o.k. - authorized");
	    authorized = TRUE;
	}

	while( (q = strtok(NULL, " \t")) )
	{
	    if(!stricmp(q, "-q"))		/* -q = QUERY */
		q_flag = TRUE;
	    if(!stricmp(q, "-l"))		/* -l = LIST */
		l_flag = TRUE;
	}

	/* Extract level, key, and real name from pwd->args */
	if(authorized)
	{
	    authorized_lvl  = 1;
	    authorized_key  = "";
	    authorized_name = "Sysop";
	    /* Warning: destroys pwd->args! */
	    if( (p = xstrtok(pwd->args, " \t")) )
		authorized_lvl = atoi(p);
	    if( (p = xstrtok(NULL, " \t")) )
		authorized_key = strsave(p);
	    if( (p = xstrtok(NULL, " \t")) )
		authorized_name = strsave(p);

	    debug(3, "passwd lvl : %d", authorized_lvl);
	    debug(3, "passwd key : %s", authorized_key);
	    debug(3, "passwd name: %s", authorized_name);
	}
    }

    /*
     * Execute commands in message body
     */
    while(fgets(buffer, sizeof(buffer), stdin))
    {
	strip_crlf(buffer);			/* Strip CR/LF */
	strip_space(buffer);			/* Strip spaces */
	if(!strncmp(buffer, " * ", 3))		/* Skip " * blah" lines */
	    continue;
	if(!strncmp(buffer, "---", 3))		/* Ignore cmds after --- */
	    break;
	if(!strncmp(buffer, "--", 2))		/* Ignore cmds after --  */
	    break;				/* (signature start)     */
	if(!strncmp(buffer, "--=20", 5))	/* dito, MIME            */
	    break;
	for(p=buffer; *p && is_space(*p); p++) ;	/* Skip white space */
	if(!*p)					/* Skip empty lines */
	    continue;

	do_command(&node, p);
    }

    /*
     * Execute commands for options
     */
    if(q_flag)
	cmd_query(&node);
    if(l_flag)
	cmd_list(&node);
    

    if(!authorized)
	/* Trouble? Give some more help ... */
	fprintf(output, "\n\
Your %s request\n\
    From: %s\n\
    Node: %s\n\
didn't made it, because authorization failed. You may try using the\n\
PASSWORD command to set address and password, if there is an address\n\
problem (the Node: address above doesn't seem to be the one you intended).\n\
\n\
Additional help on the usage of %s may be requested with the HELP command.\n",
		PROGRAM, pfrom,
		node.zone!=-1 ? node_to_asc(&node, TRUE) : "no FTN address",
		PROGRAM);

    return r_flag ? mailer_close(output) : EX_OK;
}



/*
 * Process command line
 */
#define CMD_LIST	1
#define CMD_QUERY	2
#define CMD_UNLINKED	3
#define CMD_ADD		4
#define CMD_REMOVE	5
#define CMD_HELP	6
#define CMD_PASSWD	7
#define CMD_LISTALL     8
#define CMD_CREATE	9
#define CMD_VACATION	10

int do_command(Node *node, char *line)
{
    int cmd;
    char *arg;
    char buf[32];
    int i, ret;
    int percent = FALSE;
    
    if(line[0] == '+')
    {
	cmd = CMD_ADD;
	arg = line + 1;
    }
    else if(line[0] == '-')
    {
	cmd = CMD_REMOVE;
	arg = line + 1;
    }
    else
    {
	if(line[0] == '%')
	{
	    percent = TRUE;
	    line++;
	}
	for(i=0; line[i] && !is_space(line[i]) && i<sizeof(buf)-1; i++)
	    buf[i] = line[i];
	buf[i] = 0;
	arg = line + i;
	
	if     (!stricmp(buf, "list"))
	    cmd = CMD_LIST;
	else if(!stricmp(buf, "query"))
	    cmd = CMD_QUERY;
	else if(!stricmp(buf, "unlinked"))
	    cmd = CMD_UNLINKED;
	else if(!stricmp(buf, "add"))
	    cmd = CMD_ADD;
	else if(!stricmp(buf, "remove"))
	    cmd = CMD_REMOVE;
	else if(!stricmp(buf, "delete"))
	    cmd = CMD_REMOVE;
	else if(!stricmp(buf, "del"))
	    cmd = CMD_REMOVE;
	else if(!stricmp(buf, "help"))
	    cmd = CMD_HELP;
	else if(!stricmp(buf, "passwd"))
	    cmd = CMD_PASSWD;
	else if(!stricmp(buf, "password"))
	    cmd = CMD_PASSWD;
	else if(!stricmp(buf, "pass"))
	    cmd = CMD_PASSWD;
	else if(!stricmp(buf, "listall"))
	    cmd = CMD_LISTALL;
	else if(!stricmp(buf, "create"))
	    cmd = CMD_CREATE;
	else if(!stricmp(buf, "vacation"))
	    cmd = CMD_VACATION;
	else
	{
	    if(percent)
	    {
		fprintf(output, "Unknown command %%%s\n", buf);
		return OK;
	    }
	    else
	    {
		/* Interpret line as area to add */
		cmd = CMD_ADD;
		arg = line;
	    }
	}
    }

    while(*arg && is_space(*arg))
	arg++;

    ret = OK;
    switch(cmd)
    {
    case CMD_LIST:
	ret = cmd_list(node);
	break;
    case CMD_QUERY:
	ret = cmd_query(node);
	break;
    case CMD_UNLINKED:
	ret = cmd_unlinked(node);
	break;
    case CMD_ADD:
	ret = cmd_add(node, arg);
	break;
    case CMD_REMOVE:
	ret = cmd_remove(node, arg);
	break;
    case CMD_HELP:
	ret = cmd_help(node);
	break;
    case CMD_PASSWD:
	ret = cmd_passwd(node, arg);
	break;
    case CMD_LISTALL:
	ret = cmd_listall(node);
	break;
    case CMD_CREATE:
	ret = cmd_create(node, arg);
	break;
    case CMD_VACATION:
	ret = cmd_vacation(node, arg);
	break;
    }	
    
    return ret;
}



/*
 * Create command
 */
int cmd_create(Node *node, char *line)
{
    AreasBBS *p;
    char *name, *o1, *o2;

    if(!authorized_create)
    {
	fprintf(output, "Command CREATE: not authorized.\n");
	return OK;
    }

    name = xstrtok(line, " \t");

    if( (p = areasbbs_lookup(name)) )
    {
	fprintf(output, "%s: area already exists, can't create a new one\n",
		name);
	return OK;
    }

    /* Create new areas.bbs entry */
    p = (AreasBBS *)xmalloc(sizeof(AreasBBS));
    
    p->next  = NULL;
    p->flags = 0;
    p->dir   = "-";
    p->area  = strsave(name);
    p->zone  = node->zone;
    node_invalid(&p->addr);
    p->lvl   = -1;
    p->key   = NULL;
    p->desc  = NULL;

    /* Parse options:
     *
     *     -#            passthru
     *     -p            passthru
     *     -r            read-only
     *     -l LVL        Areafix access level
     *     -k KEY        Areafix access key   */
    while( (o1 = xstrtok(NULL, " \t")) )
    {
	if(streq(o1, "-#") || streq(o1, "-p"))		/* -# */
	    p->flags |= AREASBBS_PASSTHRU;
	    
	if(streq(o1, "-r"))				/* -r */
	    p->flags |= AREASBBS_READONLY;
	    
	if(streq(o1, "-l"))				/* -l LVL */
	{
	    if(! (o2 = xstrtok(NULL, " \t")) )
		break;
	    p->lvl = atoi(o2);
	}
	
	if(streq(o1, "-k"))				/* -k KEY */
	{
	    if(! (o2 = xstrtok(NULL, " \t")) )
		break;
	    p->key = strsave(o2);
	}

	if(streq(o1, "-d"))				/* -d DESC */
	{
	    if(! (o2 = xstrtok(NULL, " \t")) )
		break;
	    p->desc = strsave(o2);
	}
    }	
    
    lon_init(&p->nodes);
    lon_add(&p->nodes, node);

    areasbbs_add(p);

    log("%s: created %s lvl=%d key=%s desc=%s%s%s",
	node_to_asc(node, TRUE),
	p->area,
	p->lvl,
	p->key ? p->key : "",
	p->desc ? p->desc : "",
	p->flags & AREASBBS_PASSTHRU ? " passthru" : "",
	p->flags & AREASBBS_READONLY ? " ro" : "");

    areas_bbs_changed = TRUE;

    return OK;
}



/*
 * Vacation command
 */
int cmd_vacation(Node *node, char *area)
{
    if(!authorized)
    {
	fprintf(output, "Command VACATION: not authorized.\n");
	return OK;
    }

    fprintf(output, "Command VACATION: sorry, not yet implemented.\n");

    return OK;
}



/*
 * ListAll command
 */
int cmd_listall(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    if(!authorized)
    {
	fprintf(output, "Command LISTALL: not authorized.\n");
	return OK;
    }
    
    fprintf(output, "\nALL available areas:\n\n");
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	fprintf(output, lon_search(l, node) ? "* " : "  ");
	fprintf(output, "Z%-3d %-39s%s%s\n", p->zone, p->area,
		p->desc ? ": " : "", p->desc ? p->desc : ""   );
    }
    
    fprintf(output, "\n* = linked to this area\n\n");

    return OK;
}



/*
 * List command
 */
int cmd_list(Node *node)
{
    AreasBBS *p;
    LON *l;
    char *s;
    int key_ok;
    
    if(!authorized)
    {
	fprintf(output, "Command LIST: not authorized.\n");
	return OK;
    }
    
    fprintf(output, "\nAreas available to %s:\n\n", node_to_asc(node, FALSE));
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	/* Check permissions */
	if(p->lvl > authorized_lvl)
	    continue;
	if(p->key)
	{
	    key_ok = TRUE;
	    for(s=p->key; *s; s++)
		if(!strchr(authorized_key, *s))
		{
		    key_ok = FALSE;
		    break;
		}
	    if(!key_ok)
		continue;
	}

	/* Check zone */
	if(areafix && p->zone!=node->zone)
	    continue;
	
	fprintf(output, lon_search(l, node) ? "* " : "  ");
	fprintf(output, "%-39s%s%s\n", p->area,
		p->desc ? ": " : "", p->desc ? p->desc : ""   );
    }
    
    fprintf(output, "\n* = linked to this area\n\n");
    
    return OK;
}



/*
 * Query command
 */
int cmd_query(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    if(!authorized)
    {
	fprintf(output, "Command QUERY: not authorized.\n");
	return OK;
    }
    
    fprintf(output, "\n%s is linked to the following areas:\n\n",
	    node_to_asc(node, FALSE));
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	if(lon_search(l, node))
	    fprintf(output, "  %s\n", p->area);
    }
    
    fprintf(output, "\n");
    
    return OK;
}



/*
 * Unlinked command
 */
int cmd_unlinked(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    if(!authorized)
    {
	fprintf(output, "Command UNLINKED: not authorized.\n");
	return OK;
    }
    
    fprintf(output, "\n%s is not linked to the following available areas:\n\n",
	    node_to_asc(node, FALSE));
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	/* Check permissions */

	/* Check zone */
	if(areafix && p->zone!=node->zone)
	    continue;
	
	if(! lon_search(l, node))
	    fprintf(output, "  %s\n", p->area);
    }
    
    fprintf(output, "\n");

    return OK;
}



/*
 * Add command
 */
int cmd_add(Node *node, char *area)
{
    AreasBBS *p;
    LON *l;
    int match = FALSE;
    char *s;
    int key_ok;
    int iswc;
    
    if(!authorized)
    {
	fprintf(output, "Command ADD: not authorized.\n");
	return OK;
    }

    iswc = is_wildcard(area);
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	if(wildmatch(p->area, area, TRUE))
	{
	    match = TRUE;

	    if(!authorized_cmdline)	/* Command line may do everything */
	    {
		/* Check permissions */
		if(p->lvl > authorized_lvl)
		{
		    if(!iswc)
			fprintf(output, "%-39s: access denied (level)\n",
				p->area);
		    continue;
		}
		if(p->key)
		{
		    key_ok = TRUE;
		    for(s=p->key; *s; s++)
			if(!strchr(authorized_key, *s))
			{
			    key_ok = FALSE;
			    break;
			}
		    if(!key_ok)
		    {
			if(!iswc)
			    fprintf(output, "%-39s: access denied (key)\n",
				    p->area);
			continue;
		    }
		}

		/* Check zone */
		if(areafix && p->zone!=node->zone)
		{
		    if(!iswc)
			fprintf(output,
				"%-39s: different zone (Z%d), not added\n",
				p->area, p->zone);
		    continue;
		}
	    }
	    
	    if(lon_search(l, node))
		fprintf(output, "%-39s: already active\n", p->area);
	    else 
	    {
		lon_add(l, node);
		areas_bbs_changed = TRUE;
		fprintf(output, "%-39s: added\n", p->area);

		log("%s: +%s", node_to_asc(node, TRUE), p->area);
	    }
	}
    }

    if(!match)
    {
	fprintf(output, "%s: no such area, or no area matching pattern\n",
		area);
    }
    
    return OK;
}



/*
 * Remove command
 */
int cmd_remove(Node *node, char *area)
{
    AreasBBS *p;
    LON *l;
    int match = FALSE;

    if(!authorized)
    {
	fprintf(output, "Command REMOVE: not authorized.\n");
	return OK;
    }
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	if(wildmatch(p->area, area, TRUE))
	{
	    match = TRUE;

	    if(!lon_search(l, node))
	    {
		if(!areafix || p->zone==node->zone)
		    fprintf(output, "%-39s: not active\n", p->area);
	    }
	    else 
	    {
		lon_remove(l, node);
		areas_bbs_changed = TRUE;
		fprintf(output, "%-39s: removed\n", p->area);

		log("%s: -%s", node_to_asc(node, TRUE), p->area);
	    }
	}
    }
    
    if(!match)
	fprintf(output, "%s: no such area, or no area matching pattern\n",
		area);
	
    return OK;
}



/*
 * Help command
 */
int cmd_help(Node *node)
{
    fprintf(output, "\n\
Help for %s, FIDOGATE %s  %s %s.\n\n",
	    MY_NAME, version_global(), PROGRAM, version_local(VERSION) );

    fprintf(output, "\n\
Send mail\n\
\n\
    To:      %s@%s\n\
or  To:      %s @ %s\n\
    Subject: PASSWORD\n",
	    MY_NAME, cf_fqdn(), MY_NAME, node_to_asc(cf_addr(), FALSE) );

    fprintf(output, "\n\
Commands in message body:\n\
\n\
    listall                      list all available areas\n\
    list                         list areas available to node\n\
    query                        list subscribed areas\n\
    unlinked                     list unsubscribed areas\n\
    passwd Z:N/F.P PASSWORD      set address and password\n\
    password Z:N/F.P PASSWORD\n\
    pass Z:N/F.P PASSWORD\n\
    add AREA                     subscribe to area\n\
    +AREA\n\
    AREA\n\
    remove AREA                  unsubscribe to area\n\
    delete AREA\n\
    del AREA\n\
    -AREA\n\
    vacation AREA                set vacation flag for area\n\
    create AREA [-options]       create new area\n\
    help                         this help\n\
\n\
AREA names are not case-sensitive and support shell-style wildcards\n\
* ? [a-z] [abc], e.g. COMP.OS.*, [A-D]*.GER\n\n"                       );

    return OK;
}



/*
 * Passwd command
 */
int cmd_passwd(Node *node, char *arg)
{
    char *p;
    Node n;
    Passwd *pwd;
    
    authorized = FALSE;

    p = strtok(arg, " \t");			/* Node address */
    if(!p)
    {
	fprintf(output, "Command PASSWORD: missing Z:N/F.P address.\n");
	return OK;
    }	
    if( asc_to_node(p, &n, FALSE) == ERROR )
    {
	fprintf(output, "Command PASSWORD: illegal address %s.\n", p);
	return OK;
    }
    *node = n;
    cf_set_zone(node->zone);
    
    p = strtok(NULL, " \t");			/* Password */
    if(!p)
    {
	fprintf(output, "Command PASSWORD: no password given!\n");
	authorized = FALSE;
	return OK;
    }

    pwd = passwd_lookup(MY_CONTEXT, node);
    debug(3, "PASSWD entry: %s", pwd ? pwd->passwd : "-NONE-");
    if(!pwd)
    {
	fprintf(output, "Command PASSWORD: address %s not authorized.\n",
		node_to_asc(node, TRUE));
	authorized = FALSE;
	return OK;
    }	
    if(stricmp(p, pwd->passwd))
    {
	fprintf(output, "Command PASSWORD: wrong password for address %s.\n",
		node_to_asc(node, TRUE));
	authorized = FALSE;
	return OK;
    }	

    debug(3, "password check o.k. - authorized");
    authorized = TRUE;

    authorized_lvl  = 1;
    authorized_key  = "";
    authorized_name = "Sysop";
    /* Warning: destroys pwd->args! */
    if( (p = xstrtok(pwd->args, " \t")) )
	authorized_lvl = atoi(p);
    if( (p = xstrtok(NULL, " \t")) )
	authorized_key = strsave(p);
    if( (p = xstrtok(NULL, " \t")) )
	authorized_name = strsave(p);
    
    debug(3, "passwd lvl : %d", authorized_lvl);
    debug(3, "passwd key : %s", authorized_key);
    debug(3, "passwd name: %s", authorized_name);
    
    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] [Z:N/F.P command]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] [Z:N/F.P command]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -m --mail                    process Areafix mail on stdin\n\
         -r --no-reply                don't send reply via mail\n\
         -n --no-rewrite              don't rewrite AREAS.BBS\n\
         -b --areas-bbs NAME          use alternate AREAS.BBS\n\
         -F --filefix                 run as Filefix program (FAREAS.BBS)\n\
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
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    Node node;
    int ret;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "mail",         0, 0, 'm'},
	{ "no-reply",     0, 0, 'r'},
	{ "no-rewrite",   0, 0, 'n'},
        { "areas-bbs",	  1, 0, 'b'},
	{ "filefix",      0, 0, 'F'},

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

#ifdef SIGPIPE
    /* Ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);
#endif

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "mrnb:Fvhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnaf options *****/
	case 'm':
	    m_flag = TRUE;
	    r_flag = TRUE;
	    break;
	case 'r':
	    r_flag = FALSE;
	    break;
	case 'n':
	    n_flag = TRUE;
	    break;
	case 'b':
	    areas_bbs = optarg;
	    break;
	case 'F':
	    my_name     = MY_NAME_FF;
	    my_context  = MY_CONTEXT_FF;
	    my_areasbbs = MY_AREASBBS_FF;
	    areafix     = FALSE;
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
     * Get name of areas.bbs file from config file
     */
    if(!areas_bbs)
	if( (areas_bbs = cf_get_string(my_areasbbs, TRUE)) )
	{
	    debug(8, "config: %s %s", my_areasbbs, areas_bbs);
	}
    if(!areas_bbs)
    {
	fprintf(stderr, "%s: no areas.bbs specified\n", PROGRAM);
	exit(EX_USAGE);
    }
    
    /* Read PASSWD */
    passwd_init();
    /* Read HOSTS */
    hosts_init();
    
    ret = 0;
    
    if(m_flag)
    {
	/*
	 * Process stdin as mail request for Areafix
	 */
	if(lock_program(PROGRAM, WAIT) == ERROR)
	    ret = EX_OSERR;
	else
	{
	    if(areasbbs_init(areas_bbs) == ERROR)
		ret = EX_OSFILE;
	    else
		ret = do_mail();
	    if(ret == 0)
		if( rewrite_areas_bbs() == ERROR )
		    ret = EX_CANTCREAT;
	}
	unlock_program(PROGRAM);
    }
    else
    {
	/*
	 * Process command on command line
	 */
	/* Node */
	if(optind >= argc)
	{
	    fprintf(stderr, "%s: expecting Z:N/F.P node\n", PROGRAM);
	    short_usage();
	}
	if( asc_to_node(argv[optind], &node, FALSE) == ERROR )
	{
	    fprintf(stderr, "%s: invalid node %s\n", PROGRAM, argv[optind]);
	    short_usage();
	}
	optind++;

	/*
	 * Execute command, always authorized if command line
	 */
	authorized = authorized_cmdline = authorized_create = TRUE;
	
	if(areasbbs_init(areas_bbs) == ERROR)
	    exit(EX_OSFILE);

	/* Command is rest of args on command line */
	buffer[0] = 0;
	for(; optind<argc; optind++)
	{
	    BUF_APPEND(buffer, argv[optind]);
	    if(optind < argc-1)
		BUF_APPEND(buffer, " ");
	}
	
	output = stdout;
	if(do_command(&node, buffer) == ERROR)
	    ret = 1;
	if(ret == 0)
	    if( rewrite_areas_bbs() == ERROR )
		ret = EX_CANTCREAT;
    }

    exit(ret);

    /**NOT REACHED**/
    return 1;
}
