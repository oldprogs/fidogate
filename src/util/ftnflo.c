/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnflo.c,v 4.2 1996/06/17 19:10:38 mj Exp $
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
#define VERSION		"$Revision: 4.2 $"
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
    int mode, ret, del_ok;
    char *line;
    char buf[MAXPATH];

    del_ok = TRUE;
    
    /* Open FLO file */
    if(flo_open(node, TRUE) == ERROR)
    {
	log("nothing on hold for %s", znfp(node));
	return OK;
    }

    /* Read FLO entries */
    while( (line = flo_gets(buffer, sizeof(buffer))) )
    {
	if(*line == '~')
	    continue;
	mode = ' ';
	if(*line == '^' || *line == '#')
	    mode = *line++;
	if(cf_dos())
	    line = cf_unix_xlate(line);

	debug(2, "FLO entry: %c %s", mode, line);

	if(l_flag)
	{
	    printf("%10ld    %c %s\n", check_size(line), mode, line);
	}
	if(x_flag)
	{
	    /* Command */
	    sprintf(buf, script, line);
	    debug(2, "Command: %s", buf);

	    if(!n_flag)
	    {
		ret = (system(buf) >> 8) & 0xff;
		debug(2, "Exit code=%d", ret);
		if(ret)
		{
		    log("ERROR: running command %s", buf);
		    flo_close(node, TRUE, FALSE);
		    return ERROR;
		}

		/* According to mode ... */
		switch(mode)
		{
		case '^':
		    /* ... delete */
		    debug(2, "Removing %s", line);
		    if(unlink(line) == ERROR)
			log("ERROR: can't remove %s", line);
		    break;

		case '#':
		    /* ... truncate */
		    debug(2, "Truncating %s", line);
		    if(truncate(line, 0) == ERROR)
			log("ERROR: can't truncate %s", line);
		    break;
		}

		/* Mark as sent */
		flo_mark();
	    }
	}
	else
	    del_ok = FALSE;
    }

    /* Close and delete if completed FLO file */
    flo_close(node, TRUE, del_ok);

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
	    l_flag = TRUE;
	    break;
	case 'n':
	    n_flag = TRUE;
	    break;
	case 'x':
	    x_flag = TRUE;
	    BUF_COPY(script, optarg);
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

    /* Default: -l */
    if(!l_flag && !x_flag)
	l_flag = TRUE;
    
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