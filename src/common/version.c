/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: version.c,v 4.0 1996/04/17 18:17:40 mj Exp $
 *
 * FIDOGATE version number handling stuff
 *
 *****************************************************************************
 * This version hacked and maintained by:
 *    _____ _____
 *   |	   |___	 |   Martin Junius	     FIDO:    2:2452/110	  2:242/6.0
 *   | | | |   | |   Republikplatz 3	     DOMAIN:  mj@dfv.rwth-aachen.de
 *   |_|_|_|_____|   W-5100 Aachen, Germany
 *
 *****************************************************************************/

#include "fidogate.h"

#include "version.h"
#include "patchlevel.h"



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
