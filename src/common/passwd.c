/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: passwd.c,v 4.1 1996/04/22 14:31:13 mj Exp $
 *
 * Read PASSWD file for ffx, ffxqt, ftnaf and other programs
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



static Passwd *passwd_list = NULL;
static Passwd *passwd_last = NULL;


/*
 * Read list of passwords from LIBDIR/PASSWD file.
 *
 * Format:
 *     CONTEXT  Z:N/F.P  PASSWORD  [ optional args ]
 */
void passwd_init(void)
{
    FILE *fp;
    Passwd *p;
    char *c, *n, *w, *r;
    
    debug(14, "Reading passwd file");
    
    fp = fopen_expand_name(PASSWD, R_MODE_T);
    if(!fp)
	return;

    while(cf_getline(buffer, BUFFERSIZE, fp))
    {
	c = strtok(buffer, " \t");
	n = strtok(NULL  , " \t");
	w = strtok(NULL  , " \t");
	r = strtok(NULL  , ""   );
	while(r && *r && is_space(*r))
	    r++;
	
	if(!c || !n || !w)
	    continue;

	p = (Passwd *)xmalloc(sizeof(Passwd));

	p->context = strsave(c);
	asc_to_node(n, &p->node, FALSE);
	p->passwd  = strsave(w);
	p->args    = r ? strsave(r) : NULL;
	p->next    = NULL;
	
	debug(15, "passwd: %s %s %s", p->context,
	      node_to_asc(&p->node, TRUE), p->passwd);
	
	/*
	 * Put into linked list
	 */
	if(passwd_list)
	    passwd_last->next = p;
	else
	    passwd_list       = p;
	passwd_last       = p;
    }
    
    fclose(fp);
}



/*
 * Lookup password in list
 */
Passwd *passwd_lookup(char *context, Node *node)
{
    Passwd *p;
    
    for(p=passwd_list; p; p=p->next)
	if(!strcmp(context, p->context) && node_eq(node, &p->node))
	    return p;
    
    return NULL;
}
