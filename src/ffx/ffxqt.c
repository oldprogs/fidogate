/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ffxqt.c,v 4.7 1997/05/10 20:40:37 mj Exp $
 *
 * Process incoming ffx control and data files
 *
 * With full supporting cast of busy files and locking. ;-)
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



#define PROGRAM		"ffxqt"
#define VERSION		"$Revision: 4.7 $"
#define CONFIG		CONFIG_FFX


#define MAXFFXCMD	16



/*
 * Info from ffx control file
 */
typedef struct st_ffx
{
    char *job;			/* Job name */
    char *name;			/* .ffx file name */
    Node from, to;		/* FTN addresses */
    char *passwd;		/* Password */
    char *cmd;			/* Command with args */
    char *in;			/* stdin file */
    char *decompr;		/* Decompressor */
    char *file;
    int  status;		/* Status: TRUE=read # EOF */
} FFX;



/*
 * List of command from config.ffx
 */
typedef struct st_ffxcmd
{
    char type;			/* 'C'=command, 'U'=uncompress */
    char *name;			/* Command name */
    char *cmd;			/* Command to execute */
} FFXCmd;



/*
 * Prototypes
 */
void	parse_ffxcmd		(void);
char   *find_ffxcmd		(int, char *);
int	do_ffx			(int);
void	remove_bad		(char *);
FFX    *parse_ffx		(char *);
int	exec_ffx		(FFX *);

void	short_usage		(void);
void	usage			(void);



/*
 * Command line options
 */
static int g_flag = 0;				/* Processing grade */


/*
 * Commands
 */
static FFXCmd l_cmd[MAXFFXCMD];
static int n_cmd = 0;



/*
 * Parse FFXCommand / FFXUncompress config.ffx parameters
 */
void parse_ffxcmd()
{
    char *p, *name, *cmd;

    /* Commands */
    for(p = cf_get_string("FFXCommand", TRUE);
	p && *p;
	p = cf_get_string("FFXCommand", FALSE) )
    {
	if(n_cmd >= MAXFFXCMD)
	    continue;
	name = xstrtok(p   , "\n\t ");
	cmd  = xstrtok(NULL, "\n");
	if(!name || !cmd)
	    continue;
	while(isspace(*cmd))
	    cmd++;
	
	debug(8, "config: FFXCommand %s %s", name, cmd);

	str_expand_name(buffer, sizeof(buffer), cmd);
	l_cmd[n_cmd].type = 'C';
	l_cmd[n_cmd].name = name;
	l_cmd[n_cmd].cmd  = strsave(buffer);
	n_cmd++;
    }
	  
    /* Uncompressors */
    for(p = cf_get_string("FFXUncompress", TRUE);
	p && *p;
	p = cf_get_string("FFXUncompress", FALSE) )
    {
	if(n_cmd >= MAXFFXCMD)
	    continue;
	name = xstrtok(p   , "\n\t ");
	cmd  = xstrtok(NULL, "\n");
	if(!name || !cmd)
	    continue;
	while(isspace(*cmd))
	    cmd++;
	
	debug(8, "config: FFXUncompress %s %s", name, cmd);

	str_expand_name(buffer, sizeof(buffer), cmd);
	l_cmd[n_cmd].type = 'U';
	l_cmd[n_cmd].name = name;
	l_cmd[n_cmd].cmd  = strsave(buffer);
	n_cmd++;
    }
}



/*
 * Find FFXCommand / FFXUncompress
 */
char *find_ffxcmd(int type, char *name)
{
    int i;

    for(i=0; i<n_cmd; i++)
	if(type==l_cmd[i].type && strieq(name, l_cmd[i].name))
	    return l_cmd[i].cmd;

    return NULL;
}



/*
 * Processs *.ffx control files in Binkley inbound directory
 */
int do_ffx(int t_flag)
{
    FFX *ffx;
    char *name;
    Passwd *pwd;
    char *passwd;
    char buf[MAXPATH];
    char pattern[16];
    
    strncpy0(pattern, "f???????.ffx", sizeof(pattern));
    if(g_flag)
	pattern[1] = g_flag;

    if( chdir(cf_inbound()) == -1 )
    {
	log("$ERROR: can't chdir %s", cf_inbound());
	return ERROR;
    }

    dir_sortmode(DIR_SORTMTIME);
    if(dir_open(".", pattern, TRUE) == ERROR)
    {
	log("$ERROR: can't open directory .");
	return ERROR;
    }
    
    for(name=dir_get(TRUE); name; name=dir_get(FALSE))
    {
	debug(1, "ffxqt: control file %s", name);

	ffx = parse_ffx(name);
	if(!ffx)
	    /* No error, this might be a file still being written to */
	    continue;

	if(!ffx->status)		/* BSY test, if not EOF */
	    if(bink_bsy_test(&ffx->from))	/* Skip if busy */
	    {
		debug(3, "ffxqt: %s busy, skipping",
		      node_to_asc(&ffx->from, TRUE));
		continue;
	    }
	
	/*
	 * Get password for from node
	 */
	if( (pwd = passwd_lookup("ffx", &ffx->from)) )
	    passwd = pwd->passwd;
	else
	    passwd = NULL;
	if(passwd)
	    debug(3, "ffxqt: password %s", passwd);
	
	/*
	 * Require password unless -t option is given
	 */
	if(!t_flag && !passwd)
	{
	    log("%s: no password for %s in PASSWD", name,
		node_to_asc(&ffx->from, TRUE)  );
	    goto rename_to_bad;
	}
	
	/*
	 * Check password
	 */
	if(passwd)
	{
	    if(ffx->passwd)
	    {
		if(stricmp(passwd, ffx->passwd))
		{
		    log("%s: wrong password from %s: ours=%s his=%s",
			name, node_to_asc(&ffx->from, TRUE), passwd,
			ffx->passwd                              );
		    goto rename_to_bad;
		}
	    }
	    else
	    {
		log("%s: no password from %s: ours=%s", name,
		    node_to_asc(&ffx->from, TRUE), passwd );
		goto rename_to_bad;
	    }
	}

	log("job %s: from %s to %s data %s (%ldb) %s / %s",
	    ffx->job,
	    node_to_asc(&ffx->from, TRUE), node_to_asc(&ffx->to, TRUE),
	    ffx->file, check_size(ffx->file),
	    ffx->decompr ? ffx->decompr : "", ffx->cmd                 );
	
	if(exec_ffx(ffx) == ERROR)
	{
	    log("%s: command failed", name);
	rename_to_bad:
	    /*
	     * Error: rename .ffx -> .bad
	     */
	    strcpy(buf, name);
	    strcpy(buf+strlen(buf)-3, "bad");
	    rename(name, buf);
	    log("%s: renamed to %s", name, buf);
	}
	
    }
    
    dir_close();

    return OK;
}



/*
 * Remove "bad" character from string
 */
void remove_bad(char *s)
{
    char *p = s;
    
    while(*p)
	if(*p>=' ' && *p < 127)
	    switch(*p)
	    {
	    case '$':
	    case '&':
	    case '(':
	    case ')':
	    case ';':
	    case '<':
	    case '>':
	    case '^':
	    case '`':
	    case '|':
		p++;		/* skip */
		break;
	    default:
		*s++ = *p++;
		break;
	    }
	else
	    p++;
    
    *s = 0;
}



/*
 * Parse control file and read into memory
 */
#define SEP " \t\r\n"

FFX *parse_ffx(char *name)
{
    FILE *fp;
    static FFX ffx;
    char *buf, *p;
    
    xfree(ffx.job);	ffx.job     = NULL;
    xfree(ffx.name);	ffx.name    = NULL;
    xfree(ffx.passwd);	ffx.passwd  = NULL;
    xfree(ffx.cmd);	ffx.cmd     = NULL;
    xfree(ffx.in);	ffx.in      = NULL;
    xfree(ffx.decompr);	ffx.decompr = NULL;
    xfree(ffx.file);	ffx.file    = NULL;
    ffx.from.zone = ffx.from.net = ffx.from.node = ffx.from.point = 0;
    ffx.to  .zone = ffx.to  .net = ffx.to  .node = ffx.to  .point = 0;
    ffx.status = FALSE;
    ffx.name = strsave(name);

    fp = fopen(ffx.name, R_MODE);
    if(!fp)
    {
	log("$ERROR: can't open %s", ffx.name);
	return NULL;
    }
    
    while((buf = fgets(buffer, BUFFERSIZE, fp)))
    {
	strip_crlf(buf);
	
	switch(*buf)
	{
	case '#':
	    /* Comment */
	    if(!strncmp(buf, "# EOF", 5))
		ffx.status = TRUE;
	    continue;

	case 'J':			/* Job name */
	    ffx.job = strsave(buf + 2);
	    break;
	    
	case 'U':
	    /* User name */
	    p = strtok(buf+2, SEP);
	    /* From node */
	    p = strtok(NULL , SEP);
	    if(p)
		asc_to_node(p, &ffx.from, FALSE);
	    /* To node */
	    p = strtok(NULL , SEP);
	    if(p)
		asc_to_node(p, &ffx.to, FALSE);
	    break;
	    
	case 'I':
	    /* data file */
	    p = strtok(buf+2, SEP);
	    if(p)
	    {
		ffx.in = strsave(p);
		remove_bad(ffx.in);
	    }
	    /* decompressor */
	    p = strtok(NULL , SEP);
	    if(p)
	    {
		ffx.decompr = strsave(p);
		remove_bad(ffx.decompr);
	    }
	    break;
	    
	case 'F':
	    ffx.file = strsave(buf + 2);
	    remove_bad(ffx.file);
	    break;
	    
	case 'C':
	    /* command */
	    ffx.cmd = strsave(buf + 2);
	    remove_bad(ffx.cmd);
	    break;

	case 'P':
	    ffx.passwd = strsave(buf + 2);
	    break;
	    
	}
    }
	
    fclose(fp);

    if(!ffx.cmd)
	return NULL;
    
    debug(3, "ffx: %s", ffx.name);
    debug(3, "     %s -> %s",
	  node_to_asc(&ffx.from, TRUE), node_to_asc(&ffx.to, TRUE));
    debug(3, "     J %s", ffx.job ? ffx.job : "");
    debug(3, "     I %s %s",
	  ffx.in      ? ffx.in      : "",
	  ffx.decompr ? ffx.decompr : "" );
    debug(3, "     F %s", ffx.file);
    debug(3, "     C %s", ffx.cmd);
    debug(3, "     P %s", ffx.passwd ? ffx.passwd : "");
    
    return &ffx;
}



/*
 * Execute command in ffx
 */
int exec_ffx(FFX *ffx)
{
    int ret;
    char *name, *args=NULL, *cmd_c=NULL, *cmd_u=NULL;
    
    /*
     * Extract command name and args
     */
    name = strtok(ffx->cmd, "\n\t ");
    args = strtok(NULL,     "\n"   );
    if(!name)
	return ERROR;
    if(!args)
	args = "";
    while(isspace(*args))
	args++;

    /*
     * Find command and uncompressor
     */
    cmd_c = find_ffxcmd('C', name);
    if(!cmd_c)
    {
	log("no FFXCommand found for \"%s\"", name);
	return ERROR;
    }
    if(ffx->decompr) 
    {
	cmd_u = find_ffxcmd('U', ffx->decompr);
	if(!cmd_u)
	{
	    log("no FFXUncompress found for \"%s\"", ffx->decompr);
	    return ERROR;
	}
    }
    

    /*
     * Execute
     */
    if(ffx->in)
    {
	/* Feed data file as stdin to command, optionally decompressing */
	if(ffx->decompr)		/* Data file with compression */
	    sprintf(buffer, "%s %s | %s %s", cmd_u, ffx->in, cmd_c, args);
	else				/* No compression */
	    sprintf(buffer, "%s %s <%s", cmd_c, args, ffx->in);
    }
    else
    {
	/* Execute command without data file */
	sprintf(buffer, "%s %s", cmd_c, args);
    }

    /*
     * FIXME: should do some proper calls to fork(), exec(), pipe() etc.
     * system() calls /bin/sh which is inefficient and creates security
     * problems.
     */
    debug(2, "Command: %s", buffer);
    ret = run_system(buffer);
    debug(2, "Exit code=%d", ret);

    if(ret == 0)
    {
	unlink(ffx->name);
	if(ffx->file)
	    unlink(ffx->file);
	return OK;
    }
    
    return ERROR;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options]\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -g --grade G                 processing grade\n\
          -I --inbound dir             set inbound directory\n\
          -t --insecure                process ffx files without password\n\
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
    char *I_flag=NULL;
    int   t_flag=FALSE;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "grade",        1, 0, 'g'},	/* grade */
	{ "insecure",     0, 0, 't'},	/* Insecure */
	{ "inbound",      1, 0, 'I'},	/* Set Binkley inbound */

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


    while ((c = getopt_long(argc, argv, "g:tI:vhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ffxqt options *****/
	case 'g':
	    g_flag = *optarg;
	    break;
	case 't':
	    t_flag = TRUE;
	    break;
	case 'I':
	    I_flag = optarg;
	    break;
	    
	/***** Common options *****/
	case 'v':
	    verbose++;
	    break;
	case 'h':
	    usage();
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
     * Process additional config statements
     */
    parse_ffxcmd();

    if(I_flag)
	cf_set_inbound(I_flag);

    passwd_init();

    
    if(lock_program(PROGRAM, NOWAIT) == ERROR)
	exit(EXIT_BUSY);
    
    do_ffx(t_flag);

    unlock_program(PROGRAM);


    exit(0);

    /**NOT REACHED**/
    return 1;
}
