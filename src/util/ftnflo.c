/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnflo.c,v 4.1 1996/06/10 19:36:10 mj Exp $
 *
 * Run script for every entry in FLO file for node
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

#include <fcntl.h>



#define PROGRAM		"ftnflo"
#define VERSION		"$Revision: 4.1 $"
#define CONFIG		CONFIG_MAIN



/*
 * Command for script
 */
static char script[MAXPATH];

/*
 * Command line options
 */
int l_flag = FALSE;
int n_flag = FALSE;
int x_flag = FALSE;



/*
 * Process FLO file of one node
 */
int do_flo(Node *node)
{

    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] Z:N/F.P ...\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] Z:N/F.P ...\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -B --binkley NAME            set Binkley-style outbound directory\n\
          -l --list                    list FLO entries\n\
          -n --no-delete               don't delete/truncate FLO entry\n\
          -x --exec SCRIPT             execute SCRIPT for every FLO entry\n\
\n\
          -v --verbose                 more verbose\n\
	  -h --help                    this help\n\
          -c --config name             read config file (\"\" = none)\n\
	  -L --lib-dir name            set lib directory\n\
	  -S --spool-dir name          set spool directory\n");
    
    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    char *B_flag=NULL;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    Node node;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "binkley",      1, 0, 'B'},	/* Binkley outbound base dir */
	{ "exec",         1, 0, 'x'},	/* Execute script */
	{ "list",         1, 0, 'l'},	/* List entries */
	{ "no-delete",    0, 0, 'n'},	/* Don't delete/truncate entries */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    log_file("stderr");
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "B:x:lnvhc:S:L:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	case 'B':
	    B_flag = optarg;
	    break;
	case 'l':

	    break;
	case 'n':

	    break;
	case 'x':

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
    if(B_flag)
	cf_set_outbound(B_flag);
    if(L_flag)
	cf_set_libdir(L_flag);
    if(S_flag)
	cf_set_spooldir(S_flag);

    cf_debug();

    /*
     * Process following command line arguments
     */
    if(optind >= argc)
	short_usage();

    /* Nodes */
    for(; optind<argc; optind++)
    {
	if(asc_to_node(argv[optind], &node, FALSE) == ERROR)
	{
	    fprintf(stderr, "%s: not an FTN address: %s\n",
		    PROGRAM, argv[optind]);
	    continue;
	}
	do_flo(&node);
    }
    
    exit(0);

    /**NOT REACHED**/
    return 1;
}
