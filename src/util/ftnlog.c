/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnlog.c,v 4.6 2000/01/28 22:01:15 mj Exp $
 *
 * Write log message to FIDOGATE log file
 *
 *****************************************************************************
 * Copyright (C) 1990-2000
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



#define PROGRAM 	"ftnlog"
#define VERSION 	"$Revision: 4.6 $"
#define CONFIG		DEFAULT_CONFIG_MAIN



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] message\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] message\n\n", PROGRAM);
    fprintf(stderr, "\
options: -p --program                 program name for log entry\n\
\n\
	 -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config NAME             read config file (\"\" = none)\n");
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    char *p_flag=NULL;
    char *c_flag=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "program",      1, 0, 'l'},	/* log_program() name */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ 0,              0, 0, 0  }
    };

    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "p:vhc:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnlog options *****/
        case 'p':
	    p_flag = optarg;
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
	default:
	    short_usage();
	    exit(EX_USAGE);
	    break;
	}

    /*
     * Read config file
     */
    cf_read_config_file(c_flag ? c_flag : CONFIG);

    cf_debug();


    if(p_flag)
	log_program(p_flag);
    
    for(; optind<argc; optind++)
	log(argv[optind]);
    
    exit(EXIT_OK);

    /**NOT REACHED**/
    return 1;
}
