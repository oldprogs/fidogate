/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: exit.c,v 4.5 2000/01/28 22:01:10 mj Exp $
 *
 * FIDOGATE's exit() replacement with clean-up
 *
 *****************************************************************************
 * Copyright (C) 1990-2000
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

/* Don't include "fidogate.h" here ! */

#include <stdlib.h>

/* Duplicates of definitions in fidogate.h */
typedef int (*ExitHandlerF) (int);
void   *xmalloc			(int);


/*
 * List of exit handlers
 */
typedef struct st_exit_handler
{
    ExitHandlerF f;			/* Function pointer */
    struct st_exit_handler *next;	/* Next entry */
} ExitHandler;


static ExitHandler *ehl_first = NULL;
static ExitHandler *ehl_last  = NULL;



/*
 * Add function to list of exit handlers
 */
void exit_handler(ExitHandlerF f)
{
    ExitHandler *eh;
    
    eh = (ExitHandler *)xmalloc(sizeof(ExitHandler));
    eh->f     = f;
    eh->next  = NULL;
    
    if(ehl_first)
	ehl_last->next = eh;
    else
	ehl_first      = eh;
    ehl_last = eh;
}



/*
 * Replacement for standard exit()
 */
void fidogate_exit(int status)
{
    ExitHandler *eh;

    /* Run exit handler functions */
    for(eh=ehl_first; eh; eh=eh->next)
	(eh->f)(status);

    /* Now the real libc exit() */
    exit(status);
}
