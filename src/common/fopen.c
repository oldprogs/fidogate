/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: fopen.c,v 4.2 1996/12/17 17:19:42 mj Exp $
 *
 * Specialized fopen()-like functions
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



/*
 * fopen_expand_name() --- expand file name and open file
 */
FILE *fopen_expand_name(char *name, char *mode)
{
    char xname[MAXPATH];

    if(!name)
	return NULL;
    
    str_expand_name(xname, sizeof(xname), name);
    
    return fopen(xname, mode);
}



/*
 * xfopen() --- expand file name, open file, check for error
 */
FILE *xfopen(char *name, char *mode)
{
    FILE *fp;
    
    if(!name)
	return NULL;
    
    if((fp = fopen_expand_name(name, mode)) == NULL) {
	log("$ERROR: can't open %s", name);
	exit(EX_OSFILE);
    }
    return fp;
}



/*
 * libfopen() --- open file in LIBDIR, check for error
 */
FILE *libfopen(char *name, char *mode)
{
    char filename[MAXPATH];

    BUF_COPY3(filename, cf_libdir(), "/", name);

    return xfopen(filename, mode);
}



/*
 * spoolfopen() --- open file in SPOOLDIR, check for error
 */
FILE *spoolfopen(char *name, char *mode)
{
    char filename[MAXPATH];

    BUF_COPY3(filename, cf_spooldir(), "/", name);

    return xfopen(filename, mode);
}
