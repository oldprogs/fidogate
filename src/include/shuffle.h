/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: shuffle.h,v 4.2 1998/01/18 09:48:06 mj Exp $
 *
 * Something very special ... ;-)
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

#define loop_increment(x,y) (x = (++x >= (y)) ? 0 : x)
#define loop_decrement(x,y) (x = (--x < 0) ? (y)-1 : x)

#define MAX_CONVERT_BUFFERS		20
#define MAX_CONVERT_BUFLEN		256

static char bufs[MAX_CONVERT_BUFFERS][MAX_CONVERT_BUFLEN], *tcharp;
static int bflag;

#define SHUFFLEBUFFERS \
loop_increment(bflag, MAX_CONVERT_BUFFERS); tcharp = bufs[bflag]
