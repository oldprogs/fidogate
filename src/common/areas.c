/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: areas.c,v 4.1 1996/04/22 14:31:11 mj Exp $
 *
 * Area <-> newsgroups conversion
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



/*
 * Prototypes
 */
static Area *area_build		(Area *, char *, char *);



static Area *area_list = NULL;
static Area *area_last = NULL;



/*
 * Default max. msg size
 */
static long areas_def_maxsize = MAXMSGSIZE;	/* config.h */


void areas_maxmsgsize(long int sz)
{
    areas_def_maxsize = sz;
}


long areas_get_maxmsgsize(void)
{
    return areas_def_maxsize;
}



/*
 * Read list of areas from LIBDIR/AREAS file.
 *
 * Format:
 *     AREA    NEWSGROUP    [-options]
 *
 * Options:
 *     -a Z:N/F.P       use alternate address for this area
 *     -z ZONE          use AKA for zone Z for this area
 *     -o ORIGIN        use alternate origin line for this area
 *     -d DISTRIBUTION  use Distribution: DISTRIBUTION for this newsgroup
 *     -l               only local xpostings allowed
 *     -x               no xpostings allowed
 *     -g               no messages from other gates FTN->Usenet
 *     -8               use ISO 8-bit umlauts
 *     -H               AREA/NEWSGROUP match entire hierarchy, names are
 *                      translated automatically
 *     -R LVL           ^ARFC header level
 *     -m MAXSIZE       set MaxMsgSize for this area (0 = infinity)
 *     -X "Xtra: xyz"	add extra RFC header (multiple -X are allowed)
 */
void areas_init(void)
{
    FILE *fp;
    Area *p;

    debug(14, "Reading areas file");
    
    fp = xfopen(AREAS, R_MODE_T);

    while(cf_getline(buffer, BUFFERSIZE, fp))
    {
	char *a, *g, *o;
	
	a = xstrtok(buffer, " \t");	/* FTN area */
	g = xstrtok(NULL,   " \t");	/* Newsgroup */
	if(a==NULL || g==NULL)
	    continue;
	
	p = (Area *)xmalloc(sizeof(Area));
	p->next         = NULL;
	p->area         = strsave(a);
	p->group        = strsave(g);
	p->zone         = cf_defzone();
	node_invalid(&p->addr);
	p->origin       = NULL;
	p->distribution = NULL;
	p->flags        = 0;
	p->rfc_lvl      = 0;
	p->maxsize      = -1;
	tl_init(&p->x_hdr);
	
	for(o=xstrtok(NULL, " \t");	/* Options */
	    o;
	    o=xstrtok(NULL, " \t")  )
	{
	    if(!strcmp(o, "-a"))
		/* -a Z:N/F.P */
		if((o = xstrtok(NULL, " \t")))
		    asc_to_node(o, &p->addr, FALSE);
	    if(!strcmp(o, "-z"))
		/* -z ZONE */
		if((o = xstrtok(NULL, " \t")))
		    p->zone = atoi(o);
	    if(!strcmp(o, "-o"))
		/* -o ORIGIN */
		if((o = xstrtok(NULL, " \t")))
		    p->origin = strsave(o);
	    if(!strcmp(o, "-d"))
		/* -d DISTRIBUTION */
		if((o = xstrtok(NULL, " \t")))
		    p->distribution = strsave(o);
	    if(!strcmp(o, "-l"))
		p->flags |= AREA_LOCALXPOST;
	    if(!strcmp(o, "-x"))
		p->flags |= AREA_NOXPOST;
	    if(!strcmp(o, "-g"))
		p->flags |= AREA_NOGATE;
	    if(!strcmp(o, "-8"))
		p->flags |= AREA_8BIT;
	    if(!strcmp(o, "-H"))
		p->flags |= AREA_HIERARCHY;
	    if(!strcmp(o, "-!"))
		p->flags |= AREA_NO;
	    if(!strcmp(o, "-R"))
		/* -R lvl */
		if((o = xstrtok(NULL, " \t")))
		    p->rfc_lvl = atoi(o);
	    if(!strcmp(o, "-m"))
		/* -m MAXMSGSIZE */
		if((o = xstrtok(NULL, " \t")))
		    p->maxsize = atoi(o);
	    if(!strcmp(o, "-X"))
		/* -X "Xtra: xyz" */
		if((o = xstrtok(NULL, " \t")))
		    tl_append(&p->x_hdr, o);
	}
	if(p->maxsize < 0)
	    /* Not set or error */
	    p->maxsize = areas_def_maxsize;
	
	debug(15, "areas: %s %s Z=%d A=%s R=%d S=%ld",
	      p->area, p->group, p->zone,
	      p->addr.zone!=-1 ? node_to_asc(&p->addr, TRUE) : "",
	      p->rfc_lvl, p->maxsize                               );
	
	/*
	 * Put into linked list
	 */
	if(area_list)
	    area_last->next = p;
	else
	    area_list       = p;
	area_last       = p;
    }
    
    fclose(fp);
}



/*
 * Lookup area/newsgroup in area_list
 *
 * Parameters:
 *     area, NULL     --- lookup by area
 *     NULL, group    --- lookup by group
 */
Area *areas_lookup(char *area, char *group)
{
    Area *p;
    
    /*
     * Inefficient search, but order is important!
     */
    for(p=area_list; p; p=p->next)
    {
	if(area && area[0]==p->area[0])
	    if(p->flags & AREA_HIERARCHY)
	    {
		if(!strncmp(area, p->area, strlen(p->area)))
		    return p->flags & AREA_NO ? NULL
			: area_build(p, area, group);
	    }
	    else
	    {
		if(!strcmp(area,  p->area ))
		    return p->flags & AREA_NO ? NULL : p;
	    }
	
	if(group && group[0]==p->group[0])
	    if(p->flags & AREA_HIERARCHY)
	    {
		if(!strncmp(group, p->group, strlen(p->group)))
		    return p->flags & AREA_NO ? NULL
			: area_build(p, area, group);
	    }
	    else
	    {
		if(!strcmp(group, p->group))
		    return p->flags & AREA_NO ? NULL : p;
	    }
    }
    
    return NULL;
}



/*
 * Build area/newsgroup name from hierarchy matching pattern
 */
static Area *area_build(Area *pa, char *area, char *group)
{
    static char bufa[MAXPATH], bufg[MAXPATH];
    static Area ret;
    char *p, *q, *end;
    
    *bufa = *bufg = 0;
    
    ret.next         = NULL;
    ret.area         = bufa;
    ret.group        = bufg;
    ret.zone         = pa->zone;
    ret.addr         = pa->addr;
    ret.origin       = pa->origin;
    ret.distribution = pa->distribution;
    ret.flags        = pa->flags;
    ret.rfc_lvl      = pa->rfc_lvl;
    ret.maxsize      = pa->maxsize;
    ret.x_hdr        = pa->x_hdr;
    
    /* AREA -> Newsgroup */
    if(area)				/* Was searching for area */
    {
	strncpy0(bufa, area, sizeof(bufa));
	strncpy0(bufg, pa->group, sizeof(bufg));
	p   = bufg + strlen(bufg);
	end = bufg + sizeof(bufg) - 1;
	q   = area + strlen(pa->area);
	
	for(; *q && p<end; q++, p++)
	    switch(*q)
	    {
	    case '_':
		*p = '-';
		break;
	    default:
		*p = tolower(*q);
		break;
	    }
	*p = 0;
    }

    /* Newsgroup -> AREA */
    if(group)				/* Was searching for newsgroup */
    {
	strncpy0(bufa, pa->area, sizeof(bufa));
	strncpy0(bufg, group, sizeof(bufg));
	p   = bufa + strlen(bufa);
	end = bufa + sizeof(bufa) - 1;
	q   = group + strlen(pa->group);
	
	for(; *q && p<end; q++, p++)
	    switch(*q)
	    {
	    case '-':
		*p = '_';
		break;
	    default:
		*p = toupper(*q);
		break;
	    }
	*p = 0;
    }
    
    return &ret;
}
