/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: history.c,v 4.1 1996/05/05 12:26:58 mj Exp $
 *
 * MSGID history functions and dupe checking
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
#include "dbz.h"



/*
 * history file
 */
static FILE *hi_file = NULL;



/*
 * Initialize MSGID history
 */
int hi_init(void)
{
    FILE *fp;
    
    /* Test for history.dir, history.pag */
    BUF_COPY4(buffer, cf_spooldir(), "/", HISTORY, ".dir");
    if(check_access(buffer, CHECK_FILE) != TRUE)
    {
	/* Doesn't exist, create */
	if( (fp = fopen(buffer, W_MODE)) == NULL )
	{
	    log("$ERROR: creating MSGID history %s failed", buffer);
	    return ERROR;
	}
	else
	    log("creating MSGID history %s", buffer);
    }
    
    BUF_COPY4(buffer, cf_spooldir(), "/", HISTORY, ".pag");
    if(check_access(buffer, CHECK_FILE) != TRUE)
    {
	/* Doesn't exist, create */
	if( (fp = fopen(buffer, W_MODE)) == NULL )
	{
	    log("$ERROR: creating MSGID history %s failed", buffer);
	    return ERROR;
	}
	else
	    log("creating MSGID history %s", buffer);
    }

    /* Open the history text file */
    BUF_COPY3(buffer, cf_spooldir(), "/", HISTORY);
    if( (hi_file = fopen(buffer, A_MODE)) == NULL ) 
    {
	log("$ERROR: open MSGID history %s failed", buffer);
	return ERROR;
    }

    /* Open the DBZ file */
    dbzincore(1);
    /**dbzwritethrough(1);**/
    if (dbminit(buffer) < 0) {
	log("$ERROR: dbminit %s failed", buffer);
	return ERROR;
    }


    return OK;
}



/*
 * Close MSGID history
 */
int hi_close(void)
{
    int ret = OK;
    
    if(hi_file)
    {
	if(fclose(hi_file) == ERROR) 
	{
	    log("$ERROR: close MSGID history failed");
	    ret = ERROR;
	}

	if (dbzsync())
	{
	    log("$ERROR: dbzsync MSGID history failed");
	    ret = ERROR;
	}
	if (dbmclose() < 0)
	{
	    log("$ERROR: dbmclose MSGID history failed");
	    ret = ERROR;
	}

	hi_file = NULL;
    }

    return ret;
}



/*
 * Write record to MSGID history
 */
int hi_write(time_t msgdate, char *msgid)
    /* msgdate currently not used */
{
    long offset;
    TIMEINFO ti;
    int ret;
    datum key, val;

    GetTimeInfo(&ti);
    
    /* Get offset in history text file */
    if( (offset = ftell(hi_file)) == ERROR)
    {
	log("$ERROR: ftell MSGID history failed");
	return ERROR;
    }

    /* Write MSGID line to history text file */
    debug(7, "history: offset=%ld: %s %ld", offset, msgid, ti.time);
    ret = fprintf(hi_file, "%s\t%ld\n", msgid, ti.time);
    if (ret == ERROR || fflush(hi_file) == ERROR)
    {
	log("$ERROR: write to MSGID history failed");
	return ERROR;
    }

    /* Write database record */
    key.dptr  = msgid;				/* Key */
    key.dsize = strlen(msgid) + 1;
    val.dptr  = (char *)&offset;		/* Value */
    val.dsize = sizeof offset;
    if (dbzstore(key, val) < 0) {
	log("ERROR: dbzstore of record for MSGID history failed");
	return ERROR;
    }

    /**FIXME: dbzsync() every n msgids */
    
    return OK;
}



/*
 * Test if MSGID is already in database
 */
int hi_test(char *msgid)
{
    datum key, val;

    key.dptr  = msgid;				/* Key */
    key.dsize = strlen(msgid) + 1;
    val = dbzfetch(key);
    return val.dptr != NULL;
}





#ifdef TEST
/*
 * history test
 */
int main(int argc, char *argv[])
{
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    int i, c;
    char *m;
    

    /* Init configuration */
    cf_initialize();

    while ((c = getopt(argc, argv, "vc:S:L:")) != EOF)
	switch (c) {
	/***** Common options *****/
	case 'v':
	    verbose++;
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
	    exit(EX_USAGE);
	    break;
	}

    /*
     * Read config file
     */
    if(L_flag)				/* Must set libdir beforehand */
	cf_set_libdir(L_flag);
    cf_read_config_file(c_flag ? c_flag : CONFIG_MAIN);

    /*
     * Process config options
     */
    if(L_flag)
	cf_set_libdir(L_flag);
    if(S_flag)
	cf_set_spooldir(S_flag);

    cf_debug();


    hi_init();

    for(i=optind; i<argc; i++)
    {
	m = argv[i];
	if(hi_test(m))
	{
	    printf("%s FOUND\n", m);
	}
	else
	{
	    printf("%s NOT FOUND, storing\n", m);
	    hi_write(0, m);
	}
    }
    
    hi_close();
    
    exit(0);

    /**NOT REACHED**/
    return 1;
}
#endif /**TEST**/
