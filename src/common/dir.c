/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: dir.c,v 4.0 1996/04/17 18:17:39 mj Exp $
 *
 * Reading/sorting directories
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



/*
 * struct for sorted dir
 */
typedef struct st_direntry
{
    char  *name;
    off_t  size;
    time_t mtime;
}
DirEntry;

#define DIR_INITSIZE	50


static DirEntry *dir_array  = NULL;		/* Array sorted dir */
static int       dir_narray = 0;		/* Array size */
static int	 dir_nentry = 0;		/* Array entries */
static int       dir_smode  = DIR_SORTNAME;	/* Sort mode */



/*
 * Prototypes
 */
static void dir_resize		(int);
static int  dir_compare		(const void *, const void *);



/*
 * Resize DirEntry array
 */
static void dir_resize(int new)
{
    DirEntry *old;
    int i;

    old = dir_array;
    
    dir_array = (DirEntry *)xmalloc(new * sizeof(DirEntry));
    
    /* Copy old entries */
    for(i=0; i<dir_narray; i++)
	dir_array[i] = old[i];
    
    /* Init new entries */
    for(; i<new; i++)
    {
	dir_array[i].name  = NULL;
	dir_array[i].size  = 0;
	dir_array[i].mtime = 0;
    }

    xfree(old);
    
    dir_narray = new;
}



/*
 * Comparison function for qsort()
 */
int dir_compare(const void *pa, const void *pb)
{
    DirEntry *a, *b;
    
    a = (DirEntry *)pa;
    b = (DirEntry *)pb;
    
    switch(dir_smode)
    {
    case DIR_SORTNAME:
	return strcmp(a->name, b->name);
    case DIR_SORTNAMEI:
	return stricmp(a->name, b->name);
    case DIR_SORTSIZE:
	return (int)(a->size - b->size);
    case DIR_SORTMTIME:
	return (int)(a->mtime - b->mtime);
    case DIR_SORTNONE:
    default:
	return 0;
    }

    /**NOT REACHED**/
    return 0;
}



/*
 * Read dir into memory and sort
 */
int dir_open(char *name, char *pattern, int ic)
    /* ic --- TRUE=ignore case, FALSE=don't */
{
    char buf[MAXPATH];
    struct dirent *dir;
    DIR *dp;
    struct stat st;
    
    if(dir_array)
	dir_close();
    
    /*
     * Open and read directory
     */
    if( ! (dp = opendir(name)) )
	return ERROR;

    dir_resize(DIR_INITSIZE);
    
    while((dir = readdir(dp)))
	if(pattern==NULL || wildmatch(dir->d_name, pattern, ic))
	{
	    strncpy0(buf, name       , sizeof(buf));
	    strncat0(buf, "/"        , sizeof(buf));
	    strncat0(buf, dir->d_name, sizeof(buf));
	    
	    if(stat(buf, &st) == ERROR)
	    {
		dir_close();
		return ERROR;
	    }

	    if(dir_nentry >= dir_narray)
		dir_resize(2 * dir_narray);
	    
	    dir_array[dir_nentry].name  = strsave(buf);
	    dir_array[dir_nentry].size  = st.st_size;
	    dir_array[dir_nentry].mtime = st.st_mtime;

	    dir_nentry++;
	}
    
    closedir(dp);

    /*
     * Sort it
     */
    qsort(dir_array, dir_nentry, sizeof(DirEntry), dir_compare);
    
    return OK;
}



/*
 * Delete sorted directory array
 */
void dir_close(void)
{
    int i;
    
    for(i=0; i<dir_nentry; i++)
	xfree(dir_array[i].name);
    
    xfree(dir_array);
    
    dir_array  = NULL;
    dir_narray = 0;
    dir_nentry = 0;
}



/*
 * Set sort mode
 */
void dir_sortmode(int mode)
{
    dir_smode = mode;
}



/*
 * Get first/next entry
 */
char *dir_get(int first)
{
    static int index = 0;
    
    if(first)
	index = 0;
    
    if(index < dir_nentry)
	return dir_array[index++].name;
    
    return NULL;
}



/*
 * Search for file in directory, ignoring case
 */
char *dir_search(char *dirname, char *name)
    /* name --- file name to find, overwritten with correct case if found */
{
    struct dirent *dir;
    DIR *dp;

    /*
     * Open and read directory
     */
    if( ! (dp = opendir(dirname)) )
	return NULL;

    while((dir = readdir(dp)))
	if(!stricmp(dir->d_name, name))
	{
	    strcpy(name, dir->d_name);
	    closedir(dp);
	    return name;
	}
    
    closedir(dp);
    return NULL;
}



#ifdef TEST

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
	fprintf(stderr, "usage: testdir dir\n");
    }
    else
    {
	char *n;

	printf("%s (name):\n", argv[1]);
	dir_sortmode(DIR_SORTNAME);
	dir_open(argv[1], NULL);
	for(n=dir_get(TRUE); n; n=dir_get(FALSE))
	    printf(" %s\n", n);
	printf("\n");
	
	printf("%s (size):\n", argv[1]);
	dir_sortmode(DIR_SORTSIZE);
	dir_open(argv[1], NULL);
	for(n=dir_get(TRUE); n; n=dir_get(FALSE))
	    printf(" %s\n", n);
	printf("\n");
	
	printf("%s (*.pkt, mtime):\n", argv[1]);
	dir_sortmode(DIR_SORTMTIME);
	dir_open(argv[1], "*.pkt");
	for(n=dir_get(TRUE); n; n=dir_get(FALSE))
	    printf(" %s\n", n);
	printf("\n");
    }
    
    return 0;
}

#endif /**TEST**/
