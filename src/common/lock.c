/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: lock.c,v 4.2 1996/06/06 15:59:28 mj Exp $
 *
 * File locking
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

#include <sys/types.h>
#include <fcntl.h>


/*
 * lock_fd() --- lock file using file descriptor, wait
 */
int lock_fd(int fd)
{
#if defined(OS2) || defined(MSDOS)
    return OK;
#else
    struct flock fl;
    int err;
    
    fl.l_type   = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    
    do
    {
	err = fcntl(fd, F_SETLKW, &fl);
    }
    while(err == EINTR);
    
    return err;
#endif /**OS2**/
}



/*
 * unlock_fd() --- unlock file using file descriptor
 */
int unlock_fd(int fd)
{
#if defined(OS2) || defined(MSDOS)
    return OK;
#else
    struct flock fl;
    int err;
    
    fl.l_type   = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start  = 0;
    fl.l_len    = 0;
    
    do
    {
	err = fcntl(fd, F_SETLKW, &fl);
    }
    while(err == EINTR);
    
    return err;
#endif /**OS2**/
}



/*
 * lock_file() --- lock file using FILE *
 */
int lock_file(FILE *fp)
{
    return lock_fd( fileno(fp) );
}



/*
 * unlock_file() --- unlock file using FILE *
 */
int unlock_file(FILE *fp)
{
    return unlock_fd( fileno(fp) );
}



/*
 * Create lock file
 */
int lock_lockfile(char *name, int wait)
{
    int fd;
    FILE *fp;
    
    /* Create lock file */
    debug(5, "Creating lock file %s ...", name);
    do
    {
	/*
	 * Use open() with flag O_EXCL, this will fail if the
	 * lock file already exists
	 */
	fd = open(name, O_RDWR | O_CREAT | O_EXCL, BSY_MODE);
	debug(5, "Creating lock file %s %s.",
	      name, fd==-1 ? "failed" : "succeeded");
	if(fd != -1)
	{
	    if((fp = fdopen(fd, "w")))
	    {
		fprintf(fp, "%d\n", (int)getpid());
		fclose(fp);
	    }
	    close(fd);
	}
	else if(wait)
	    sleep(5);
    }
    while(fd==-1 && wait);

    return fd==-1 ? ERROR : OK;
}



/*
 * Remove lock file
 */
int unlock_lockfile(char *name)
{
    int ret;
    
    ret = unlink(name);
    debug(5, "Deleting lock file %s %s.",
	  name, ret==-1 ? "failed" : "succeeded");

    return ret==-1 ? ERROR : OK;
}



/*
 * Create lock file for program in SPOOLDIR/LOCKS
 */
int lock_program(char *name, int wait)
{
    char buf[MAXPATH];

    BUF_COPY5(buf, cf_spooldir(), "/", LOCKS, "/", name);

    return lock_lockfile(buf, wait);
}



/*
 * Remove lock file for program in SPOOLDIR/LOCKS
 */
int unlock_program(char *name)
{
    char buf[MAXPATH];
    
    BUF_COPY5(buf, cf_spooldir(), "/", LOCKS, "/", name);

    return unlock_lockfile(buf);
}



/***** Test program *********************************************************/
#ifdef TEST

int main(int argc, char *argv[])
{
    char *file;
    FILE *fp;
    int c;
    
    if(argc < 2)
    {
	fprintf(stderr, "usage: lock.c-TEST file\n");
	exit(1);
    }
    file = argv[1];

    if((fp = fopen(file, "a")) == NULL)
    {
	fprintf(stderr, "lock.c-TEST: can't open %s: ", file);
	perror("");
	exit(1);
    }    

    printf("Locking %s ...\n", file);
    if(lock_file(fp))
    {
	fprintf(stderr, "lock.c-TEST: can't lock %s: ", file);
	perror("");
	exit(1);
    }    
    printf("%s locked.\n", file);

    printf("Press <Return> ..."); fflush(stdout);
    while((c = getchar()) != '\n') ;

    printf("Unlocking %s ...\n", file);
    if(unlock_file(fp))
    {
	fprintf(stderr, "lock.c-TEST: can't unlock %s: ", file);
	perror("");
	exit(1);
    }    
    printf("%s unlocked.\n", file);

    exit(0);
}

#endif /**TEST**/
