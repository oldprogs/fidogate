/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: ftnaf.c,v 1.2 1998/02/14 17:13:57 mj Exp $
 *
 * Areafix-like AREAS.BBS EchoMail distribution manager. Commands somewhat
 * conforming to FSC-0057.
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

#include <signal.h>



#define PROGRAM		"ftnaf"
#define VERSION		"$Revision: 1.2 $"
#define CONFIG		DEFAULT_CONFIG_MAIN



#define MAILER		"/usr/lib/sendmail -t"


static int r_flag = FALSE;
static int m_flag = FALSE;



/*
 * Prototypes
 */
int	areafix_init		(int);
void	areafix_auth_cmd	(void);
char   *areafix_areasbbs	(void);
void	areafix_set_areasbbs	(char *name);
char   *areafix_name		(void);
int	areafix_do		(Node *node, char *subj, Textlist*, Textlist*);
int	rewrite_areas_bbs	(void);
int	areafix_do_cmd		(Node *, char *, Textlist *);

FILE   *mailer_open		(char *);
int     mailer_close		(FILE *);
int	do_mail			(void);
void	short_usage		(void);
void	usage			(void);



/*
 * Open mailer for sending reply
 */
FILE *mailer_open(char *to)
{
    FILE *fp;
    char *cc;
    
    fp = popen(MAILER, W_MODE);
    if(!fp)
    {
	log("$ERROR: can't open pipe to %s", MAILER);
	return NULL;
    }
    
    fprintf(fp, "From: %s-Daemon@%s (%s Daemon)\n",
	    areafix_name(), cf_fqdn(), areafix_name());
    fprintf(fp, "To: %s\n", to);
    if( (cc = cf_get_string("CCMail", TRUE)) )
	fprintf(fp, "Cc: %s\n", cc);
    fprintf(fp, "Subject: Your %s request\n", areafix_name());
    fprintf(fp, "\n");
    
    return fp;
}



/*
 * Close mailer
 */
int mailer_close(FILE *fp)
{
    return pclose(fp);
}



/*
 * Process request message in stdin
 */
int do_mail(void)
{
    RFCAddr from;
    char *pfrom, *subj, *p;
    Node node, *n;
    FILE *output;
    Textlist tl, out;

    node_invalid(&node);
    tl_init(&tl);
    tl_init(&out);

    /* Read message header and body from stdin */
    header_delete();
    header_read(stdin);
    while(fgets(buffer, BUFFERSIZE, stdin)) {
	strip_crlf(buffer);
	tl_append(&tl, buffer);
    }

    pfrom = header_get("From");
    if(!pfrom)
	return EX_UNAVAILABLE;
    debug(3, "From: %s", pfrom);

    /*
     * Check From / X-FTN-From for FTN address
     */
    n = NULL;
    if((p = header_get("X-FTN-From")))
    {
	debug(3, "X-FTN-From: %s", p);
	if((p = strchr(p, '@')))
	{
	    p++;
	    while(*p && is_space(*p))
		p++;
	    if(asc_to_node(p, &node, FALSE) == OK)
		n = &node;
	}
    }
    else
    {
	from = rfcaddr_from_rfc(pfrom);
	n    = inet_to_ftn(from.addr);
    }
    
    if(n)
    {
	debug(3, "FTN address: %s", node_to_asc(n, TRUE));
	node = *n;
	cf_set_zone(n->zone);
    }
    else
	node_invalid(&node);

    /* Run Areafix */
    subj = header_get("Subject");
    areafix_do(&node, subj, &tl, (r_flag ? NULL : &out) );
    
    /* Send output to mailer */
    if(!r_flag)
    {
	output = mailer_open(pfrom);
	if(!output)
	    return EX_OSERR;
	tl_print_x(&out, output, "\n");
	return mailer_close(output);
    }

    return EX_OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] [Z:N/F.P command]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] [Z:N/F.P command]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -m --mail                    process Areafix mail on stdin\n\
         -r --no-reply                don't send reply via mail\n\
         -n --no-rewrite              don't rewrite AREAS.BBS\n\
         -b --areas-bbs NAME          use alternate AREAS.BBS\n\
         -F --filefix                 run as Filefix program (FAREAS.BBS)\n\
\n\
         -v --verbose                 more verbose\n\
	 -h --help                    this help\n\
         -c --config name             read config file (\"\" = none)\n\
	 -a --addr Z:N/F.P            set FTN address\n\
	 -u --uplink-addr Z:N/F.P     set FTN uplink address\n");

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    int   n_flag=FALSE;
    char *c_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    int areafix=TRUE;
    Node node;
    int ret;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "mail",         0, 0, 'm'},
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

#ifdef SIGPIPE
    /* Ignore SIGPIPE */
    signal(SIGPIPE, SIG_IGN);
#endif

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "mrnb:Fvhc:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnaf options *****/
	case 'm':
	    m_flag = TRUE;
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

    /* Common init */
    areafix_init(areafix);
    /* Read PASSWD */
    passwd_init();
    /* Read HOSTS */
    hosts_init();
    
    ret = 0;
    
    if(m_flag)
    {
	/*
	 * Process stdin as mail request for Areafix
	 */
	if(lock_program(PROGRAM, WAIT) == ERROR)
	    ret = EX_OSERR;
	else
	{
	    if(areasbbs_init(areafix_areasbbs()) == ERROR)
		ret = EX_OSFILE;
	    else
		ret = do_mail();
	    if(ret==0 && !n_flag)
		if( rewrite_areas_bbs() == ERROR )
		    ret = EX_CANTCREAT;
	}
	unlock_program(PROGRAM);
    }
    else
    {
	/*
	 * Process command on command line
	 */
	/* Node */
	if(optind >= argc)
	{
	    fprintf(stderr, "%s: expecting Z:N/F.P node\n", PROGRAM);
	    short_usage();
	}
	if( asc_to_node(argv[optind], &node, FALSE) == ERROR )
	{
	    fprintf(stderr, "%s: invalid node %s\n", PROGRAM, argv[optind]);
	    short_usage();
	}
	optind++;

	/*
	 * Execute command, always authorized if command line
	 */
	areafix_auth_cmd();

	if(areasbbs_init(areafix_areasbbs()) == ERROR)
	    exit(EX_OSFILE);

	/* Command is rest of args on command line */
	buffer[0] = 0;
	for(; optind<argc; optind++)
	{
	    BUF_APPEND(buffer, argv[optind]);
	    if(optind < argc-1)
		BUF_APPEND(buffer, " ");
	}
	
	if(areafix_do_cmd(&node, buffer, NULL) == ERROR)
	    ret = 1;
	if(ret==0 && !n_flag)
	    if( rewrite_areas_bbs() == ERROR )
		ret = EX_CANTCREAT;
    }

    exit(ret);

    /**NOT REACHED**/
    return 1;
}
