/* Automatically generated by subst.pl --- DO NOT EDIT!!! */

/*
 * FIDOGATE
 *
 * $Id: cf_funcs.mak,v 4.19 2001/10/21 20:17:22 mj Exp $
 */

#include "fidogate.h"

/***** HOSTS *****/
static char *cf_p_s_hosts(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("HOSTS", TRUE)) )
	    pval = DEFAULT_HOSTS;
	debug(8, "config: HOSTS %s", pval);
    }
    return pval;
}
char *cf_p_hosts(void)
{
    return cf_p_s_hosts(NULL);
}
char *cf_s_hosts(char *s)
{
    return cf_p_s_hosts(s);
}

/***** ALIASES *****/
static char *cf_p_s_aliases(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("ALIASES", TRUE)) )
	    pval = DEFAULT_ALIASES;
	debug(8, "config: ALIASES %s", pval);
    }
    return pval;
}
char *cf_p_aliases(void)
{
    return cf_p_s_aliases(NULL);
}
char *cf_s_aliases(char *s)
{
    return cf_p_s_aliases(s);
}

/***** CONFIGDIR *****/
static char *cf_p_s_configdir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("CONFIGDIR", TRUE)) )
	    pval = DEFAULT_CONFIGDIR;
	debug(8, "config: CONFIGDIR %s", pval);
    }
    return pval;
}
char *cf_p_configdir(void)
{
    return cf_p_s_configdir(NULL);
}
char *cf_s_configdir(char *s)
{
    return cf_p_s_configdir(s);
}

/***** INBOUND *****/
static char *cf_p_s_inbound(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("INBOUND", TRUE)) )
	    pval = DEFAULT_INBOUND;
	debug(8, "config: INBOUND %s", pval);
    }
    return pval;
}
char *cf_p_inbound(void)
{
    return cf_p_s_inbound(NULL);
}
char *cf_s_inbound(char *s)
{
    return cf_p_s_inbound(s);
}

/***** PINBOUND *****/
static char *cf_p_s_pinbound(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("PINBOUND", TRUE)) )
	    pval = DEFAULT_PINBOUND;
	debug(8, "config: PINBOUND %s", pval);
    }
    return pval;
}
char *cf_p_pinbound(void)
{
    return cf_p_s_pinbound(NULL);
}
char *cf_s_pinbound(char *s)
{
    return cf_p_s_pinbound(s);
}

/***** LOGDIR *****/
static char *cf_p_s_logdir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("LOGDIR", TRUE)) )
	    pval = DEFAULT_LOGDIR;
	debug(8, "config: LOGDIR %s", pval);
    }
    return pval;
}
char *cf_p_logdir(void)
{
    return cf_p_s_logdir(NULL);
}
char *cf_s_logdir(char *s)
{
    return cf_p_s_logdir(s);
}

/***** BINDIR *****/
static char *cf_p_s_bindir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("BINDIR", TRUE)) )
	    pval = DEFAULT_BINDIR;
	debug(8, "config: BINDIR %s", pval);
    }
    return pval;
}
char *cf_p_bindir(void)
{
    return cf_p_s_bindir(NULL);
}
char *cf_s_bindir(char *s)
{
    return cf_p_s_bindir(s);
}

/***** CHARSETMAP *****/
static char *cf_p_s_charsetmap(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("CHARSETMAP", TRUE)) )
	    pval = DEFAULT_CHARSETMAP;
	debug(8, "config: CHARSETMAP %s", pval);
    }
    return pval;
}
char *cf_p_charsetmap(void)
{
    return cf_p_s_charsetmap(NULL);
}
char *cf_s_charsetmap(char *s)
{
    return cf_p_s_charsetmap(s);
}

/***** HISTORY *****/
static char *cf_p_s_history(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("HISTORY", TRUE)) )
	    pval = DEFAULT_HISTORY;
	debug(8, "config: HISTORY %s", pval);
    }
    return pval;
}
char *cf_p_history(void)
{
    return cf_p_s_history(NULL);
}
char *cf_s_history(char *s)
{
    return cf_p_s_history(s);
}

/***** LIBDIR *****/
static char *cf_p_s_libdir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("LIBDIR", TRUE)) )
	    pval = DEFAULT_LIBDIR;
	debug(8, "config: LIBDIR %s", pval);
    }
    return pval;
}
char *cf_p_libdir(void)
{
    return cf_p_s_libdir(NULL);
}
char *cf_s_libdir(char *s)
{
    return cf_p_s_libdir(s);
}

/***** ACL *****/
static char *cf_p_s_acl(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("ACL", TRUE)) )
	    pval = DEFAULT_ACL;
	debug(8, "config: ACL %s", pval);
    }
    return pval;
}
char *cf_p_acl(void)
{
    return cf_p_s_acl(NULL);
}
char *cf_s_acl(char *s)
{
    return cf_p_s_acl(s);
}

/***** SPOOLDIR *****/
static char *cf_p_s_spooldir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("SPOOLDIR", TRUE)) )
	    pval = DEFAULT_SPOOLDIR;
	debug(8, "config: SPOOLDIR %s", pval);
    }
    return pval;
}
char *cf_p_spooldir(void)
{
    return cf_p_s_spooldir(NULL);
}
char *cf_s_spooldir(char *s)
{
    return cf_p_s_spooldir(s);
}

/***** ROUTING *****/
static char *cf_p_s_routing(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("ROUTING", TRUE)) )
	    pval = DEFAULT_ROUTING;
	debug(8, "config: ROUTING %s", pval);
    }
    return pval;
}
char *cf_p_routing(void)
{
    return cf_p_s_routing(NULL);
}
char *cf_s_routing(char *s)
{
    return cf_p_s_routing(s);
}

/***** LOCKDIR *****/
static char *cf_p_s_lockdir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("LOCKDIR", TRUE)) )
	    pval = DEFAULT_LOCKDIR;
	debug(8, "config: LOCKDIR %s", pval);
    }
    return pval;
}
char *cf_p_lockdir(void)
{
    return cf_p_s_lockdir(NULL);
}
char *cf_s_lockdir(char *s)
{
    return cf_p_s_lockdir(s);
}

/***** PASSWD *****/
static char *cf_p_s_passwd(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("PASSWD", TRUE)) )
	    pval = DEFAULT_PASSWD;
	debug(8, "config: PASSWD %s", pval);
    }
    return pval;
}
char *cf_p_passwd(void)
{
    return cf_p_s_passwd(NULL);
}
char *cf_s_passwd(char *s)
{
    return cf_p_s_passwd(s);
}

/***** VARDIR *****/
static char *cf_p_s_vardir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("VARDIR", TRUE)) )
	    pval = DEFAULT_VARDIR;
	debug(8, "config: VARDIR %s", pval);
    }
    return pval;
}
char *cf_p_vardir(void)
{
    return cf_p_s_vardir(NULL);
}
char *cf_s_vardir(char *s)
{
    return cf_p_s_vardir(s);
}

/***** LOGFILE *****/
static char *cf_p_s_logfile(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("LOGFILE", TRUE)) )
	    pval = DEFAULT_LOGFILE;
	debug(8, "config: LOGFILE %s", pval);
    }
    return pval;
}
char *cf_p_logfile(void)
{
    return cf_p_s_logfile(NULL);
}
char *cf_s_logfile(char *s)
{
    return cf_p_s_logfile(s);
}

/***** UUINBOUND *****/
static char *cf_p_s_uuinbound(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("UUINBOUND", TRUE)) )
	    pval = DEFAULT_UUINBOUND;
	debug(8, "config: UUINBOUND %s", pval);
    }
    return pval;
}
char *cf_p_uuinbound(void)
{
    return cf_p_s_uuinbound(NULL);
}
char *cf_s_uuinbound(char *s)
{
    return cf_p_s_uuinbound(s);
}

/***** AREAS *****/
static char *cf_p_s_areas(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("AREAS", TRUE)) )
	    pval = DEFAULT_AREAS;
	debug(8, "config: AREAS %s", pval);
    }
    return pval;
}
char *cf_p_areas(void)
{
    return cf_p_s_areas(NULL);
}
char *cf_s_areas(char *s)
{
    return cf_p_s_areas(s);
}

/***** BTBASEDIR *****/
static char *cf_p_s_btbasedir(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("BTBASEDIR", TRUE)) )
	    pval = DEFAULT_BTBASEDIR;
	debug(8, "config: BTBASEDIR %s", pval);
    }
    return pval;
}
char *cf_p_btbasedir(void)
{
    return cf_p_s_btbasedir(NULL);
}
char *cf_s_btbasedir(char *s)
{
    return cf_p_s_btbasedir(s);
}

/***** PACKING *****/
static char *cf_p_s_packing(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("PACKING", TRUE)) )
	    pval = DEFAULT_PACKING;
	debug(8, "config: PACKING %s", pval);
    }
    return pval;
}
char *cf_p_packing(void)
{
    return cf_p_s_packing(NULL);
}
char *cf_s_packing(char *s)
{
    return cf_p_s_packing(s);
}

/***** FTPINBOUND *****/
static char *cf_p_s_ftpinbound(char *s)
{
    static char *pval = NULL;
    if(s)
        pval = strsave(s);
    if(! pval)
    {
	if( ! (pval = cf_get_string("FTPINBOUND", TRUE)) )
	    pval = DEFAULT_FTPINBOUND;
	debug(8, "config: FTPINBOUND %s", pval);
    }
    return pval;
}
char *cf_p_ftpinbound(void)
{
    return cf_p_s_ftpinbound(NULL);
}
char *cf_s_ftpinbound(char *s)
{
    return cf_p_s_ftpinbound(s);
}

