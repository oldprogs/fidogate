/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: charsetc.c,v 1.1 1998/03/22 17:57:38 mj Exp $
 *
 * Charset mapping table compiler
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
#include "getopt.h"



#define PROGRAM		"charsetc"
#define VERSION		"$Revision: 1.1 $"



/*->structs.h----------------------------------------------------------------*/
/*
 * Charset mapping
 */
#define MAX_CHARSET_NAME	16
#define MAX_CHARSET_IN		128
#define MAX_CHARSET_OUT		4

struct st_charset_alias
{
    long magic;				/* For binary format */
    char alias[MAX_CHARSET_NAME];	/* Alias charset name */
    char name[MAX_CHARSET_NAME];	/* Real charset name */
    struct st_charset_alias *next;
}
CharsetAlias;

struct st_charset_table
{
    long magic;				/* For binary format */
    char in[MAX_CHARSET_NAME];		/* Input charset name */
    char out[MAX_CHARSET_NAME];		/* Output charset name */
    char map[MAX_CHARSET_OUT][MAX_CHARSET_IN];
    struct st_charset_table *next;
}
CharsetTable;



/*->charset.c----------------------------------------------------------------*/



/*---------------------------------------------------------------------------*/
/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] charset.map charset.bin\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] charset.map charset.bin\n\n",
	    PROGRAM);
    fprintf(stderr, "\
options:  \n\
\n\
          -v --verbose                 verbose\n\
	  -h --help                    this help\n"                     );

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    int ret = EXIT_OK;
    char *name_in, *name_out;
    
    int option_index;
    static struct option long_options[] =
    {

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ 0,              0, 0, 0  }
    };

    
    while ((c = getopt_long(argc, argv, "vh",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	    
	/***** Common options *****/
	case 'v':
	    verbose++;
	    break;
	case 'h':
	    usage();
	    break;
	default:
	    short_usage();
	    break;
	}

    if(optind+2 != argc)
	short_usage();

    name_in  = argv[optind]++;
    name_out = argv[optind]++;
    
    
    
    exit(ret);

    /**NOT REACHED**/
    return 1;
}
