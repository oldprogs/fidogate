/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: gettime.c,v 4.0 1996/04/17 18:17:39 mj Exp $
 *
 * Get system date/time. Taken from ifmail 1.7 / inn 1.4 and adopted
 * for FIDOGATE .
 *
 *****************************************************************************/
/*  Original %Revision: 1.4 %
**
*/

#include "fidogate.h"

#include <sys/time.h>

#ifdef DO_HAVE_TM_GMTOFF
# undef  DONT_HAVE_TM_GMTOFF
#else
# define DONT_HAVE_TM_GMTOFF
#endif




int
GetTimeInfo(TIMEINFO *Now)
{
    static time_t	LastTime;
    static long		LastTzone;
    struct tm		*tm;
#if	defined(DO_HAVE_GETTIMEOFDAY)
    struct timeval	tv;
#endif	/* defined(DO_HAVE_GETTIMEOFDAY) */
#if	defined(DONT_HAVE_TM_GMTOFF)
    struct tm		local;
    struct tm		gmt;
#endif	/* !defined(DONT_HAVE_TM_GMTOFF) */

    /* Get the basic time. */
#if	defined(DO_HAVE_GETTIMEOFDAY)
    if (gettimeofday(&tv, (struct timezone *)NULL) == -1)
	return -1;
    Now->time = tv.tv_sec;
    Now->usec = tv.tv_usec;
#else
    /* Can't check for -1 since that might be a time, I guess. */
    (void)time(&Now->time);
    Now->usec = 0;
#endif	/* defined(DO_HAVE_GETTIMEOFDAY) */

    /* Now get the timezone if it's been an hour since the last time. */
    if (Now->time - LastTime > 60 * 60) {
	LastTime = Now->time;
	if ((tm = localtime(&Now->time)) == NULL)
	    return -1;
#if	defined(DONT_HAVE_TM_GMTOFF)
	/* To get the timezone, compare localtime with GMT. */
	local = *tm;
	if ((tm = gmtime(&Now->time)) == NULL)
	    return -1;
	gmt = *tm;

	/* Assume we are never more than 24 hours away. */
	LastTzone = gmt.tm_yday - local.tm_yday;
	if (LastTzone > 1)
	    LastTzone = -24;
	else if (LastTzone < -1)
	    LastTzone = 24;
	else
	    LastTzone *= 24;

	/* Scale in the hours and minutes; ignore seconds. */
	LastTzone += gmt.tm_hour - local.tm_hour;
	LastTzone *= 60;
	LastTzone += gmt.tm_min - local.tm_min;
#else
	LastTzone =  (0 - tm->tm_gmtoff) / 60;
#endif	/* defined(DONT_HAVE_TM_GMTOFF) */
    }
    Now->tzone = LastTzone;
    return 0;
}
