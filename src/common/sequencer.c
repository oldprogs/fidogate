/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: sequencer.c,v 4.2 1998/01/18 09:47:54 mj Exp $
 *
 * Number sequencer using sequence file in LIBDIR
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



/*
 * Sequencer: read number from file and increment it
 */
long sequencer(char *filename)
{
    long seqn = sequencer_nx(filename);
    
    if(seqn == -1L)
    {
	log("$can't access sequencer file %s/%s/%s",
	    cf_p_spooldir(), SEQ, filename);
	exit(EX_OSFILE);
    }
    
    return seqn;
}


long sequencer_nx(char *filename)
{
    char buffer[MAXPATH];
    FILE *fp;
    long seqn;

    BUF_COPY5(buffer, cf_p_spooldir(), "/", SEQ, "/", filename);

    /*
     * Open file, create if necessary
     */
    if((fp = fopen(buffer, RP_MODE)) == NULL)
	if(errno == ENOENT) {
	    /* Create */
	    if((fp = fopen(buffer, WP_MODE)) == NULL)
		return -1L;
	}
	else
	    return -1L;

    /*
     * Lock file, get number and increment it
     */
    lock_file(fp);

    if(fgets(buffer, sizeof(buffer), fp))
	seqn = atol(buffer);
    else
	seqn = 0;
    seqn++;
    if(seqn < 0)
	seqn = 0;
    
    rewind(fp);
    fprintf(fp, "%ld\n", seqn);
    rewind(fp);

    fclose(fp);

    return seqn;
}		
