/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftnafpkt.c,v 1.1 1998/02/14 17:13:57 mj Exp $
 *
 * Areafix processing FTN packets
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

#include <signal.h>

#include "fidogate.h"
#include "getopt.h"


#define PROGRAM 	"ftnafpkt"
#define VERSION 	"$Revision: 1.1 $"
#define CONFIG		DEFAULT_CONFIG_MAIN



/* Prototypes */

/* areafix.c */
int	areafix_init		(int);
void	areafix_auth_cmd	(void);
char   *areafix_areasbbs	(void);
void	areafix_set_areasbbs	(char *name);
char   *areafix_name		(void);
int	areafix_do		(Node *node, char *subj, Textlist*, Textlist*);
int	rewrite_areas_bbs	(void);
int	areafix_do_cmd		(Node *, char *, Textlist *);


int	do_netmail		(Message *, MsgBody *);
int	do_echomail		(Message *, MsgBody *);
int	do_packet		(FILE *, Packet *);
int	rename_bad		(char *);
int	do_file			(char *);
void	prog_signal		(int);
void	short_usage		(void);
void	usage			(void);



/* Global vars */
static char in_dir[MAXPATH];		/* Input directory */
static int must_exit    = FALSE;	/* Flag for -x operation */
static int severe_error = OK;		/* ERROR: exit after error */
static int signal_exit  = FALSE;	/* Flag: TRUE if signal received */
static int r_flag       = FALSE;	/* -r --no-reply */



/*
 * Process one NetMail message
 */
int do_netmail(Message *msg, MsgBody *body)
{
    Textlist outbody;
    Message outmsg;
    
    tl_init(&outbody);

    /* Run Areafix */
    if(areafix_do(&msg->node_from, msg->subject, &body->body,
		  (r_flag ? NULL : &outbody) )               == ERROR)
	return ERROR;

    if(r_flag)
	return OK;
    
    /* Send reply */
    node_clear(&outmsg.node_from);
    node_clear(&outmsg.node_orig);
    outmsg.node_to   = msg->node_from;
    outmsg.attr      = 0;
    outmsg.cost      = 0;
    outmsg.date      = time(NULL);
    BUF_COPY  (outmsg.name_from, areafix_name());
    BUF_APPEND(outmsg.name_from, " Daemon"     );
    BUF_COPY  (outmsg.name_to  , msg->name_from);
    BUF_COPY  (outmsg.subject  , "Your "       );
    BUF_APPEND(outmsg.subject  , areafix_name());
    BUF_APPEND(outmsg.subject  , " Request"    );
    outmsg.area      = NULL;
    
    return outpkt_netmail(&outmsg, &outbody, PROGRAM);
}



/*
 * Process one EchoMail message
 */
int do_echomail(Message *msg, MsgBody *body)
{
    log("echomail message from %s not processed", znfp(&msg->node_orig));
    
    return OK;
}



/*
 * Read and process FTN packets
 */
int do_packet(FILE *pkt_file, Packet *pkt)
{
    Message msg;			/* Message header */
    Textlist tl;			/* Textlist for message body */
    MsgBody body;			/* Message body of FTN message */
    int type;
    
    /* Initialize */
    tl_init(&tl);
    msg_body_init(&body);


    /* Read packet */
    type = pkt_get_int16(pkt_file);
    if(type == ERROR)
    {
	if(feof(pkt_file))
	{
	    log("WARNING: premature EOF reading input packet");
	    return OK;
	}
	
	log("ERROR: reading input packet");
	return ERROR;
    }

    while(type == MSG_TYPE)
    {
	/* Read message header */
	msg.node_from = pkt->from;
	msg.node_to   = pkt->to;
	if(pkt_get_msg_hdr(pkt_file, &msg) == ERROR)
	{
	    log("ERROR: reading input packet");
	    return ERROR;
	}
	
	/* Read message body */
	type = pkt_get_body(pkt_file, &tl);
	if(type == ERROR)
	{
	    if(feof(pkt_file))
	    {
		log("WARNING: premature EOF reading input packet");
	    }
	    else
	    {
		log("ERROR: reading input packet");
		return ERROR;
	    }
	}
	
	/* Parse message body */
	if( msg_body_parse(&tl, &body) == -2 )
	    log("ERROR: parsing message body");
	/* Retrieve address information from kludges for NetMail */
	if(body.area == NULL)
	{
	    /* Retrieve complete address from kludges */
	    kludge_pt_intl(&body, &msg, TRUE);
	    msg.node_orig = msg.node_from;

	    debug(5, "NetMail: %s -> %s",
		  node_to_asc(&msg.node_from, TRUE),
		  node_to_asc(&msg.node_to  , TRUE) );
	    if(do_netmail(&msg, &body) == ERROR)
		return ERROR;
	}
	else 
	{
	    /* Retrieve address information from * Origin line */
	    if(msg_parse_origin(body.origin, &msg.node_orig) == ERROR)
		/* No * Origin line address, use header */
		msg.node_orig = msg.node_from;

	    debug(5, "EchoMail: %s -> %s",
		  node_to_asc(&msg.node_from, TRUE),
		  node_to_asc(&msg.node_to  , TRUE) );
	    if(do_echomail(&msg, &body) == ERROR)
		return ERROR;
	}

	/*
	 * Exit if signal received
	 */
	if(signal_exit)
	{
	    return severe_error=ERROR;
	}
    } /**while(type == MSG_TYPE)**/

    return OK;
}



/*
 * Rename .pkt -> .bad
 */
int rename_bad(char *name)
{
    char bad[MAXPATH];
    int len;
    
    strncpy0(bad, name, sizeof(bad));
    len = strlen(bad) - 4;
    if(len < 0)
	len = 0;
    strcpy(bad + len, ".bad");
    
    log("ERROR: bad packet renamed to %s", bad);
    if(rename(name, bad) == ERROR)
    {
	log("$ERROR: can't rename %s -> %s", name, bad);
	return ERROR;
    }
    
    return OK;
}



/*
 * Process one packet file
 */
int do_file(char *pkt_name)
{
    Packet pkt;
    FILE *pkt_file;
    long pkt_size;
    
    /* Open packet and read header */
    pkt_file = fopen(pkt_name, R_MODE);
    if(!pkt_file) {
	log("$ERROR: can't open packet %s", pkt_name);
	rename_bad(pkt_name);
	return OK;
    }
    if(pkt_get_hdr(pkt_file, &pkt) == ERROR)
    {
	log("ERROR: reading header from %s", pkt_name);
	fclose(pkt_file);
	rename_bad(pkt_name);
	return OK;
    }
    
    /* Process it */
    pkt_size = check_size(pkt_name);
    log("packet %s (%ldb) from %s for %s", pkt_name, pkt_size,
	node_to_asc(&pkt.from, TRUE), node_to_asc(&pkt.to, TRUE) );
    
    if(do_packet(pkt_file, &pkt) == ERROR) 
    {
	log("ERROR: processing %s", pkt_name);
	fclose(pkt_file);
	rename_bad(pkt_name);
	return severe_error;
    }
    
    fclose(pkt_file);

    if (unlink(pkt_name)) {
	log("$ERROR: can't unlink %s", pkt_name);
	rename_bad(pkt_name);
	return ERROR;
    }

    return OK;
}



/*
 * Signal handler
 */
void prog_signal(int signum)
{
    char *name = "";

    signal_exit = TRUE;
    
    switch(signum)
    {
    case SIGHUP:
	name = " by SIGHUP";  break;
    case SIGINT:
	name = " by SIGINT";  break;
    case SIGQUIT:
	name = " by SIGQUIT"; break;
    default:
	name = "";            break;
    }

    log("KILLED%s: exit forced", name);
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] [packet ...]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] [packet ...]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -I --in-dir DIR              set input packet directory\n\
         -O --out-dir DIR             set output packet directory\n\
         -l --lock-file               create lock file while processing\n\
         -r --no-reply                don't send reply via NetMail\n\
         -n --no-rewrite              don't rewrite AREAS.BBS\n\
         -b --areas-bbs NAME          use alternate AREAS.BBS\n\
         -F --filefix                 run as Filefix program (FAREAS.BBS)\n\
\n\
	 -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config NAME             read config file (\"\" = none)\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");
    
    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c, ret;
    int l_flag = FALSE, n_flag=FALSE;
    char *I_flag=NULL, *O_flag=NULL;
    char *c_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    char *pkt_name;
    int areafix=TRUE;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "in-dir",       1, 0, 'I'},	/* Set inbound packets directory */
	{ "lock-file",    0, 0, 'l'},	/* Create lock file while processing */
	{ "out-dir",      1, 0, 'O'},	/* Set packet directory */
	{ "no-reply",     0, 0, 'r'},
	{ "no-rewrite",   0, 0, 'n'},
        { "areas-bbs",	  1, 0, 'b'},
	{ "filefix",      0, 0, 'F'},

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    /* Log name */
    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();

    /* Parse options */
    while ((c = getopt_long(argc, argv, "O:I:lrnb:Fvhc:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** program options *****/
	case 'I':
	    I_flag = optarg;
	    break;
        case 'l':
	    l_flag = TRUE;
            break;
	case 'O':
	    O_flag = optarg;
	    break;
	case 'r':
	    r_flag = TRUE;
	    break;
	case 'n':
	    n_flag = TRUE;
	    break;
	case 'b':
	    areafix_set_areasbbs(optarg);
	    break;
	case 'F':
	    areafix = FALSE;
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

    /* Read config file */
    cf_read_config_file(c_flag ? c_flag : CONFIG);
    /* Process config option */
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);
    cf_debug();
    
    /* Process local options */
    BUF_EXPAND(in_dir, I_flag ? I_flag : cf_p_pinbound());
    pkt_outdir(O_flag ? O_flag : DEFAULT_OUTPKT, NULL);
    
    /* Install signal/exit handlers */
    signal(SIGHUP,  prog_signal);
    signal(SIGINT,  prog_signal);
    signal(SIGQUIT, prog_signal);

    /* Common init */
    areafix_init(areafix);
    /* Read PASSWD */
    passwd_init();


    /***** Main processing loop *****/
    ret = EXIT_OK;
    
    if(optind >= argc)
    {
	/* process packet files in directory */
	dir_sortmode(DIR_SORTMTIME);
	if(dir_open(in_dir, "*.pkt", TRUE) == ERROR)
	{
	    log("$ERROR: can't open directory %s", in_dir);
	    exit(EXIT_ERROR);
	}
    
	/* Lock file */
	if(l_flag)
	    if(lock_program(PROGRAM, FALSE) == ERROR)
		/* Already busy */
		exit(EXIT_BUSY);

	/* Read areas.bbs */
	if(areasbbs_init(areafix_areasbbs()) == ERROR)
	{
	    if(l_flag)
		unlock_program(PROGRAM);
	    exit(EXIT_ERROR);
	}
	
	for(pkt_name=dir_get(TRUE); pkt_name; pkt_name=dir_get(FALSE))
	{
	    if(do_file(pkt_name) == ERROR)
	    {
		ret = EXIT_ERROR;
		break;
	    }
	    if(must_exit)
	    {
		ret = EXIT_CONTINUE;
		break;
	    }
	}

	dir_close();
	
	/* Rewrite areas.bbs */
	if(ret==EXIT_OK && !n_flag)
	    if(rewrite_areas_bbs() == ERROR)
		ret = EXIT_ERROR;

	/* Lock file */
	if(l_flag)
	    unlock_program(PROGRAM);
    }
    else
    {
	/* Lock file */
	if(l_flag)
	    if(lock_program(PROGRAM, FALSE) == ERROR)
		/* Already busy */
		exit(EXIT_BUSY);

	/* Read areas.bbs */
	if(areasbbs_init(areafix_areasbbs()) == ERROR)
	{
	    if(l_flag)
		unlock_program(PROGRAM);
	    exit(EXIT_ERROR);
	}
	
	/* Process packet files on command line */
	for(; optind<argc; optind++)
	{
	    if(do_file(argv[optind]) == ERROR)
	    {
		ret = EXIT_ERROR;
		break;
	    }
	    if(must_exit)
	    {
		ret = EXIT_CONTINUE;
		break;
	    }
	}

	/* Rewrite areas.bbs */
	if(ret==EXIT_OK && !n_flag)
	    if(rewrite_areas_bbs() == ERROR)
		ret = EXIT_ERROR;

	/* Lock file */
	if(l_flag)
	    unlock_program(PROGRAM);
    }
    

    exit(ret);

    /**NOT REACHED**/
    return 1;
}