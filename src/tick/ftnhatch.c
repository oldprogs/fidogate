/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: ftnhatch.c,v 4.12 2001/05/28 18:00:28 mj Exp $
 *
 * Hatch file into file area
 *
 *****************************************************************************
 * Copyright (C) 1990-2001
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



#define PROGRAM		"ftnhatch"
#define VERSION		"$Revision: 4.12 $"
#define CONFIG		DEFAULT_CONFIG_MAIN



#define CREATOR		"by FIDOGATE/ftnhatch"

#define MY_AREASBBS	"FAreasBBS"    
#define MY_CONTEXT	"ff"



/*
 * Prototypes
 */
int	hatch			(char *, char *, char *);

void	short_usage		(void);
void	usage			(void);



/*
 * Hatch file
 */
int hatch(char *area, char *file, char *desc)
{
    char file_name[MAXPATH];
    AreasBBS *bbs;
    struct stat st;
    unsigned long file_size, file_crc;
    time_t file_time, now;
    Tick tic;
    Node node;
    LNode *p;
    int ret;
    
    /*
     * Lookup area
     */
    if( (bbs = areasbbs_lookup(area)) == NULL )
    {
	/* Unknown area: log it, dump it. ;-) */
	log("ERROR: unknown area %s", area);
	return EXIT_ERROR;
    }
    cf_set_zone(bbs->zone);


    /*
     * Make sure that file exists
     */
    BUF_COPY3(file_name, bbs->dir, "/", file);
    if(stat(file_name, &st) == ERROR)
    {
	log("$ERROR: can't stat() file %s", file_name);
	return EXIT_ERROR;
    }
    file_size = st.st_size;
    file_time = st.st_mtime;
    file_crc  = crc32_file(file_name);
    
    debug(4, "file: name=%s size=%ld time=%ld crc=%08lx",
	  file_name, file_size, file_time, file_crc);

    /*
     * Build Tick struct
     */
    tick_init(&tic);

    now = time(NULL);
    
    tic.origin  = cf_n_addr();
    tic.from    = cf_n_addr();
    /* tic.to set by hatch_one() */
    tic.area    = area;
    tic.file    = file;
    tl_append(&tic.desc, desc);
    tic.crc     = file_crc;
    tic.created = CREATOR;
    tic.size    = file_size;
    tl_appendf(&tic.path, "%s %ld %s",
	       znf1(cf_addr()), now, date(NULL, &now) );
    lon_add(&tic.seenby, cf_addr());
    lon_join(&tic.seenby, &bbs->nodes);
    /* tic.pw set by hatch_one() */
    tic.date    = file_time;

    /*
     * Send to all nodes
     */
    ret = EXIT_OK;
    for(p=bbs->nodes.first; p; p=p->next)
    {
	node = p->node;
	debug(4, "sending to %s", znfp1(&node));
	if(tick_send(&tic, &node, file_name) == ERROR)
	    ret = EXIT_ERROR;
    }
    
    return ret;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] AREA FILE \"DESCRIPTION\"\n",
	    PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] AREA FILE \"DESCRIPTION\"\n\n",
	    PROGRAM);
    fprintf(stderr, "\
options: -b --fareas-bbs NAME         use alternate FAREAS.BBS\n\
\n\
         -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config name             read config file (\"\" = none)\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");

    exit(0);
}



int main(int argc, char **argv)
{
    char *areas_bbs = NULL;
    int c;
    char *c_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    int ret;
    char *area, *file, *desc;
    
    int option_index;
    static struct option long_options[] =
    {
        { "fareas-bbs",	  1, 0, 'b'},

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "b:vhc:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnhatch options *****/
	case 'b':
	    areas_bbs = optarg;
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
    cf_read_config_file(c_flag ? c_flag : CONFIG);

    /*
     * Process config options
     */
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);

    cf_debug();

    /*
     * Command line parameters
     */
    if(argc-optind != 3)
	short_usage();
    area = argv[optind++];
    file = argv[optind++];
    desc = argv[optind++];
    
    /*
     * Get name of fareas.bbs file from config file
     */
    if( !areas_bbs && (areas_bbs = cf_get_string(MY_AREASBBS, TRUE)) )
    {
	debug(8, "config: %s %s", MY_AREASBBS, areas_bbs);
    }
    if(!areas_bbs)
    {
	fprintf(stderr, "%s: no areas.bbs specified\n", PROGRAM);
	exit(EX_USAGE);
    }

    /* Read PASSWD */
    passwd_init();
    /* Read FAreas.BBS */
    if(areasbbs_init(areas_bbs) == ERROR)
    {
	log("$ERROR: can't open %s", areas_bbs);
	ret = EX_OSFILE;
    }
    
    /* Hatch it! */
    ret = hatch(area, file, desc);
    tmps_freeall();

    exit(ret);
}
