/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: date.c,v 4.3 1998/01/18 09:47:46 mj Exp $
 *
 * date() date/time print function
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



#define DST_OFFSET	1

static char *get_tz_name (struct tm *);



/*
 * Get name of current time zone
 */
static char *get_tz_name(struct tm *tm)
{
#ifdef HAS_STRFTIME
    static char buf[32];
    
    strftime(buf, sizeof(buf), "%Z", tm);
    return buf;
#endif

#ifdef HAS_TM_ZONE
    return tm->tm_zone;
#endif

#ifdef HAS_TZNAME
    return tm->tm_isdst > 0 ? tzname[1] : tzname[0];
#endif
}
    


/*
 * Format date/time according to strftime() format string
 */
char *date(char *fmt, time_t *t)
{
    static char buf[128];

    return date_buf(buf, fmt, t);
}


char *date_buf(char *buf, char *fmt, time_t *t)
{
    TIMEINFO ti;
    struct tm *tm;

    /* names for weekdays */
    static char *weekdays[] = {
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat",
    };
    /* names for months */
    static char *months[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
    };
    char *p = buf;

    /* Check for invalid time (-1) */
    if(fmt==NULL && t && *t==-1)
	return "INVALID";
    
    GetTimeInfo(&ti);
    tm = localtime(&ti.time);
    if(tm->tm_isdst)
	ti.tzone += DST_OFFSET * 60;

    if(t)
	ti.time = *t;
    tm = localtime(&ti.time);
    if(tm->tm_isdst)
	ti.tzone -= DST_OFFSET * 60;

    /* Default format string */
    if(!fmt)
	fmt = DATE_DEFAULT;

    /*
     * Partial strftime() format implementation with additional
     *   %O    time difference to UTC, format [+-]hhmm,
     *         e.g. +0100 for MET, +0200 for MET DST
     */
    *p = 0;
    while(*fmt)
    {
	if(*fmt == '%')
	{
	    fmt++;
	    switch (*fmt)
	    {
	    case 'a':					/* Abbr. weekday */
		strcpy(p, weekdays[tm->tm_wday]); break;
	    /* A not implemented */
	    case 'b':					/* Abbr. month */
		strcpy(p, months[tm->tm_mon]); break;
	    /* B not implemented */
	    /* c not implemented */
	    case 'd':					/* Day of month */
		sprintf(p, "%02d", tm->tm_mday); break;
	    case 'H':					/* Hour (24h) */
		sprintf(p, "%02d", tm->tm_hour); break;
	    /* I not implemented */
	    case 'j':					/* Day of year */
		sprintf(p, "%03d", tm->tm_yday); break;
	    case 'm':					/* Month */
		sprintf(p, "%02d", tm->tm_mon + 1); break;
	    case 'M':					/* Minutes */
		sprintf(p, "%02d", tm->tm_min); break;
	    /* p not implemented */
	    case 'S':					/* Seconds */
		sprintf(p, "%02d", tm->tm_sec); break;
	    /* U not implemented */
	    case 'w':					/* Day of week */
		sprintf(p, "%d", tm->tm_wday); break;
	    /* W not implemented */
	    case 'x':					/* Date */
		sprintf(p, "%s %2d %4d",
			months[tm->tm_mon],
			tm->tm_mday, tm->tm_year+1900); break;
	    case 'X':					/* Time */
		sprintf(p, "%02d:%02d:%02d",
			tm->tm_hour, tm->tm_min, tm->tm_sec); break;
	    case 'y':					/* Year 00-99 */
		sprintf(p, "%02d", (tm->tm_year % 100) ); break;
	    case 'Y':					/* Year 1900 ... */
		sprintf(p, "%4d", 1900 + tm->tm_year); break;
	    case 'Z':					/* Time zone */
		strcpy(p, get_tz_name(tm)); break;
		
	    /***** Additional %O format *****/
	    case 'O':					/* Time diff to UTC */
	    {
		int hour, min, off;
		char cc;
		
		off  = - ti.tzone;
		cc   = off>=0 ? '+' : '-';
		off  = off<0 ? -off : off;
		hour = off / 60;
		min  = off % 60;
		
		sprintf(p, "%c%02d%02d", cc, hour, min);
	    }
		break;
		
		
	    default:
		*p++ = *fmt;
		*p   = 0;
		break;
	    }

	    fmt++;
	    /* Advance buffer pointer */
	    while(*p)
		p++;
	}
	else 
	{
	    *p++ = *fmt++;
	    *p   = 0;
	}
    }

    return buf;
}
