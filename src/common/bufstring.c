/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: bufstring.c,v 4.0 1996/04/17 18:17:38 mj Exp $
 *
 * buf_sprintf() --- sprintf() to SHUFFLE buffer
 *
 *****************************************************************************
 * Copyright (C) 1990-1996
 *  _____ _____
 * |	 |___  |   Martin Junius	     FIDO:	2:2452/110
 * | | | |   | |   Republikplatz 3	     Internet:	mj@fido.de
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FIDOGATE; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

#include "fidogate.h"

#include "shuffle.h"



/*
 * sprintf() returning string and using shuffled buffer
 */
char *buf_sprintf(char *fmt, ...)
{
    va_list pvar;

    va_start(pvar, fmt);

    SHUFFLEBUFFERS;
    vsprintf(tcharp, fmt, pvar);
    va_end(pvar);

    return tcharp;
}



/*
 * Copy string to shuffled buffer
 */
char *buf_copy(char *s)
{
    SHUFFLEBUFFERS;
    
    strncpy0(tcharp, s, MAX_CONVERT_BUFLEN);
    
    return tcharp;
}

