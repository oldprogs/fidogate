/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnin.c,v 4.3 1997/02/09 10:40:24 mj Exp $
 *
 * Search for mail packets destined to gateway's FTN addresses and feed
 * them to ftn2rfc.
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



#define PROGRAM		"ftnin"
#define VERSION		"$Revision: 4.3 $"
#define CONFIG		CONFIG_GATE



#define FTN2RFC		"ftn2rfc"



/*
 * Prototypes
 */
void	args_add		(char *);
int	do_packets		(void);
int	exec_ftn2rfc		(char *);

void	short_usage		(void);
void	usage			(void);



/*
 * List of addresses we're processing packets for
 */
Node *addr[2 * MAXADDRESS];
int naddr = 0;


/*
 * Command and args ftn2rfc
 */
static char cmd[MAXPATH];
static char args[MAXPATH];

/*
 * Command for script
 */
static char script[MAXPATH];

/*
 * Command line options
 */
int n_flag = FALSE;
int x_flag = FALSE;



/*
 * Add string to args[]
 */
void args_add(char *s)
{
    strncat0(args, s, sizeof(args));
}



/*
 * Process packets for all adresses
 */
int do_packets(void)
{
    char *name;
    Node *node;
    
    /*
     * If -n option not given, call ftn2rfc for each packet
     */
    if(!n_flag)
	/** Traverse addresses **/
	for(node=cf_addr_trav(TRUE); node; node=cf_addr_trav(FALSE))
	{
	    debug(5, "node=%s", node_to_asc(node, TRUE));
	    if((name = bink_find_out(node, NULL)))
		if(exec_ftn2rfc(name) == ERROR)
		    return ERROR;
	}
    
    /*
     * If -x option given, call command in script[]
     */
    if(x_flag)
    {
	int ret;

	debug(1, "Command: %s", script);
	ret = (system(script) >> 8) & 0xff;
	debug(1, "Exit code=%d", ret);
	if(ret)
	{
	    log("ERROR: can't exec command %s", script);
	    return ERROR;
	}
    }
	
    return OK;
}



/*
 * Call ftn2rfc with name of packet file
 */
int exec_ftn2rfc(char *name)
{
    int ret;
    
    debug(1, "Packet: %s", name);
    
    strncpy0(buffer, cmd , BUFFERSIZE);
    strncat0(buffer, args, BUFFERSIZE);
    strncat0(buffer, " " , BUFFERSIZE);
    strncat0(buffer, name, BUFFERSIZE);
    debug(1, "Command: %s", buffer);

    ret = (system(buffer) >> 8) & 0xff;
    debug(1, "Exit code=%d", ret);
    if(ret)
    {
	log("ERROR: can't exec command %s", buffer);
	return ERROR;
    }
    
    return OK;
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
options:  -n --no-toss                 don't call ftn2rfc for tossing\n\
          -x --exec-program SCRIPT     call SCRIPT after tossing\n\
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
    char *exec=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "no-toss",      0, 0, 'n'},	/* Don't call ftn2rfc */
	{ "exec-program", 1, 0, 'x'},	/* Exec script after tossing */

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
    log_file("stderr");
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "nx:vhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** Local options *****/
	case 'n':
	    n_flag = TRUE;
	    break;
	case 'x':
	    exec   = optarg;
	    x_flag = TRUE;
	    break;
	    
	/***** Common options *****/
	case 'v':
	    args_add(" -v");
	    verbose++;
	    break;
	case 'h':
	    usage();
	    break;
	case 'c':
	    args_add(" -c ");
	    args_add(optarg);
	    c_flag = optarg;
	    break;
	case 'S':
	    args_add(" -S ");
	    args_add(optarg);
	    S_flag = optarg;
	    break;
	case 'L':
	    args_add(" -L ");
	    args_add(optarg);
	    L_flag = optarg;
	    break;
	case 'a':
	    args_add(" -a ");
	    args_add(optarg);
	    a_flag = optarg;
	    break;
	case 'u':
	    args_add(" -u ");
	    args_add(optarg);
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

#if defined(OS2) || defined(MSDOS)
    BUF_COPY3(cmd, cf_libdir(), "\\", FTN2RFC);
#else /**UNIX**/
    BUF_COPY3(cmd, cf_libdir(), "/", FTN2RFC);
#endif
    if(exec)
	str_expand_name(script, sizeof(script), exec);
    
    /* Create busy files, if o.k., process packets */
    if(bink_bsy_create_all(NOWAIT) == OK)
	do_packets();
    /* Delete busy files */
    bink_bsy_delete_all();

    exit(0);

    /**NOT REACHED**/
    return 1;
}
