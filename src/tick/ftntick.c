/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: ftntick.c,v 4.9 1997/06/21 21:16:46 mj Exp $
 *
 * Process incoming TIC files
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
#include <utime.h>



#define PROGRAM		"ftntick"
#define VERSION		"$Revision: 4.9 $"
#define CONFIG		CONFIG_MAIN



#define MY_AREASBBS	"FAreasBBS"    
#define MY_CONTEXT	"ff"

#define MY_FILESBBS	"files.bbs"



/*
 * Prototypes
 */
int	do_tic			(int);
int	process_tic		(Tick *);
int	move			(Tick *, char *, char *);
int	copy_file		(char *, char *);
int	add_files_bbs		(Tick *, char *);
int	do_seenby		(LON *, LON *, LON *);
int	check_file		(Tick *);

void	short_usage		(void);
void	usage			(void);



/*
 * Processs *.tic files in Binkley inbound directory
 */
int do_tic(int t_flag)
{
    Tick tic;
    char *name;
    Passwd *pwd;
    char *passwd;
    char buf[MAXPATH];
    char pattern[16];

    tick_init(&tic);
    
    BUF_COPY(pattern, "*.tic");

    dir_sortmode(DIR_SORTMTIME);
    if(dir_open(cf_inbound(), pattern, TRUE) == ERROR)
    {
	log("$ERROR: can't open directory %s", cf_inbound());
	return ERROR;
    }
    
    for(name=dir_get(TRUE); name; name=dir_get(FALSE))
    {
	debug(1, "ftntick: tick file %s", name);

	/* Read TIC file */
	if(tick_get(&tic, name) == ERROR)
	{
	    log("$ERROR: reading %s", name);
	    goto rename_to_bad;
	}

	/* Check file against Tick data */
	if(check_file(&tic) == ERROR)
	    goto rename_to_bad;
	
	tick_debug(&tic, 3);

	log("area %s file %s (%lub) from %s", tic.area, tic.file, tic.size,
	    node_to_asc(&tic.from, TRUE));
	
	/*
	 * Get password for from node
	 */
	if( (pwd = passwd_lookup(MY_CONTEXT, &tic.from)) )
	    passwd = pwd->passwd;
	else
	    passwd = NULL;
	if(passwd)
	    debug(3, "ftntick: password %s", passwd);
	
	/*
	 * Require password unless -t option is given
	 */
	if(!t_flag && !passwd)
	{
	    log("%s: no password for %s in PASSWD", name,
		node_to_asc(&tic.from, TRUE)  );
	    goto rename_to_bad;
	}
	
	/*
	 * Check password
	 */
	if(passwd)
	{
	    if(tic.pw)
	    {
		if(stricmp(passwd, tic.pw))
		{
		    log("%s: wrong password from %s: ours=%s his=%s",
			name, node_to_asc(&tic.from, TRUE), passwd,
			tic.pw                                      );
		    goto rename_to_bad;
		}
	    }
	    else
	    {
		log("%s: no password from %s: ours=%s", name,
		    node_to_asc(&tic.from, TRUE), passwd );
		goto rename_to_bad;
	    }
	}
	
	if(process_tic(&tic) == ERROR)
	{
	    log("%s: failed", name);
	rename_to_bad:
	    /*
	     * Error: rename .tic -> .bad
	     */
	    strcpy(buf, name);
	    strcpy(buf+strlen(buf)-3, "bad");
	    rename(name, buf);
	    log("%s: renamed to %s", name, buf);
	}
	else 
	{
	    /* o.k., remove the TIC file */
	    if(unlink(name) == ERROR)
		log("$ERROR: can't remove %s", name);
	}
    }
    
    dir_close();

    return OK;
}



/*
 * Process Tick
 */
int process_tic(Tick *tic)
{
    AreasBBS *bbs;
    LON new;
    LNode *p;
    char old_name[MAXPATH];
    char new_name[MAXPATH];
    
    /*
     * Lookup file area
     */
    if(!tic->area)
    {
	log("ERROR: missing area in %s", tic->file);
	return ERROR;
    }
    if( (bbs = areasbbs_lookup(tic->area)) == NULL )
    {
	log("unknown area %s from %s",
	    tic->area, node_to_asc(&tic->from, TRUE) );
	return ERROR;
    }
    cf_set_zone(bbs->zone);
    tic->to = cf_n_addr();

    /*
     * Check that sender is listed in FAreas.BBS
     */
    if(! lon_search(&bbs->nodes, &tic->from) )
    {
	log("insecure tic area %s from %s", tic->area,
	    node_to_asc(&tic->from, TRUE)             );
	return ERROR;
    }

    /*
     * Replaces: move or delete old file
     */
    if(tic->replaces)
    {
	char *rdir = cf_get_string("TickReplacedDir", TRUE);

	BUF_COPY3(old_name, bbs->dir, "/", tic->replaces);
	if(check_access(old_name, CHECK_FILE) == TRUE)
	{
	    if(rdir)
	    {
		/* Copy to ReplacedFilesDir */
		BUF_COPY3(new_name, rdir, "/", tic->replaces);
		debug(1, "%s -> %s", old_name, new_name);
		if(copy_file(old_name, new_name) == ERROR)
		{
		    log("$ERROR: can't copy %s -> %s", old_name, new_name);
		    return ERROR;
		}
		log("area %s file %s replaces %s, moved to %s",
		    tic->area, tic->file, tic->replaces, rdir);
	    }
	    else
		log("area %s file %s replaces %s, removed",
		    tic->area, tic->file, tic->replaces);
	    
	    /* Remove old file, no error if this fails */
	    unlink(old_name);
	    
	    /* Remove old file from FILES.BBS */
	    /**FIXME**/
	}
    }
    
    /*
     * Move file from inbound to file area, add description to FILES.BBS
     */
    BUF_COPY3(old_name, cf_inbound(), "/", tic->file);
    BUF_COPY3(new_name, bbs->dir    , "/", tic->file);
    debug(1, "%s -> %s", old_name, new_name);
    if(move(tic, old_name, new_name) == ERROR)
	return ERROR;
    add_files_bbs(tic, bbs->dir);

    /*
     * Add us to Path list
     */
    tick_add_path(tic);

    /*
     * Add sender to SEEN-BY if not already there
     */
    if(!lon_search(&tic->seenby, &tic->from))
	lon_add(&tic->seenby, &tic->from);
	
    /*
     * We're the sender
     */
    tic->from = cf_n_addr();
    
    /*
     * Add nodes not already in SEEN-BY to seenby and new.
     */
    lon_init(&new);
    do_seenby(&tic->seenby, &bbs->nodes, &new);
    lon_debug(3, "Send to new nodes: ", &new, TRUE);

    /*
     * Send file to all nodes in LON new
     */
    BUF_COPY3(new_name, bbs->dir, "/", tic->file);
    for(p=new.first; p; p=p->next)
	if(tick_send(tic, &p->node, new_name) == ERROR)
	    log("ERROR: send area %s file %s to %s failed",
		tic->area, tic->file, node_to_asc(&p->node, TRUE));
    

    return OK;
}



/*
 * Move file (copy then unlink)
 */
int move(Tick *tic, char *old, char *new)
{
    unsigned long crc;
    struct utimbuf ut;

    /* Copy */
    if(copy_file(old, new) == ERROR)
    {
	log("$ERROR: can't copy %s -> %s", old, new);
	return ERROR;
    }
    
    /* Compute CRC again to be sure */
    crc = crc32_file(new);
    if(crc != tic->crc)
    {
	log("ERROR: error while copying to %s, wrong CRC", new);
	unlink(new);
	return ERROR;
    }
    
    /* o.k., now unlink file in inbound */
    if(unlink(old) == ERROR)
    {
	log("$ERROR: can't remove %s", old);
	return ERROR;
    }

    /* Set a/mtime to time from TIC */
    if(tic->date != -1)
    {
	ut.actime = ut.modtime = tic->date;
	if(utime(new, &ut) == ERROR)
	{
	    log("$ERROR: can't set time of %s", new);
	    return ERROR;
	}
    }

    return OK;
}



/*
 * Copy file
 */
int copy_file(char *old, char *new)
{
    FILE *fold, *fnew;
    int nr, nw;
    
    /* Open */
    if( (fold = fopen(old, R_MODE)) == NULL)
    {
	return ERROR;
    }
    if( (fnew = fopen(new, W_MODE)) == NULL)
    {
	fclose(fold);
	return ERROR;
    }

    /* Copy */
    do 
    {
	nr = fread(buffer, sizeof(char), sizeof(buffer), fold);
	if(ferror(fold))
	{
	    log("$ERROR: can't read from %s", old);
	    fclose(fold);
	    fclose(fnew);
	    unlink(new);
	    return ERROR;
	}
	
	nw = fwrite(buffer, sizeof(char), nr, fnew);
	if(ferror(fnew))
	{
	    log("$ERROR: can't write to %s", new);
	    fclose(fold);
	    fclose(fnew);
	    unlink(new);
	    return ERROR;
	}
    }
    while(!feof(fold));
    
    /* Close */
    fclose(fold);
    fclose(fnew);

    return OK;
}



/*
 * Add description to FILES.BBS
 */
int add_files_bbs(Tick *tic, char *dir)
{
    char files_bbs[MAXPATH];
    FILE *fp;

    BUF_COPY3(files_bbs, dir, "/", MY_FILESBBS);
    if( (fp = fopen(files_bbs, A_MODE)) == NULL )
    {
	log("$ERROR: can't append to %s", files_bbs);
	return ERROR;
    }
    
    fprintf(fp, "%-12s  %s\r\n", tic->file, tic->desc.first->line);

    fclose(fp);
    
    return OK;
}



/*
 * Add nodes to SEEN-BY (4D)
 */
int do_seenby(LON *seenby, LON *nodes, LON *new)
                				/* Nodes in SEENBY */
               					/* Nodes in FAREAS.BBS */
             					/* New nodes added */
{
    LNode *p;
    
    for(p=nodes->first; p; p=p->next)
	if(! lon_search(seenby, &p->node) )
	{
	    lon_add(seenby, &p->node);
	    if(new)
		lon_add(new, &p->node);
	}

    return OK;
}



/*
 * Check file
 */
int check_file(Tick *tic)
{
    struct stat st;
    unsigned long crc;
    char name[MAXPATH];
    
    if(!tic->file)
    {
	log("ERROR: no file name");
	return ERROR;
    }

    /* Search file */
    if(dir_search(cf_inbound(), tic->file) == NULL)
    {
	log("ERROR: can't find file %s", tic->file);
	return ERROR;
    }

    /* Full path name */
    BUF_COPY3(name, cf_inbound(), "/", tic->file);
    if(stat(name, &st) == ERROR)
    {
	log("$ERROR: can't stat() file %s", name);
	return ERROR;
    }

    /*
     * File size
     */
    if(tic->size)
    {
	if(tic->size != st.st_size)
	{
	    log("ERROR: wrong size for file %s: got %lu, expected %lu",
		name, st.st_size, tic->size                           );
	    return ERROR;
	}
    }
    else
	tic->size = st.st_size;

    /*
     * File date
     */
    if(tic->date == -1)
	tic->date = st.st_mtime;
    
    /*
     * File CRC
     */
    crc  = crc32_file(name);
    if(tic->crc == 0 && crc != 0)
	tic->crc = crc;
    else
    {
	if(tic->crc != crc)
	{
	    log("ERROR: wrong CRC for file %s: got %08lx, expected %08lx",
		name, crc, tic->crc                                       );
	    return ERROR;
	}
    }
    
    
    return OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options]\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options]\n\n", PROGRAM);
    fprintf(stderr, "\
options:  -b --fareas-bbs NAME         use alternate FAREAS.BBS\n\
          -I --inbound dir             set inbound directory\n\
          -t --insecure                process TIC files without password\n\
\n\
          -v --verbose                 more verbose\n\
	  -h --help                    this help\n\
          -c --config name             read config file (\"\" = none)\n\
	  -L --lib-dir name            set lib directory\n\
	  -S --spool-dir name          set spool directory\n\
	  -a --addr Z:N/F.P            set FTN address\n\
	  -u --uplink-addr Z:N/F.P     set FTN uplink address\n");

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    char *areas_bbs = NULL;
    int c;
    char *I_flag=NULL;
    int   t_flag=FALSE;
    char *c_flag=NULL;
    char *S_flag=NULL, *L_flag=NULL;
    char *a_flag=NULL, *u_flag=NULL;
    
    int option_index;
    static struct option long_options[] =
    {
        { "fareas-bbs",	  1, 0, 'b'},
	{ "insecure",     0, 0, 't'},	/* Insecure */
	{ "inbound",      1, 0, 'I'},	/* Set Binkley inbound */

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ "config",       1, 0, 'c'},	/* Config file */
	{ "spool-dir",    1, 0, 'S'},	/* Set FIDOGATE spool directory */
	{ "lib-dir",      1, 0, 'L'},	/* Set FIDOGATE lib directory */
	{ "addr",         1, 0, 'a'},	/* Set FIDO address */
	{ "uplink-addr",  1, 0, 'u'},	/* Set FIDO uplink address */
	{ 0,              0, 0, 0  }
    };

    log_program(PROGRAM);
    
    /* Init configuration */
    cf_initialize();


    while ((c = getopt_long(argc, argv, "b:tI:vhc:S:L:a:u:",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	/***** ftntick options *****/
	case 'b':
	    areas_bbs = optarg;
	    break;
	case 't':
	    t_flag = TRUE;
	    break;
	case 'I':
	    I_flag = optarg;
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
    if(L_flag)
	cf_set_libdir(L_flag);
    if(S_flag)
	cf_set_spooldir(S_flag);
    if(a_flag)
	cf_set_addr(a_flag);
    if(u_flag)
	cf_set_uplink(u_flag);

    cf_debug();

    if(I_flag)
	cf_set_inbound(I_flag);

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
	return EXIT_ERROR;
    }

    
    if(lock_program(PROGRAM, NOWAIT) == ERROR)
	exit(EXIT_BUSY);
    
    do_tic(t_flag);

    unlock_program(PROGRAM);


    exit(0);

    /**NOT REACHED**/
    return 1;
}
