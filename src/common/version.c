/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: version.c,v 4.1 1996/09/07 16:40:46 mj Exp $
 *
 * FIDOGATE version number handling stuff
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

#include "version.h"



static char global_id[]    = { VERSION };
/* static char global_state[] = { STATE   }; */
    
    
static char *get_keyword_arg (char *);



/*
 * version_global() --- Get global FIDOGATE version string
 */

char *version_global(void)
{
    static char id[128];

#if   defined(ALPHA)
    sprintf(id, "%salpha%d", get_keyword_arg(global_id), ALPHA);
#elif defined(BETA)
    sprintf(id, "%sbeta%d", get_keyword_arg(global_id), BETA);
#else
    sprintf(id, "%s", get_keyword_arg(global_id));
#endif

    return id;
}



/*
 * version_local() --- Get local version from passed RCS Revision string
 */

char *version_local(char *rev)
{
    static char id[128];
    
    strncpy0(id, rev, 128);
    
    return get_keyword_arg(id);
}



/*
 * get_keyword_arg()
 */

static char *get_keyword_arg(char *s)
{
    char *p;

    while(*s && *s!=':')
	s++;
    if(*s == ':')
	s++;
    while(*s && *s==' ')
	s++;
    
    for(p=s; *p && *p!=' '; p++) ;
    *p = 0;
    
    return s;
}



/*
 * Get major/minor version number
 */
int version_major(void)
{
    int major=ERROR, minor=ERROR;
    
    strcpy(buffer, version_global());
    sscanf(buffer, "%d.%d", &major, &minor);
    
    return major;
}

int version_minor(void)
{
    int major=ERROR, minor=ERROR;
    
    strcpy(buffer, version_global());
    sscanf(buffer, "%d.%d", &major, &minor);
    
    return minor;
}
