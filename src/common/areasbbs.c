/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: areasbbs.c,v 4.0 1996/04/17 18:17:38 mj Exp $
 *
 * Function for processing AREAS.BBS EchoMail distribution file.
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



static char     *areasbbs_1stline = NULL;
static AreasBBS *areasbbs_list    = NULL;
static AreasBBS *areasbbs_last    = NULL;



/*
 * Create new AreasBBS struct for line from AREAS.BBS
 */
static AreasBBS *areasbbs_new	(char *);

static AreasBBS *areasbbs_new(char *line)
{
    AreasBBS *p;
    char *dir, *tag, *nl, *o1, *o2;
   
    dir = strtok(line, " \t");
    tag = strtok(NULL, " \t");
    nl  = strtok(NULL, "\r\n");
    if(!dir || !tag || !nl)
	return NULL;
    
    while(is_space(*nl))
	nl++;
    
    p = (AreasBBS *)xmalloc(sizeof(AreasBBS));
    
    p->next  = NULL;
    p->flags = 0;
    if(*dir == '#')
    {
	p->flags |= AREASBBS_PASSTHRU;
	dir++;
    }
    p->dir   = strsave(dir);
    p->area  = strsave(tag);
    p->zone  = -1;
    node_invalid(&p->addr);
    p->lvl   = -1;
    p->key   = NULL;
    
    /*
     * Parse options before list of linked nodes:
     *
     *     -a Z:N/F.P    alternate AKA for this area
     *     -z ZONE       alternate zone AKA for this area
     *     -l LVL        Areafix access level
     *     -k KEY        Areafix access key
     */
    while(*nl == '-')
    {
	o1 = strtok(nl  , " \t");
	o2 = strtok(NULL, " \t");
	nl = strtok(NULL, "");
 	while(is_space(*nl))
	    nl++;

	if(o1 && o2 && !strcmp(o1, "-a"))		/* -a Z:N/F.P */
	    asc_to_node(o2, &p->addr, FALSE);
	if(o1 && o2 && !strcmp(o1, "-z"))		/* -z ZONE */
	    p->zone = atoi(o2);
	if(o1 && o2 && !strcmp(o1, "-l"))		/* -l LVL */
	    p->lvl = atoi(o2);
	if(o1 && o2 && !strcmp(o1, "-k"))		/* -k KEY */
	    p->key = strsave(o2);
    }	
    
    lon_init(&p->nodes);
    lon_add_string(&p->nodes, nl);
    if(p->zone == -1)
	p->zone = p->nodes.first ? p->nodes.first->node.zone : 0;
    
    return p;
}



/*
 * Read area distribution list from AREAS.BBS file
 *
 * Format:
 *    [#$]DIR AREA [-options] Z:N/F.P Z:N/F.P ...
 */
int areasbbs_init(char *name)
{
    FILE *fp;
    AreasBBS *p;

    if(!name)
	return ERROR;
    
    debug(14, "Reading %s file" , name);
    
    fp = fopen(name, R_MODE);
    if(!fp)
	return ERROR;
    
    /*
     * 1st line is special
     */
    if(fgets(buffer, BUFFERSIZE, fp))
    {
	strip_crlf(buffer);
	areasbbs_1stline = strsave(buffer);
    }

    /*
     * The following lines are areas and linked nodes
     */
    while(fgets(buffer, BUFFERSIZE, fp))
    {
	p = areasbbs_new(buffer);
	if(!p)
	    continue;
	
	debug(15, "areas.bbs: %s %s Z%d", p->dir, p->area, p->zone);

	/*
	 * Put into linked list
	 */
	if(areasbbs_list)
	    areasbbs_last->next = p;
	else
	    areasbbs_list       = p;
	areasbbs_last       = p;
    }

    fclose(fp);

    return OK;
}



/*
 * Output AREAS.BBS, format short sorted list of downlink
 */
int areasbbs_print(FILE *fp)
{
    AreasBBS *p;
    
    fprintf(fp, "%s\r\n", areasbbs_1stline);
    
    for(p=areasbbs_list; p; p=p->next)
    {
	fprintf(fp, "%s %s ", p->dir, p->area);
	if(p->zone != -1)
	    fprintf(fp, "-z %d ", p->zone);
	if(p->addr.zone != -1)
	    fprintf(fp, "-a %s ", node_to_asc(&p->addr, TRUE));
	if(p->lvl != -1)
	    fprintf(fp, "-l %d ", p->lvl);
	if(p->key)
	    fprintf(fp, "-k %s ", p->key);
	lon_print_sorted(&p->nodes, fp, 1);
	fprintf(fp, "\r\n");
    }
    
    return ferror(fp);
}



/*
 * Return areasbbs_list
 */
AreasBBS *areasbbs_first(void)
{
    return areasbbs_list;
}



/*
 * Lookup area
 */
AreasBBS *areasbbs_lookup(char *area)
{
    AreasBBS *p;
    
    /*
     * FIXME: the search method should use hashing or similar
     */

    for(p=areasbbs_list; p; p=p->next)
    {
	if(area  && !strcmp(area,  p->area ))
	    return p;
    }
    
    return NULL;
}
