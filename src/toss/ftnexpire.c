/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: ftnexpire.c,v 4.3 1996/05/05 12:26:58 mj Exp $
 *
 * Expire MSGID history database
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
#include "dbz.h"


#define PROGRAM 	"ftnexpire"
#define VERSION 	"$Revision: 4.3 $"
#define CONFIG		CONFIG_MAIN



/*
 * Prototypes
 */
int	do_expire		(void);
int	do_line			(FILE *, char *);



/*
 * Files
 */
static char history      [MAXPATH];
static char history_dir  [MAXPATH];
static char history_pag  [MAXPATH];
static char history_n    [MAXPATH];
static char history_n_dir[MAXPATH];
static char history_n_pag[MAXPATH];

static long history_line = 0;



/*
 * Options
 */
static double max_history = 14;		/* Max. number of days entry stays
					   in MSGID history database */

static time_t max_sec = 0;
static time_t now_sec = 0;
static time_t exp_sec = 0;

static long n_expired = 0;
static long n_processed = 0;



/*
 * Run expire
 */
int do_expire(void)
{
    FILE *hi_o, *hi_n, *fp;
    TIMEINFO ti;
    
    /* Filenames */
    BUF_COPY3(history      , cf_spooldir(), "/", HISTORY          );
    BUF_COPY4(history_dir  , cf_spooldir(), "/", HISTORY, ".dir"  );
    BUF_COPY4(history_pag  , cf_spooldir(), "/", HISTORY, ".pag"  );
    BUF_COPY4(history_n    , cf_spooldir(), "/", HISTORY, ".n"    );
    BUF_COPY4(history_n_dir, cf_spooldir(), "/", HISTORY, ".n.dir");
    BUF_COPY4(history_n_pag, cf_spooldir(), "/", HISTORY, ".n.pag");

    debug(4, "old history: %s\n             %s\n             %s",
	  history, history_dir, history_pag                      );
    debug(4, "new history: %s\n             %s\n             %s",
	  history_n, history_n_dir, history_n_pag                );

    /* Expire time (seconds) */
    GetTimeInfo(&ti);
    now_sec = ti.time;
    max_sec = 24L * 3600L * max_history;
    exp_sec = now_sec - max_sec;
    if(exp_sec < 0)
	exp_sec = 0;
    debug(4, "expire: now=%ld max=%ld, expire < %ld",
	  now_sec, max_sec, exp_sec                   );

    /* Open old history for reading */
    if( (hi_o = fopen(history, R_MODE)) == NULL ) 
    {
	log("$ERROR: open MSGID history %s failed", history);
	return ERROR;
    }

    /* Open new history for writing */
    if( (hi_n = fopen(history_n, W_MODE)) == NULL ) 
    {
	log("$ERROR: open MSGID history %s failed", history_n);
	fclose(hi_o);
	return ERROR;
    }

    /* Create empty new .dir and .pag*/
    if( (fp = fopen(history_n_dir, W_MODE)) == NULL ) 
    {
	log("$ERROR: open MSGID history %s failed", history_n_dir);
	fclose(hi_o);
	fclose(hi_n);
	return ERROR;
    }
    fclose(fp);
    if( (fp = fopen(history_n_pag, W_MODE)) == NULL ) 
    {
	log("$ERROR: open MSGID history %s failed", history_n_pag);
	fclose(hi_o);
	fclose(hi_n);
	return ERROR;
    }
    fclose(fp);
    
    /* Open the new DBZ file */
    dbzincore(1);
    /**dbzwritethrough(1);**/
    if (dbminit(history_n) < 0) {
	log("$ERROR: dbminit %s failed", history_n);
	fclose(hi_o);
	fclose(hi_n);
	return ERROR;
    }


    /* Process entries */
    while(fgets(buffer, sizeof(buffer), hi_o))
    {
	do_line(hi_n, buffer);
    }
    

    /* Close everything */
    /**FIXME: error checking**/
    fclose(hi_o);
    fclose(hi_n);
    dbzsync();
    dbmclose();

    /* Statistics */
    log("ids processed: %ld total, %ld expired", n_processed, n_expired);
    
    /* Rename */
    if(rename(history_n, history) == ERROR)
    {
	log("$ERROR: rename %s -> %s failed", history_n, history);
	return ERROR;
    }
    if(rename(history_n_dir, history_dir) == ERROR)
    {
	log("$ERROR: rename %s -> %s failed", history_n_dir, history_dir);
	return ERROR;
    }
    if(rename(history_n_pag, history_pag) == ERROR)
    {
	log("$ERROR: rename %s -> %s failed", history_n_pag, history_pag);
	return ERROR;
    }
    
    return OK;
}



/*
 * Process one line from history
 */
int do_line(FILE *hi_n, char *line)
{
    char *msgid, *p;
    time_t t;
    int expired;
    long offset;
    int ret;
    datum key, val;

    /* Parse old entry */
    strip_crlf(line);
    history_line++;
    
    msgid = strtok(line, "\t");
    p     = strtok(NULL, "\t");
    if(!msgid || !p)
    {
	log("WARNING: illegal entry in %s, line %ld", history, history_line);
	return ERROR;
    }

    /* Check expire */
    t = atol(p);
    expired = t < exp_sec;
    debug(7, "msgid=%s  time=%ld  expired=%s",
	  msgid, t, expired ? "YES" : "NO"     );

    /* Write if not expired */
    if(!expired) 
    {
	/* Get offset in history text file */
	if( (offset = ftell(hi_n)) == ERROR)
	{
	    log("$ERROR: ftell MSGID history failed");
	    return ERROR;
	}
	
	/* Write MSGID line to history text file */
	ret = fprintf(hi_n, "%s\t%ld\n", msgid, t);
	if (ret == ERROR || fflush(hi_n) == ERROR)
	{
	    log("$ERROR: write to MSGID history failed");
	    return ERROR;
	}

	/* Write database record */
	key.dptr  = msgid;			/* Key */
	key.dsize = strlen(msgid) + 1;
	val.dptr  = (char *)&offset;		/* Value */
	val.dsize = sizeof offset;
	if (dbzstore(key, val) < 0) {
	    log("ERROR: dbzstore of record for MSGID history failed");
	    return ERROR;
	}
    }
    else
	n_expired++;

    n_processed++;
    
    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options]\n\n", PROGRAM);
    fprintf(stderr, "\
options: -m --maxhistory DAYS         set max number of days in history\n\
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
    int c, ret=EXIT_OK;
    char *p;
    char *m_flag=NULL;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
	{ "maxhistory",   1, 0, 'm'},	/* MaxHistory */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "m:vhc:S:L:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftnexpire options *****/
	case 'm':
	    m_flag = optarg;
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
    
    /*
     * Process optional config statements
     *
     * -m overrides config MaxHistory
     */
    if( (p = m_flag) || (p = cf_get_string("MaxHistory", TRUE)) )
    {
	max_history = atof(p);
	if(max_history < 0)
	    max_history = 0;
	debug(8, "config: MaxHistory %lg", max_history);
    }

    /*
     * Run expire, locking MSGID history database
     */
    if(lock_program(LOCK_HISTORY, FALSE) == ERROR)
	/* Already busy */
	exit(EXIT_BUSY);

    if(do_expire() == ERROR)
	ret = EXIT_ERROR;

    unlock_program(LOCK_HISTORY);

    
    exit(ret);

    /**NOT REACHED**/
    return 1;
}
