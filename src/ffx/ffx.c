/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ffx.c,v 4.0 1996/04/17 18:17:40 mj Exp $
 *
 * ffx FIDO-FIDO execution
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

#include <pwd.h>



#define PROGRAM		"ffx"
#define VERSION		"$Revision: 4.0 $"
#define CONFIG		CONFIG_FFX



/*
 * Default compression
 */
/*#define DATA_COMPR	"/usr/local/bin/gzip -c9"*/
#define DATA_COMPR	"/bin/gzip -c9"
#define DATA_EXT	".gz"
#define DATA_DECOMPR	"gunzip"

/*#define DATA_NOCOMPR	"/usr/bin/cat"*/
#define DATA_NOCOMPR	"/bin/cat"
#define DATA_NOEXT	""
#define DATA_NODECOMPR	""

#define DATA_FLAV	"Hold"


/*
 * Prototypes
 */
char   *get_user		(void);
char   *new_job_id		(int);
int	ffx			(Node *, int, char **,
				 char *, char *, char *,
				 char *, int, char *);

void	short_usage		(void);
void	usage			(void);



/*
 * Get current user id
 */
char *get_user(void)
{
    struct passwd *pwd;

    if((pwd = getpwuid(getuid())))
	return pwd->pw_name;
    
    return "root";
}



/*
 * Get new job ID
 */
char *new_job_id(int f)
{
    static char buf[16];
    long seq;

    seq = sequencer(SEQ_FF) % 1000000;		/* max. 6 digits */
    if(!f)
	f = 'f';
  
    sprintf(buf, "f%c%06ld", f, seq);
    return buf;
}



/*
 * Do the remote execution
 */
int ffx(Node *node, int cmdc, char **cmdv, char *cmprprog, char *cmprext, char *cmprdecmpr, char *flav, int grade, char *batch)
{
    int i, ret;
    char *seq;
    char *out;
    char ctrlname[MAXPATH], dataname[MAXPATH];
    FILE *ctrl, *data;
    Passwd *pwd;
    char *password;
    
    for(i=0; i<cmdc; i++)
	if(i == 0)
	    strncpy0(buffer, cmdv[i], BUFFERSIZE);
	else
	{
	    strncat0(buffer, " "    , BUFFERSIZE);
	    strncat0(buffer, cmdv[i], BUFFERSIZE);
	}
    
    debug(1, "ffx: addr=%s", node_to_asc(node, TRUE));
    debug(1, "ffx: cmd=%s", buffer);

    seq = new_job_id(grade);
    cf_set_zone(node->zone);
    
    out = cf_zones_out(node->zone);
    if(!out)
    {
	log("ffx: no configured outbound for zone %d",
		node->zone);
	return EX_DATAERR;
    }

    if(batch)
    {
	sprintf(ctrlname, "%s/%s/%s", cf_outbound(), out, batch);
	if( mkdir(ctrlname, DIR_MODE) == -1 )
	    if(errno != EEXIST)
	    {
		log("$ffx: can't create directory %s", ctrlname);
		return EX_OSERR;
	    }
	
	sprintf(ctrlname, "%s/%s/%s/%s.ffx",
		cf_outbound(), out, batch, seq);
	sprintf(dataname, "%s/%s/%s/%s%s" ,
		cf_outbound(), out, batch, seq, cmprext);
    }
    else
    {
	sprintf(ctrlname, "%s/%s/%s.ffx", cf_outbound(), out, seq);
	sprintf(dataname, "%s/%s/%s%s" , cf_outbound(), out, seq, cmprext);
    }
    
    debug(2, "ffx: job=%s", seq);
    debug(2, "ffx: ctrl=%s", ctrlname);
    debug(2, "ffx: data=%s", dataname);

    /*
     * Get password for node
     */
    pwd = passwd_lookup("ffx", node);
    if(pwd)
    {
	password = pwd->passwd;
	if(password)
	    debug(3, "ffx: password=%s", password);
    }
    else
	password = NULL;
    
    /*
     * Create control file
     */
    ctrl = fopen(ctrlname, W_MODE);
    if(!ctrl)
    {
	log("$ffx: can't create %s", ctrlname);
	return EX_OSERR;
    }
    
    chmod(ctrlname, DATA_MODE);
    fprintf(ctrl, "# ffx %s\n", version_local(VERSION));
    fprintf(ctrl, "U %s %s %s\n",
	    get_user(), node_to_asc(cf_addr(), TRUE), node_to_asc(node, TRUE));
    fprintf(ctrl, "Z\n");
    fprintf(ctrl, "J %s\n", seq);
    fprintf(ctrl, "F %s%s\n", seq, cmprext);
    fprintf(ctrl, "I %s%s %s\n", seq, cmprext, cmprdecmpr);
    fprintf(ctrl, "C %s\n", buffer);
    if(password)
	fprintf(ctrl, "P %s\n", password);
    fprintf(ctrl, "# EOF\n");
    
    fclose(ctrl);

    /*
     * Copy stdin to data file (w/ compression)
     */
    data = fopen(dataname, W_MODE);
    if(data)
    {
	chmod(dataname, DATA_MODE);
	fclose(data);
	
	sprintf(buffer, "%s >%s", cmprprog, dataname);
	debug(2, "Command: %s", buffer);
	ret = (system(buffer) >> 8) & 0xff;
	debug(2, "Exit code=%d", ret);
    }
    else
    {
	log("$ffx: can't open %s", dataname);
	ret = ERROR;
    }
    
    if(ret)
    {
	log("ffx: failed to spool job %s", seq);
	unlink(ctrlname);
	unlink(dataname);
	return EX_OSERR;
    }

    if(!batch)
    {
	/*
	 * Attach to FLO file
	 *
	 * batch==NULL: TRUE, if -b option is not given. FALSE, if -b
	 * option is given. In this case the BSY file will be created
	 * and deleted in main().
	 */
	if(bink_attach(node, '^', dataname, flav, batch==NULL) == ERROR)
	{
	    log("ffx: failed to spool job %s", seq);
	    unlink(ctrlname);
	    unlink(dataname);
	    return EX_OSERR;
	}
	if(bink_attach(node, '^', ctrlname, flav, batch==NULL) == ERROR)
	{
	    log("ffx: failed to spool job %s", seq);
	    unlink(ctrlname);
	    unlink(dataname);
	    return EX_OSERR;
	}
    }

    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] node command\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] node command\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -b --batch-dir DIR           operate in batch mode, using DIR\n\
          -B --binkley DIR             set Binkley-style outbound directory\n\
          -F --flavor FLAV             Hold | Normal | Direct | Crash\n\
          -g --grade G                 Grade [a-z]\n\
          -n --nocompression           don't compress data file\n\
\n\
          -v --verbose                 more verbose\n\
	  -h --help                    this help\n\
          -c --config FILE             read config file (\"\" = none)\n\
	  -L --lib-dir DIR             set lib directory\n\
	  -S --spool-dir DIR           set spool directory\n\
	  -a --addr Z:N/F.P            set FTN address\n\
	  -u --uplink-addr Z:N/F.P     set FTN uplink address\n");

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c, ret;
    char *b_flag=NULL, *B_flag=NULL;
    int   n_flag=FALSE;
    char *F_flag=DATA_FLAV;
    int   g_flag=0;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;

    int option_index;
    static struct option long_options[] =
    {
	{ "batch-dir",    1, 0, 'b'},	/* Set batch mode and batch dir */
	{ "binkley",      1, 0, 'B'},	/* Binkley outbound base dir */
	{ "flavor",       1, 0, 'F'},	/* Outbound flavor */
	{ "grade",        1, 0, 'g'},	/* ffx grade (a-z) */
	{ "nocompression",0, 0, 'n'},	/* Don't compress data */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    Node node;
    char **cmdv;
    int cmdc;

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "b:B:F:g:nvhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	case 'b':
	    b_flag = optarg;
	    n_flag = TRUE;
	    break;
	case 'B':
	    B_flag = optarg;
	    break;
	case 'F':
	    F_flag = optarg;
	    break;
	case 'g':
	    g_flag = *optarg;
	    if(g_flag<'a' || g_flag>'z')
		g_flag = 0;
	    break;
	case 'n':
	    n_flag = TRUE;
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
	case 'a':
	    a_flag = optarg;
	    break;
	case 'u':
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
    if(B_flag)
	cf_set_outbound(B_flag);
    if(L_flag)
	cf_set_libdir(L_flag);
    if(S_flag)
	cf_set_spooldir(S_flag);
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);

    cf_debug();


    /*
     * Node address from command line
     */
    if(optind >= argc)
    {
	log("expecting FTN address");
	short_usage();
    }
    if(asc_to_node(argv[optind], &node, FALSE) == ERROR)
    {
	log("invalid FTN address %s", argv[optind]);
	short_usage();
    }
    optind++;

    /*
     * Remote execution command
     */
    if(optind >= argc)
    {
	log("expecting command");
	short_usage();
    }
    cmdc = argc - optind;
    cmdv = &argv[optind];

    passwd_init();
    
    if(b_flag  &&  bink_bsy_create(&node, WAIT) == ERROR)
	exit(1);
    
    ret = ffx(&node, cmdc, cmdv,
	      n_flag ? DATA_NOCOMPR   : DATA_COMPR  ,
	      n_flag ? DATA_NOEXT     : DATA_EXT    ,
	      n_flag ? DATA_NODECOMPR : DATA_DECOMPR,
	      F_flag, g_flag, b_flag                 );

    if(b_flag)
	bink_bsy_delete(&node);
    
    exit(ret);

    /**NOT REACHED**/
    return 1;
}
