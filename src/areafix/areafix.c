/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: areafix.c,v 1.3 1998/02/19 16:15:47 mj Exp $
 *
 * Common Areafix functions
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



#define MY_NAME_AF	"Areafix"
#define MY_CONTEXT_AF	"af"
#define MY_AREASBBS_AF	"AreasBBS"

#define MY_NAME_FF	"Filefix"
#define MY_CONTEXT_FF	"ff"
#define MY_AREASBBS_FF	"FAreasBBS"

#define MY_NAME		my_name
#define MY_CONTEXT	my_context



int	areafix_tlprintf	(const char *fmt, ...);
int	areafix_stdprintf	(const char *fmt, ...);

int	is_wildcard		(char *);
int	rewrite_areas_bbs	(void);
int	areafix_do_cmd		(Node *, char *, Textlist *);
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



/*
 * Number of old AREAS.BBS to keep as AREAS.Onn
 */
#define N_HISTORY	5



/*
 * Global vars
 */

/* Areafix (TRUE) / Filefix (FALSE) mode */
static int areafix              = TRUE;

/* Program name, context, config areas.bbs name */
static char *my_name	        = MY_NAME_AF;
static char *my_context         = MY_CONTEXT_AF;
static char *my_areasbbs        = MY_AREASBBS_AF;

/* Name of areas.bbs file */
static char *areas_bbs          = NULL;
static int   areas_bbs_changed  = FALSE;

static int   authorized     	= FALSE;
static int   authorized_lvl 	= 1;
static char *authorized_key 	= "";
static char *authorized_name    = "Sysop";
static Node  authorized_node    = { -1, -1, -1, -1, "" };
static int   authorized_cmdline = FALSE;
static int   authorized_create  = FALSE;


/*
 * Output functions
 */
typedef int (*OFuncP)(const char *, ...);

static OFuncP    areafix_printf = areafix_stdprintf;
static Textlist *areafix_otl    = NULL;


int areafix_tlprintf(const char *fmt, ...)
{
    va_list args;
    static char buf[4096];
    va_start(args, fmt);
    
#ifdef HAS_SNPRINTF    
    vsnprintf(buf, sizeof(buf), fmt, args);
#else
    vsprintf(buf, fmt, args);
#endif
    tl_append(areafix_otl, buf);

    va_end(args);

    return OK;
}


int areafix_stdprintf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    
    vprintf(fmt, args);
    printf("\n");
    
    va_end(args);

    return OK;
}



/*
 * Common Areafix init
 */
int areafix_init(int mode)
{
    areafix = mode;

    if(mode)
    {
	/* Areafix */
	my_name     = MY_NAME_AF;
	my_context  = MY_CONTEXT_AF;
	my_areasbbs = MY_AREASBBS_AF;
    }
    else
    {
	/* Filefix */
	my_name     = MY_NAME_FF;
	my_context  = MY_CONTEXT_FF;
	my_areasbbs = MY_AREASBBS_FF;
    }
    
    /* Get name of areas.bbs file from config file */
    if(!areas_bbs)
	if( (areas_bbs = cf_get_string(my_areasbbs, TRUE)) )
	{
	    debug(8, "config: %s %s", my_areasbbs, areas_bbs);
	}
    if(!areas_bbs)
    {
	fprintf(stderr, "%s: no areas.bbs specified\n", my_name);
	exit(EX_USAGE);
    }

    return OK;
}



/*
 * Get/set name of areas.bbs file
 */
char *areafix_areasbbs(void)
{
    return areas_bbs;
}

void areafix_set_areasbbs(char *name)
{
    areas_bbs = name;
}



/*
 * Authorize for ftnafcmd
 */
void areafix_auth_cmd(void)
{
    authorized = authorized_cmdline = authorized_create = TRUE;
}



/*
 * Areafix name
 */
char *areafix_name(void)
{
    return my_name;
}



/*
 * Return authorized node
 */
Node *areafix_auth_node(void)
{
    return &authorized_node;
}



/*
 * Process Areafix command from stdin
 */
int areafix_do(Node *node, char *subj, Textlist *tl, Textlist *out)
{
    char *passwd;
    Passwd *pwd;
    char *p, *q;
    int q_flag=FALSE, l_flag=FALSE;
    Textline *tp;

    /* Reset */
    authorized = authorized_cmdline = authorized_create = FALSE;
    authorized_lvl = 1;
    authorized_key = "";
    node_invalid(&authorized_node);
    
    /* Check password in Subject and process options */
    passwd = strtok(subj, " \t");
    debug(3, "Subject passwd: %s", passwd);
    pwd    = passwd_lookup(MY_CONTEXT, node);
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
	authorized_node = *node;
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

    /* Execute commands for subject options */
    if(q_flag)
	cmd_query(node);
    if(l_flag)
	cmd_list(node);
    
    /* Execute commands from stdin */
    for(tp=tl->first; tp; tp=tp->next)
    {
	p = tp->line;
	
	strip_crlf(p);				/* Strip CR/LF */
	strip_space(p);				/* Strip spaces */
	if(strneq(p, " * ", 3))			/* Skip " * blah" lines */
	    continue;
	if(strneq(p, "---", 3))			/* Ignore cmds after --- */
	    break;
	if(strneq(p, "--", 2))			/* Ignore cmds after --  */
	    break;				/* (signature start)     */
	if(strneq(p, "--=20", 5))		/* dito, MIME            */
	    break;
	for(; *p && is_space(*p); p++) ;	/* Skip white space */
	if(!*p)					/* Skip empty lines */
	    continue;

	areafix_do_cmd(node, p, out);
    }


    return OK;
}



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
 * Rewrite AREAS.BBS if changed
 */
int rewrite_areas_bbs(void)
{
    char old[MAXPATH], new[MAXPATH];
    int i, ovwr;
    FILE *fp;
    
    if(!areas_bbs_changed)
    {
	debug(4, "AREAS.BBS not changed");
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
    debug(4, "Writing %s", new);

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

    /*
     * Renumber saved AREAS.Onn
     */
    strcpy(old, buffer);
    sprintf(old+ovwr, "o%02d", N_HISTORY);
    debug(4, "Removing %s", old);
    unlink(old);
    for(i=N_HISTORY-1; i>=1; i--)
    {
	strcpy(old, buffer);
	sprintf(old+ovwr, "o%02d", i);
	strcpy(new, buffer);
	sprintf(new+ovwr, "o%02d", i+1);
	debug(4, "Renaming %s -> %s", old, new);
	rename(old, new);
    }
    
    /*
     * Rename AREAS.BBS -> AREAS.O01
     */
    strcpy(old, buffer);
    strcpy(old+ovwr, "bbs");
    strcpy(new, buffer);
    strcpy(new+ovwr, "o01");
    debug(4, "Renaming %s -> %s", old, new);
    rename(old, new);
    
    /*
     * Rename AREAS.NEW -> AREAS.BBS
     */
    strcpy(old, buffer);
    strcpy(old+ovwr, "new");
    strcpy(new, buffer);
    strcpy(new+ovwr, "bbs");
    debug(4, "Renaming %s -> %s", old, new);
    rename(old, new);

    log("%s changed", buffer);

    return OK;
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

int areafix_do_cmd(Node *node, char *line, Textlist *out)
{
    int cmd;
    char *arg;
    char buf[32];
    int i, ret;
    int percent = FALSE;

    /* Output */
    if(out) 
    {
	debug(3, "output via textlist");
	areafix_otl    = out;
	areafix_printf = areafix_tlprintf;
    }
    else
    {
	debug(3, "output via stdout");
	areafix_otl    = NULL;
	areafix_printf = areafix_stdprintf;
    }

    debug(2, "node=%s command=%s", znfp(node), line);

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
		areafix_printf("Unknown command %%%s", buf);
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

    debug(2, "cmd=%d node=%s arg=%s", cmd, znfp(node), arg);

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
	areafix_printf("Command CREATE: not authorized.");
	return OK;
    }

    name = xstrtok(line, " \t");

    if( (p = areasbbs_lookup(name)) )
    {
	areafix_printf("%s: area already exists, can't create.",
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
    log("%s: vacation", znfp(node));

    if(!authorized)
    {
	areafix_printf("Command VACATION: not authorized.");
	return OK;
    }

    areafix_printf("Command VACATION: sorry, not yet implemented.");

    return OK;
}



/*
 * ListAll command
 */
int cmd_listall(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    log("%s: listall", znfp(node));

    if(!authorized)
    {
	areafix_printf("Command LISTALL: not authorized.");
	return OK;
    }
    
    areafix_printf("");
    areafix_printf("ALL available areas:");
    areafix_printf("");
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	char *mark;

	l = &p->nodes;
	mark = (lon_search(l, node) ? "*" : " ");

	if(p->desc)
	    areafix_printf("%s Z%-3d %-39s: %s",
			   mark, p->zone, p->area, p->desc);
	else
	    areafix_printf("%s Z%-3d %s",
			   mark, p->zone, p->area);
    }
    
    areafix_printf("");
    areafix_printf("* = linked to this area");
    areafix_printf("");

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
    
    log("%s: list", znfp(node));

    if(!authorized)
    {
	areafix_printf("Command LIST: not authorized.");
	return OK;
    }
    
    areafix_printf("");
    areafix_printf("Areas available to %s:", node_to_asc(node, FALSE));
    areafix_printf("");
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	char *mark;

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
	
	mark = (lon_search(l, node) ? "*" : " ");
	if(p->desc)
	    areafix_printf("%s %-39s: %s", mark, p->area, p->desc);
	else
	    areafix_printf("%s %s", mark, p->area);
    }
    
    areafix_printf("");
    areafix_printf("* = linked to this area");
    areafix_printf("");
    
    return OK;
}



/*
 * Query command
 */
int cmd_query(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    log("%s: query", znfp(node));

    if(!authorized)
    {
	areafix_printf("Command QUERY: not authorized.");
	return OK;
    }
    
    areafix_printf("");
    areafix_printf("%s is linked to the following areas:", znfp(node));
    areafix_printf("");
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	if(lon_search(l, node))
	    areafix_printf("  %s", p->area);
    }
    
    areafix_printf("");
    
    return OK;
}



/*
 * Unlinked command
 */
int cmd_unlinked(Node *node)
{
    AreasBBS *p;
    LON *l;
    
    log("%s: unlinked", znfp(node));

    if(!authorized)
    {
	areafix_printf("Command UNLINKED: not authorized.");
	return OK;
    }
    
    areafix_printf("");
    areafix_printf("%s is not linked to the following available areas:",
		   znfp(node));
    areafix_printf("");
    
    for(p=areasbbs_first(); p; p=p->next)
    {
	l = &p->nodes;

	/* Check permissions */

	/* Check zone */
	if(areafix && p->zone!=node->zone)
	    continue;
	
	if(! lon_search(l, node))
	    areafix_printf("  %s", p->area);
    }
    
    areafix_printf("");

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
	areafix_printf("Command ADD: not authorized.");
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
			areafix_printf("%-41s: access denied (level)",
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
			    areafix_printf("%-41s: access denied (key)",
					   p->area);
			continue;
		    }
		}

		/* Check zone */
		if(areafix && p->zone!=node->zone)
		{
		    if(!iswc)
			areafix_printf("%-41s: different zone (Z%d), not added",
				       p->area, p->zone);
		    continue;
		}
	    }
	    
	    if(lon_search(l, node))
		areafix_printf("%-41s: already active", p->area);
	    else 
	    {
		lon_add(l, node);
		areas_bbs_changed = TRUE;
		areafix_printf("%-41s: added", p->area);

		log("%s: +%s", node_to_asc(node, TRUE), p->area);
	    }
	}
    }

    if(!match)
    {
	areafix_printf("%s: no such area, or no area matching pattern",
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
	areafix_printf("Command REMOVE: not authorized.");
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
		    areafix_printf("%-41s: not active", p->area);
	    }
	    else 
	    {
		lon_remove(l, node);
		areas_bbs_changed = TRUE;
		areafix_printf("%-41s: removed", p->area);

		log("%s: -%s", node_to_asc(node, TRUE), p->area);
	    }
	}
    }
    
    if(!match)
	areafix_printf("%s: no such area, or no area matching pattern",
		       area);
	
    return OK;
}



/*
 * Help command
 */
int cmd_help(Node *node)
{
    log("%s: help", znfp(node));

    areafix_printf("");
    areafix_printf("Help for %s, FIDOGATE %s", MY_NAME, version_global());
    areafix_printf("");
    areafix_printf("Send mail");
    areafix_printf("");
    areafix_printf("  To:      %s @ %s", MY_NAME, znfp(cf_addr()));
    areafix_printf("  Subject: PASSWORD");
    areafix_printf("");
    areafix_printf("Commands in message body:");
    areafix_printf("");
    areafix_printf("  listall                  list all available areas");
    areafix_printf("  list                     list areas available to node");
    areafix_printf("  query                    list subscribed areas");
    areafix_printf("  unlinked                 list unsubscribed areas");
    areafix_printf("  passwd Z:N/F.P PASSWORD  set address and password");
    areafix_printf("  password Z:N/F.P PASSWORD");
    areafix_printf("  pass Z:N/F.P PASSWORD");
    areafix_printf("  add AREA                 subscribe to area");
    areafix_printf("  +AREA");
    areafix_printf("  AREA");
    areafix_printf("  remove AREA              unsubscribe to area");
    areafix_printf("  delete AREA");
    areafix_printf("  del AREA");
    areafix_printf("  -AREA");
    areafix_printf("  vacation AREA            set vacation flag for area");
    areafix_printf("  create AREA [-options]   create new area");
    areafix_printf("  help                     this help");
    areafix_printf("");
    areafix_printf("AREA names are not case-sensitive "
		   "and support shell-style wildcards");
    areafix_printf("  * ? [a-z] [abc], e.g. COMP.OS.*, [A-D]*.GER");

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
    
    log("%s: passwd", znfp(node));

    authorized = FALSE;

    p = strtok(arg, " \t");			/* Node address */
    if(!p)
    {
	areafix_printf("Command PASSWORD: missing Z:N/F.P address.");
	return OK;
    }	
    if( asc_to_node(p, &n, FALSE) == ERROR )
    {
	areafix_printf("Command PASSWORD: illegal address %s.", p);
	return OK;
    }
    *node = n;
    cf_set_zone(node->zone);
    
    p = strtok(NULL, " \t");			/* Password */
    if(!p)
    {
	areafix_printf("Command PASSWORD: no password given!");
	authorized = FALSE;
	return OK;
    }

    pwd = passwd_lookup(MY_CONTEXT, node);
    debug(3, "PASSWD entry: %s", pwd ? pwd->passwd : "-NONE-");
    if(!pwd)
    {
	areafix_printf("Command PASSWORD: address %s not authorized.",
		node_to_asc(node, TRUE));
	authorized = FALSE;
	return OK;
    }	
    if(stricmp(p, pwd->passwd))
    {
	areafix_printf("Command PASSWORD: wrong password for address %s.",
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
