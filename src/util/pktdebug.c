/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: pktdebug.c,v 4.0 1996/04/17 18:17:43 mj Exp $
 *
 * Debug contents of FTN packet
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



#define PROGRAM		"pktdebug"
#define VERSION		"$Revision: 4.0 $"



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] file ...\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] file ...\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -m --msg-header              print message header\n\
          -t --msg-text                print message text\n\
\n\
          -v --verbose                 more verbose\n\
	  -h --help                    this help\n");

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    FILE *fp;
    Packet pkt;
    Message msg;
    Textlist tl;
    int c, type;
    char *name;
    int m_flag=FALSE, t_flag=FALSE;
    MsgBody body;
    int err;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "msg-header",   0, 0, 'm'},
	{ "msg-text",     0, 0, 't'},	    
	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ 0,              0, 0, 0  }
    };

    log_file("stdout");
    log_program(PROGRAM);
    
    tl_init(&tl);
    msg_body_init(&body);
    
    while ((c = getopt_long(argc, argv, "mtvh",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	case 'm':
	    m_flag = TRUE;
	    break;
	case 't':
	    m_flag = TRUE;
	    t_flag = TRUE;
	    break;
	    
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

    /*
     * Process following command line arguments
     */
    if(optind == argc)
	short_usage();
	
    /* Files */
    for(; optind<argc; optind++)
    {
	name = argv[optind];

	if(!strcmp(name, "-"))		/* "-" = stdin */
	    fp = stdin;
	else
	    fp = xfopen(name, R_MODE);

	do
	{
	    if( pkt_get_hdr(fp, &pkt) == ERROR )
	    {
		printf("ERROR: %s: reading packet header\n", name);
		printf("Partially read ");
		pkt_debug_hdr(stdout, &pkt, "");
		break;
	    }

	    if(t_flag)
		fprintf(stdout, "========================================"
			"======================================\n");
	    pkt_debug_hdr(stdout, &pkt, "");
	    if(t_flag)
		fprintf(stdout, "========================================"
			"======================================\n");
	    
	    type = pkt_get_int16(fp);
	    if(type == ERROR)
	    {
		if(feof(fp))
		{
		    printf("WARNING: %s: premature EOF reading input packet\n",
			   name);
		    break;
		}
		printf("ERROR: %s: reading input packet\n", name);
		break;
	    }
	    while(type == MSG_TYPE)
	    {
		msg.node_from = pkt.from;
		msg.node_to   = pkt.to;
		if( pkt_get_msg_hdr(fp, &msg) == ERROR )
		{
		    printf("ERROR: %s: reading message header\n", name);
		    break;
		}
		if(m_flag)
		    pkt_debug_msg_hdr(stdout, &msg, "");
		
		type = pkt_get_body(fp, &tl);
		if(type == ERROR)
		{
		    if(feof(fp))
		    {
			printf("WARNING: %s: premature EOF reading "
			       "input packet\n", name);
		    }
		    else 
		    {
			printf("ERROR: %s: reading input packet\n", name);
			break;
		    }
		}

		if( (err = msg_body_parse(&tl, &body)) != OK)
		    fprintf(stdout, "ERROR: %s: parsing message "
			    "body failed (%d)\n", name, err);
		if(t_flag)
		    msg_body_debug(stdout, &body);
	    }
	}
	while(0);
	    
	if(fp != stdin)
	    fclose(fp);
    }
    
    exit(0);

    /**NOT REACHED**/
    return 1;
}

