/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: binkley.c,v 4.6 1998/04/07 12:21:55 mj Exp $
 *
 * BinkleyTerm-style outbound directory functions
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

#include <fcntl.h>




/*
 * BinkleyTerm flavors and FLO/OUT file extensions
 */
#define NOUTB		5

static struct st_outb
{
    int type;
    char flo[4];
    char out[4];
    char flav[8];
    char shrt[2];
}
outb_types[NOUTB] =
{
    { FLAV_NONE  , ""   , ""   , "None"  , "-" },
    { FLAV_HOLD  , "hlo", "hut", "Hold"  , "H" },
    { FLAV_NORMAL, "flo", "out", "Normal", "N" },
    { FLAV_DIRECT, "dlo", "dut", "Direct", "D" },
    { FLAV_CRASH , "clo", "cut", "Crash" , "C" }
};

	
	
/*
 * FLAV_* flavor code to string
 */
char *flav_to_asc(int flav)
{
    int i;

    for(i=0; i<NOUTB; i++)
	if(outb_types[i].type == flav)
	    return outb_types[i].flav;

    return "Normal";
}



/*
 * String to FLAV_* flavor code
 */
int asc_to_flav(char *flav)
{
    int i;

    for(i=0; i<NOUTB; i++)
	if(!stricmp(outb_types[i].flav, flav))
	    return outb_types[i].type;

    return ERROR;
}



/*
 * Convert node address to outbound base name
 */
char *bink_out_name(Node *node)
{
    static char buf[MAXPATH];
    char *out, *outbound;
    
    out = cf_zones_out(node->zone);
    if(!out)
	return NULL;
    outbound = cf_p_btbasedir();
    if(!outbound)
	return NULL;

#ifdef AMIGADOS_4D_OUTBOUND
    sprintf(buf,"%s/%s/%d.%d.%d.%d.", outbound, out, node->zone,
            node->net, node->node, node->point);
#else    
    if(node->point)
	sprintf(buf, "%s/%s/%04x%04x.pnt/0000%04x.",
		outbound, out, node->net, node->node, node->point);
    else
	sprintf(buf, "%s/%s/%04x%04x.",
		outbound, out, node->net, node->node);
#endif /**AMIGADOS_4D_OUTBOUND**/

    return buf;
}



/*
 * Name of BSY file for a node
 */
char *bink_bsy_name(Node *node)
{
    static char buf[MAXPATH];
    char *out;
    
    out = bink_out_name(node);
    if(!out)
	return NULL;

    strncpy0(buf, out,   sizeof(buf));
    strncat0(buf, "bsy", sizeof(buf));
    debug(6, "node=%s bsy file=%s", node_to_asc(node, TRUE), buf);
    return buf;
}



/*
 * Test for existing BSY file
 */
int bink_bsy_test(Node *node)
{
    char *name = bink_bsy_name(node);

    if(!name)
	return FALSE;

    return check_access(name, CHECK_FILE)==TRUE;
}



/*
 * Create BSY file for a node
 */
int bink_bsy_create(Node *node, int wait)
{
#ifdef DO_BSY_FILES
    char *name = bink_bsy_name(node);

    if(!name)
	return ERROR;

    /* Create directory if necessary */
    if(bink_mkdir(node) == ERROR)
	return ERROR;
    
    /* Create BSY file */
# ifdef NFS_SAFE_LOCK_FILES
    return lock_lockfile_nfs(name, wait, NULL);
# else
    return lock_lockfile_id(name, wait, NULL);
# endif
#else
    return OK;
#endif
}



/*
 * Delete BSY file for a node
 */
int bink_bsy_delete(Node *node)
{
#ifdef DO_BSY_FILES
    char *name = bink_bsy_name(node);
    int ret;
    
    if(!name)
	return ERROR;

    ret = unlink(name);
    debug(5, "Deleting BSY file %s %s.",
	  name, ret==-1 ? "failed" : "succeeded");

    return ret==-1 ? ERROR : OK;
#else
    return OK;
#endif
}



/*
 * Find FLO file for node
 *
 * flav==NULL: only return non-NULL if existing FLO file found.
 * flav!=NULL: return existing FLO file or name of new FLO file according
 *             to flav.
 */
char *bink_find_flo(Node *node, char *flav)
{
    static char buf[MAXPATH];
    char *outb, *flo=NULL;
    int i;
    
    outb = bink_out_name(node);
    if(!outb)
	return NULL;
    
    /*
     * Search existing FLO files first
     */
    for(i=1; i<NOUTB; i++)
    {
	strncpy0(buf, outb,              sizeof(buf));
	strncat0(buf, outb_types[i].flo, sizeof(buf));
	if(access(buf, F_OK) == 0)
	{
	    /* FLO file exists */
	    debug(5, "found FLO file %s", buf);
	    return buf;
	}
    }

    if(!flav)
	return NULL;

    /*
     * No FLO file exists, new one with flavor from arg
     */
    for(i=1; i<NOUTB; i++)
    {
	if(!stricmp(outb_types[i].flav, flav) ||
	   !stricmp(outb_types[i].shrt, flav) ||
	   !stricmp(outb_types[i].flo , flav)   )
	    flo = outb_types[i].flo;
    }
    if(!flo)
	return NULL;

    strncpy0(buf, outb, sizeof(buf));
    strncat0(buf, flo , sizeof(buf));
    debug(5, "new FLO file %s", buf);
    return buf;
}



/*
 * Find OUT file for node
 *
 * flav==NULL: only return non-NULL if existing OUT file found.
 * flav!=NULL: return existing OUT file or name of new OUT file according
 *             to flav.
 */
char *bink_find_out(Node *node, char *flav)
{
    static char buf[MAXPATH];
    char *outb, *out=NULL;
    int i;
    
    outb = bink_out_name(node);
    if(!outb)
	return NULL;
    
    /*
     * Search existing OUT files first
     */
    for(i=1; i<NOUTB; i++)
    {
	strncpy0(buf, outb,              sizeof(buf));
	strncat0(buf, outb_types[i].out, sizeof(buf));
	if(access(buf, F_OK) == 0)
	{
	    /* OUT file exists */
	    debug(5, "found OUT file %s", buf);
	    return buf;
	}
    }

    if(!flav)
	return NULL;

    /*
     * No OUT file exists, new one with flavor from arg
     */
    for(i=1; i<NOUTB; i++)
    {
	if(!stricmp(outb_types[i].flav, flav) ||
	   !stricmp(outb_types[i].shrt, flav) ||
	   !stricmp(outb_types[i].out , flav)   )
	    out = outb_types[i].out;
    }
    if(!out)
	return NULL;

    strncpy0(buf, outb, sizeof(buf));
    strncat0(buf, out , sizeof(buf));
    debug(5, "new OUT file %s", buf);
    return buf;
}



/*
 * Attach file to FLO control file
 */
int bink_attach(Node *node, int mode, char *name, char *flav, int bsy)
{
    FILE *fp;
    char *flo;
    char *n;

    if(mode)
	debug(4, "attach mode=%c (^=delete, #=trunc)", mode);
    debug(4, "attach name=%s", name);

    if(cf_dos())			/* MSDOS translation enabled? */
    {
	n = cf_dos_xlate(name);
	if(!n)
	{
	    log("can't convert file name to MSDOS: %s", name);
	    return ERROR;
	}
	debug(4, "attach MSDOS name=%s", n);
    }
    else
	n = name;
    
    flo = bink_find_flo(node, flav);
    if(!flo)
	return ERROR;

    /*
     * Create directory if necessary
     */
    if(bink_mkdir(node) == ERROR)
	return ERROR;
    
    /*
     * Create BSY file
     */
    if(bsy)
	if(bink_bsy_create(node, WAIT) == ERROR)
	    return ERROR;
    
    /*
     * Open and lock FLO file
     */
    do
    {
	/* Open FLO file for append */
	debug(4, "Open FLO file in append mode");
	fp = fopen(flo, A_MODE);
	if(fp == NULL)
	{
	    /* If this failed we're out of luck ... */
	    log("$append to FLO file %s failed", flo);
	    if(bsy)
		bink_bsy_delete(node);
	    return ERROR;
	}
	chmod(flo, FLO_MODE);

	/* Lock it, waiting for lock to be granted */
	debug(4, "Locking FLO file");
	if(lock_file(fp))
	{
	    /* Lock error ... */
	    log("$locking FLO file %s failed", flo);
	    if(bsy)
		bink_bsy_delete(node);
	    return ERROR;
	}

	/* Lock succeeded, but the FLO file may have been deleted */
	if(access(flo, F_OK) == -1)
	{
	    debug(4, "FLO file deleted, retrying");
	    fclose(fp);
	    fp = NULL;
	}
	/* Seek to EOF again, in case someone else has appended */
	else if(fseek(fp, 0L, SEEK_END) == -1)
	{
	    /* fseek() error ... */
	    log("$fseek EOF FLO file %s failed", flo);
	    if(bsy)
		bink_bsy_delete(node);
	    return ERROR;
	}
    }
    while(fp == NULL);
    
    /*
     * We're there ...
     */
    debug(4, "FLO file open and locking succeeded");
    
    if(mode)
	fprintf(fp, "%c%s%s", mode, n, cf_dos() ? "\r\n" : "\n" );
    else
	fprintf(fp, "%s%s"  ,       n, cf_dos() ? "\r\n" : "\n" );
    fclose(fp);

    if(bsy)
	bink_bsy_delete(node);
    return OK;
}



/*
 * Check access for file/directory
 */
int check_access(char *name, int check)
{
    struct stat st;
    
    if(stat(name, &st) == -1)
	return ERROR;
    
    if(check==CHECK_FILE && S_ISREG(st.st_mode))
	return TRUE;
    if(check==CHECK_DIR  && S_ISDIR(st.st_mode))
	return TRUE;

    return FALSE;
}



/*
 * Create directory for zone/points if needed
 */
int bink_mkdir(Node *node)
{
    char buf[MAXPATH];
    char *base;
    
    /*
     * Outbound dir + zone dir
     */
    strncpy0(buf, cf_p_btbasedir(), sizeof(buf));
    strncat0(buf, "/"          , sizeof(buf));
    if((base = cf_zones_out(node->zone)) == NULL)
	return ERROR;
    strncat0(buf, base         , sizeof(buf));
    base = buf + strlen(buf);

    if(check_access(buf, CHECK_DIR) == ERROR)
    {
	if(mkdir(buf, DIR_MODE) == -1)
	    return ERROR;
	chmod(buf, DIR_MODE);
    }
    
    /*
     * Point directory for point addresses
     */
    if(node->point)
    {
	sprintf(base, "/%04x%04x.pnt", node->net, node->node);
	if(check_access(buf, CHECK_DIR) == ERROR)
	{
	    if(mkdir(buf, DIR_MODE) == -1)
		return ERROR;
	    chmod(buf, DIR_MODE);
	}
    }

    return OK;
}



/*
 * Get file size
 */
long check_size(char *name)
{
    struct stat st;
    
    if(stat(name, &st) == -1)
	return ERROR;
    else
	return st.st_size;
}



/*
 * Check for old archive (m_time older than dt)
 */
int check_old(char *name, time_t dt)
{
    struct stat st;
    TIMEINFO ti;
    time_t t;
    
    GetTimeInfo(&ti);
    t = ti.time;
    
    if(stat(name, &st) == -1)
	return ERROR;

    return t - st.st_mtime > dt;
}
