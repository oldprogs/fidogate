/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ffxbatch.c,v 4.7 1997/04/18 15:37:43 mj Exp $
 *
 * ffx FIDO-FIDO execution batcher, packs batched (-b) ffx jobs.
 *
 *****************************************************************************
 * Copyright (C) 1990-1997
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

#include <pwd.h>



#define PROGRAM		"ffxbatch"
#define VERSION		"$Revision: 4.7 $"
#define CONFIG		CONFIG_FFX



/*
 * Config parameters
 */
static char *arc_prog, *arc_prog_v, *arc_ext, *arc_cmd;
static char *data_flav;


#define ARC_PROG	arc_prog
#define ARC_PROG_V	arc_prog_v
#define ARC_EXT		arc_ext
#define ARC_UNPACK	arc_cmd
			
#define DATA_FLAV	data_flav



/*
 * Prototypes
 */
char   *get_user		(void);
char   *new_job_id		(int);
int	pack_batch		(char *, char *);
int	ffx			(Node *, char *, char *, char *,
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
 * Archive spooled ffx files to batch
 */
int pack_batch(char *dir, char *arc)
{
    int ret;
    
    sprintf(buffer, verbose ? ARC_PROG_V : ARC_PROG, arc, dir);
    debug(2, "Command: %s", buffer);
    ret = run_system(buffer);
    debug(2, "Exit code=%d", ret);
    if(ret)
    {
	unlink(arc);
	return ret;
    }

    sprintf(buffer, "rm -f %s/*", dir);
    debug(2, "Command: %s", buffer);
    ret = run_system(buffer);
    debug(2, "Exit code=%d", ret);
    
    return ret ? ret : OK;
}



/*
 * Do the remote execution
 */
int ffx(Node *node, char *cmprprog, char *cmprext, char *cmprdecmpr,
	char *flav, int grade, char *batch)
{
    char *seq;
    char *out;
    char ctrlname[MAXPATH], dataname[MAXPATH];
    char batchdir[MAXPATH];
    FILE *ctrl;
    Passwd *pwd;
    char *password;
    
    seq = new_job_id(grade);
    cf_set_zone(node->zone);
    
    out = cf_zones_out(node->zone);
    if(!out)
    {
	log("ffx: no configured outbound for zone %d",
		node->zone);
	return(EX_DATAERR);
    }
    
    sprintf(ctrlname, "%s/%s/%s.ffx", cf_outbound(), out, seq);
    sprintf(dataname, "%s/%s/%s%s"  , cf_outbound(), out, seq, cmprext);
    sprintf(batchdir, "%s/%s/%s"    , cf_outbound(), out, batch);

    debug(2, "ffx: job=%s", seq);
    debug(2, "ffx: ctrl=%s", ctrlname);
    debug(2, "ffx: data=%s", dataname);
    debug(2, "ffx: batch=%s", batchdir);
    
    /*
     * Pack ffx jobs in batch dir
     */
    if( pack_batch(batchdir, dataname) != OK )
	/* Error or no files to archive ... */
	return EX_OSERR;
    chmod(dataname, DATA_MODE);

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

    /* Log it */
    log("job %s: to %s / %s %s%s",
	seq, node_to_asc(node, TRUE), cmprdecmpr, seq, cmprext);

    /*
     * Create control file
     */
    ctrl = fopen(ctrlname, W_MODE);
    if(!ctrl)
    {
	log("ffxbatch: can't open %s", ctrlname);
	return EX_OSERR;
    }
    chmod(ctrlname, DATA_MODE);
    fprintf(ctrl, "# ffx %s\n", version_local(VERSION));
    fprintf(ctrl, "U %s %s %s\n",
	    get_user(), node_to_asc(cf_addr(), TRUE), node_to_asc(node, TRUE));
    fprintf(ctrl, "Z\n");
    fprintf(ctrl, "J %s\n", seq);
    fprintf(ctrl, "F %s%s\n", seq, cmprext);
    fprintf(ctrl, "C %s %s%s\n", cmprdecmpr, seq, cmprext );
    if(password)
	fprintf(ctrl, "P %s\n", password);
    fprintf(ctrl, "# EOF\n");
    
    fclose(ctrl);

    /*
     * Attach to FLO file
     */
    if(bink_attach(node, '^', dataname, flav, FALSE) == ERROR)
    {
	log("ffx: failed to spool job %s", seq);
	return EX_OSERR;
    }
    if(bink_attach(node, '^', ctrlname, flav, FALSE) == ERROR)
    {
	log("ffx: failed to spool job %s", seq);
	return EX_OSERR;
    }

    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] node\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] node command\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -b --batch-dir DIR           set batch directory\n\
          -B --binkley DIR             set Binkley-style outbound directory\n\
          -F --flavor FLAV             Hold | Normal | Direct | Crash\n\
          -g --grade G                 Grade [a-z]\n\
          -w --wait-bsy                Wait if bsy file exists\n\
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
    char *p;
    char *b_flag="batch", *B_flag=NULL;
    char *F_flag=NULL;
    int   g_flag=0;
    int   w_flag=FALSE;
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
	{ "wait-bsy",     0, 0, 'w'},	/* Wait if bsy file exists */

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


    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "b:B:F:g:wvhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	case 'b':
	    b_flag = optarg;
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
	case 'w':
	    w_flag = TRUE;
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
     * Process additional config statements
     */
    if((p = cf_get_string("FFXBatchArc", TRUE)))
    {
	debug(8, "config: FFXBatchArc %s", p);
	arc_prog = p;
    }
    else
    {
	log("ERROR: %s: FFXBatchArc definition missing", CONFIG);
	exit(EXIT_ERROR);
    }
    if((p = cf_get_string("FFXBatchArcV", TRUE)))
    {
	debug(8, "config: FFXBatchArcV %s", p);
	arc_prog_v = p;
    }
    else
    {
	log("ERROR: %s: FFXBatchArcV definition missing", CONFIG);
	exit(EXIT_ERROR);
    }
    if((p = cf_get_string("FFXBatchArcExt", TRUE)))
    {
	debug(8, "config: FFXBatchArcExt %s", p);
	arc_ext = p;
    }
    else
    {
	log("ERROR: %s: FFXBatchArcExt definition missing", CONFIG);
	exit(EXIT_ERROR);
    }
    if((p = cf_get_string("FFXBatchArcCmd", TRUE)))
    {
	debug(8, "config: FFXBatchArcCmd %s", p);
	arc_cmd = p;
    }
    else
    {
	log("ERROR: %s: FFXBatchArcCmd definition missing", CONFIG);
	exit(EXIT_ERROR);
    }
    if((p = cf_get_string("FFXDataFlav", TRUE)))
    {
	debug(8, "config: FFXDataFlav %s", p);
	data_flav = p;
    }
    else
    {
	log("ERROR: %s: FFXDataFlav definition missing", CONFIG);
	exit(EXIT_ERROR);
    }

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
	log("invalid FTN address %s\n", argv[optind]);
	short_usage();
    }
    optind++;

    passwd_init();
    
    if(bink_bsy_create(&node, w_flag ? WAIT : NOWAIT) == ERROR)
	exit(EXIT_BUSY);

    ret = ffx(&node, ARC_PROG, ARC_EXT, ARC_UNPACK,
	      F_flag ? F_flag : DATA_FLAV, g_flag, b_flag );

    bink_bsy_delete(&node);
    
    exit(ret);

    /**NOT REACHED**/
    return EXIT_ERROR;
}
