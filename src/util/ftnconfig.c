/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnconfig.c,v 4.2 1996/10/18 16:58:06 mj Exp $
 *
 * Fetch FIDOGATE config.* parameters
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



#define PROGRAM 	"ftnconfig"
#define VERSION 	"$Revision: 4.2 $"
#define CONFIG		CONFIG_MAIN



/*
 * Prototypes
 */
int	do_para			(char *);

void	short_usage		(void);
void	usage			(void);



/*
 * Command line options
 */
static int n_flag = FALSE;		/* -n --no-output */
static int l_flag = FALSE;		/* -l --no-newlines */
static int t_flag = FALSE;		/* -t --test-only */



/*
 * Process parameter name on command line
 */
int do_para(char *name)
{
    int ret = FALSE;
    
    /* List of =name for fixed cf_* functions */
    static struct st_parafunc
    {
	char *name;
	char *(*func)(void);
    }
    fixed[] =
    {
        { "=fqdn",        cf_fqdn        },
        { "=hostname",    cf_hostname    },
        { "=domainname",  cf_domainname  },
        { "=hostsdomain", cf_hostsdomain },
	{ NULL   , NULL     }
    };

    
    /* Fixed parameter */
    if(*name == '=')
    {
	struct st_parafunc *p;

	for(p=fixed; p->name; p++)
	    if(strieq(p->name, name))
	    {
		if(!n_flag)
		    printf("%s%s", p->func(), l_flag ? "" : "\n");
		ret = TRUE;
		break;
	    }
    }
    /* Arbitrary parameter */
    else 
    {
	char *p;

	for(p=cf_get_string(name, TRUE); p; p=cf_get_string(name, FALSE))
	{
	    if(!n_flag)
		printf("%s%s", p, l_flag ? "" : "\n");
	    ret = TRUE;
	}
    }

    if(t_flag)
	printf("%s%s", ret ? "1" : "0", l_flag ? "" : "\n");
    return ret;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] parameter\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] parameter\n\n", PROGRAM);
    fprintf(stderr, "\
options: -l --no-newline              no newline after parameter value\n\
         -n --no-output               no output, exit code only\n\
         -t --test-only               output '1' if present, '0' ifnot\n\
\n\
	 -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config NAME             read config file (\"\" = none)\n\
	 -L --lib-dir NAME            set lib directory\n\
	 -S --spool-dir NAME          set spool directory\n"           );
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    char *c_flag=NULL, *S_flag=NULL, *L_flag=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "no-newline",   0, 0, 'l'},	/* No newline */
	{ "no-output",    0, 0, 'n'},	/* No output */
	{ "test-only",    0, 0, 't'},	/* No output */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ 0,              0, 0, 0  }
    };

    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "lntvhc:S:L:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnconfig options *****/
        case 'l':
	    l_flag = TRUE;
            break;
        case 'n':
	    n_flag = TRUE;
            break;
        case 't':
	    t_flag = TRUE;
	    n_flag = TRUE;
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

    cf_debug();

    if(optind != argc-1) 
    {
	    short_usage();
	    exit(EX_USAGE);
    }
    
    exit( do_para(argv[optind]) ? 0 : 1 );
    
    /**NOT REACHED**/
    return 1;
}
