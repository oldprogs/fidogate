/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: tick.c,v 4.0 1996/04/17 18:17:40 mj Exp $
 *
 * TIC file processing
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



#define MY_CONTEXT	"ff"



/*
 * Init Tick struct
 */
void tick_init(Tick *tic)
{
    node_invalid(&tic->origin);
    node_invalid(&tic->from);
    node_invalid(&tic->to);
    tic->area = NULL;
    tic->file = NULL;
    tl_init(&tic->desc);
    tic->crc = 0;
    tic->created = NULL;
    tic->size = 0;
    tl_init(&tic->path);
    lon_init(&tic->seenby);
    tic->pw = 0;
    tic->release = -1;
    tic->date = -1;
    tl_init(&tic->app);
}



/*
 * Delete Tick struct
 */
void tick_delete(Tick *tic)
{
    node_invalid(&tic->origin);
    node_invalid(&tic->from);
    node_invalid(&tic->to);
    xfree(tic->area);
    tic->area = NULL;
    xfree(tic->file);
    tic->file = NULL;
    tl_clear(&tic->desc);
    tic->crc = 0;
    xfree(tic->created);
    tic->created = NULL;
    tic->size = 0;
    tl_clear(&tic->path);
    lon_delete(&tic->seenby);
    xfree(tic->pw);
    tic->pw = NULL;
    tic->release = -1;
    tic->date = -1;
    tl_clear(&tic->app);
}



/*
 * Write TIC file
 */
int tick_put(Tick *tic, char *name)
{
    FILE *fp;
    LNode *p;
    Textline *pl;
    
    if((fp = fopen(name, W_MODE)) == NULL)
	return ERROR;
    
    fprintf(fp, "Area %s\r\n", tic->area);
    fprintf(fp, "Origin %s\r\n", node_to_asc(&tic->origin, FALSE));
    fprintf(fp, "From %s\r\n", node_to_asc(&tic->from, FALSE));
    fprintf(fp, "File %s\r\n", tic->file);
    fprintf(fp, "Desc %s\r\n", tic->desc.first->line);
    fprintf(fp, "CRC %08lX\r\n", tic->crc);
    fprintf(fp, "Created %s\r\n", tic->created);
    fprintf(fp, "Size %lu\r\n", tic->size);
    fprintf(fp, "Date %ld\r\n", tic->date);
    for(pl=tic->path.first; pl; pl=pl->next)
	fprintf(fp, "Path %s\r\n", pl->line);
    for(p=tic->seenby.first; p; p=p->next)
	fprintf(fp, "Seenby %s\r\n", node_to_asc(&p->node, FALSE));
    fprintf(fp, "Pw %s\r\n", tic->pw);
    
    return fclose(fp);
}



/*
 * Read TIC file
 */
int tick_get(Tick *tic, char *name)
{
    FILE *fp;
    char *key, *arg;
    Node node;
    
    tick_delete(tic);
    
    if((fp = fopen(name, R_MODE)) == NULL)
	return ERROR;

    while(fgets(buffer, sizeof(buffer), fp))
    {
	strip_crlf(buffer);
	
	key = strtok(buffer, " \t");		/* Keyword */
	arg = strtok(NULL  , "");		/* Arg(s) */
	
	if(! stricmp(key, "Origin"))
	{
	    if(asc_to_node(arg, &node, FALSE) == OK)
		tic->origin = node;
	}
	
	if(! stricmp(key, "From"))
	{
	    if(asc_to_node(arg, &node, FALSE) == OK)
		tic->from = node;
	}
	
	if(! stricmp(key, "Area"))
	{
	    tic->area = strsave(arg);
	}
	
	if(! stricmp(key, "File"))
	{
	    tic->file = strsave(arg);
	    str_lower(tic->file);
	}
	
	if(! stricmp(key, "Desc"))
	{
	    tl_append(&tic->desc, arg);
	}
	
	if(! stricmp(key, "CRC"))
	{
	    sscanf(arg, "%lx", &tic->crc);
	}
	
	if(! stricmp(key, "Created"))
	{
	    tic->created = strsave(arg);
	}
	
	if(! stricmp(key, "Size"))
	{
	    tic->size = atol(arg);
	}
	
	if(! stricmp(key, "Path"))
	{
	    tl_append(&tic->path, arg);
	}
	
	if(! stricmp(key, "Seenby"))
	{
	    if(asc_to_node(arg, &node, FALSE) == OK)
		lon_add(&tic->seenby, &node);
	}
	
	if(! stricmp(key, "Pw"))
	{
	    tic->pw = strsave(arg);
	}
	
	if(! stricmp(key, "Release"))
	{
	    tic->release = atol(arg);
	}
	
	if(! stricmp(key, "Date"))
	{
	    tic->date = atol(arg);
	}
	
	if(! stricmp(key, "App"))
	{
	    tl_append(&tic->app, arg);
	}
    }
    
    fclose(fp);
    return OK;
}



/*
 * Debug output of Tick struct
 */
void tick_debug(Tick *tic, int lvl)
{
    Textline *pl;
    LNode *p;
    
    debug(lvl, "Origin : %s", node_to_asc(&tic->origin, TRUE));
    debug(lvl, "From   : %s", node_to_asc(&tic->from, TRUE));
    debug(lvl, "To     : %s", node_to_asc(&tic->to, TRUE));
    debug(lvl, "Area   : %s", tic->area);
    debug(lvl, "File   : %s", tic->file);
    for(pl=tic->desc.first; pl; pl=pl->next)
	debug(lvl, "Desc   : %s", pl->line);
    debug(lvl, "CRC    : %08lX", tic->crc);
    debug(lvl, "Created: %s", tic->created);
    debug(lvl, "Size   : %lu", tic->size);
    for(pl=tic->path.first; pl; pl=pl->next)
	debug(lvl, "Path   : %s", pl->line);
    for(p=tic->seenby.first; p; p=p->next)
	debug(lvl, "Seenby : %s", node_to_asc(&p->node, FALSE));
    debug(lvl, "Pw     : %s", tic->pw);
    debug(lvl, "Release: %ld", tic->release);
    debug(lvl, "Date   : %ld", tic->date);
    for(pl=tic->app.first; pl; pl=pl->next)
	debug(lvl, "App    : %s", pl->line);
}




/*
 * Send file and TIC to node
 */
int tick_send(Tick *tic, Node *node, char *name)
{
    Passwd *pwd;
    char *pw = "";

    /*
     * Attach file
     */
    debug(4, "attach %s", name);
    if(bink_attach(node, 0, name, "Hold", TRUE) == ERROR)
	return ERROR;
    
    /*
     * Get password
     */
    if( (pwd = passwd_lookup(MY_CONTEXT, node)) )
	pw = pwd->passwd;
    debug(4, "passwd: %s", pwd ? pwd->passwd : "-NONE-");

    tic->to = *node;
    tic->pw = strsave(pw);

    /*
     * Make sure tick dir exists
     */
    sprintf(buffer, "%s/%s", cf_outbound(), TICK_HOLD);
    if(check_access(buffer, CHECK_DIR) == ERROR)
    {
	if(mkdir(buffer, DIR_MODE) == -1)
	    return ERROR;
	chmod(buffer, DIR_MODE);
    }

    /*
     * Create TIC
     */
    sprintf(buffer, "%s/%s/tk%06ld.tic",
	    cf_outbound(), TICK_HOLD, sequencer(SEQ_TICK) % 1000000);
    debug(4, "creating %s", buffer);
    if(tick_put(tic, buffer) == ERROR)
	return ERROR;
    
    /*
     * Attach TIC
     */
    debug(4, "attach %s", buffer);
    if(bink_attach(node, '^', buffer, "Hold", TRUE) == ERROR)
	return ERROR;

    log("area %s file %s (%lub) to %s",
	tic->area, tic->file, tic->size, node_to_asc(node, TRUE));

    return OK;
}



/*
 * Add Path line with our address
 */
void tick_add_path(Tick *tic)
{
    time_t now;

    now = time(NULL);
    tl_appendf(&tic->path, "%s %ld %s",
	       node_to_asc(cf_addr(), FALSE), now, date(DATE_FIDO, &now) );

}
