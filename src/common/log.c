/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: log.c,v 4.10 1998/01/18 15:33:08 mj Exp $
 *
 * Log and debug functions
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



/* Debug level -v --verbose option */
int verbose = 0;

/* Inhibit debug output */
int no_debug = FALSE;

/* Log file name */
static char logname[MAXPATH] = "";

/* Log file */
static FILE *logfile = NULL;

/* Log program */
static char logprog[MAXPATH] = "FIDOGATE";

/* Debug file */
static FILE *debugfile = NULL;

/* Syslog support */
#ifdef HAS_SYSLOG
static int use_syslog   = FALSE;
static int must_openlog = TRUE;

#include <syslog.h>
#define FACILITY	LOG_LOCAL0
#endif



#ifndef HAS_STRERROR
/*
 * strerror()  ---  get string from sys_errlist[]
 */
char *strerror(int errnum)
{
#ifndef OS2
    extern int sys_nerr;
# ifndef __FreeBSD__
    extern char *sys_errlist[];
# endif
#endif

    if (errnum > 0 && errnum < sys_nerr)
	return sys_errlist[errnum];
    return "unknown error";
}
#endif /**HAS_STRERROR**/



/*
 * Log to logfile.
 *
 * If first character in format string is '$', output errno and error
 * string too.
 */
void log(const char *fmt, ...)
{
    va_list args;
    FILE *fp;
    char buf[32];
    
    va_start(args, fmt);

    /* Set logfile name if called 1st time */
    if(!logname[0])
    {
	log_file(cf_p_logfile());
    }
    
#ifdef HAS_SYSLOG
    if(use_syslog) {
	if(must_openlog) {
	    openlog(logprog, LOG_PID, FACILITY);
	    must_openlog = FALSE;
	}
	vsyslog(LOG_NOTICE, *fmt == '$' ? fmt + 1 : fmt, args);
	if (*fmt == '$')
	    syslog(LOG_NOTICE, "  (errno=%d: %m)", errno);
	
    }
    else {
#endif	
	if(logfile)
	    /* Use set FILE (stdout or stderr) */
	    fp = logfile;
	else
	{
	    /* Open logname[] or default */
	    if((fp = fopen(logname, A_MODE)) == NULL)
	    {
		fprintf(stderr,
			"%s WARNING: can't open log file %s  (errno=%d: %s)\n",
			logprog, logname, errno, strerror(errno)             );
		if(!verbose)
		    verbose = -1;
	    }
	}
	if(fp)
	{
	    fprintf(fp, "%s %s ",
		    date_buf(buf, "%b %d %H:%M:%S", (long *)0), logprog);
	    vfprintf(fp, *fmt == '$' ? fmt + 1 : fmt, args);
	    if (*fmt == '$')
		fprintf(fp, "  (errno=%d: %s)", errno, strerror(errno));
	    fprintf(fp, "\n");
	    if(logfile == NULL)
		fclose(fp);
	}

	/* if verbose is set, print also to debugfile (without date) */
	if (verbose)
	{
	    if(!debugfile)
		debugfile = stderr;

	    fprintf(debugfile, "%s ", logprog);
	    vfprintf(debugfile, *fmt == '$' ? fmt + 1 : fmt, args);
	    if (*fmt == '$')
		fprintf(debugfile, "  (errno=%d: %s)", errno, strerror(errno));
	    fprintf(debugfile, "\n");
	    fflush(debugfile);
	}
#ifdef HAS_SYSLOG
    }
#endif	

    va_end(args);
}



/*
 * Debug output with debug level (verbose)
 */
void debug(int lvl, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

#ifdef HAS_SYSLOG
    if(use_syslog) {
	if(verbose >= lvl)
	{
	    if(no_debug)
	    {
		syslog(LOG_DEBUG,
		       "debug called for uid=%d euid=%d, output disabled\n",
		       (int)getuid(), (int)geteuid());
	    }
	    else
	    {
		if(must_openlog) {
		    openlog(logprog, LOG_PID, FACILITY);
		    must_openlog = FALSE;
		}
		vsyslog(LOG_DEBUG, fmt, args);
	    }
	}
	
    }
    else {
#endif	
	if(!debugfile)
	    debugfile = stderr;

	if(verbose >= lvl)
	{
	    if(no_debug)
	    {
		fprintf(debugfile,
			"debug called for uid=%d euid=%d, output disabled\n",
			(int)getuid(), (int)geteuid());
	    }
	    else
	    {
		vfprintf(debugfile, fmt, args);
		fprintf(debugfile, "\n");
		fflush(debugfile);
	    }
	}
#ifdef HAS_SYSLOG
    }
#endif	

    va_end(args);
}



/*
 * Set file name for log output
 */
void log_file(char *name)
{
    logfile   = NULL;
    debugfile = stderr;

    if(streq(name, "stdout"))
    {
	use_syslog = FALSE;
	logfile = debugfile = stdout;
	logname[0] = 0;
    }
    else if(streq(name, "stderr"))
    {
	use_syslog = FALSE;
	logfile = debugfile = stderr;
	logname[0] = 0;
    }
#ifdef HAS_SYSLOG
    else if(streq(name, "syslog"))
    {
	use_syslog = TRUE;
	logfile = debugfile = NULL;
	logname[0] = 0;
    }
#endif
    else
    {
	use_syslog = FALSE;
	BUF_EXPAND(logname, name);
    }
}



/*
 * Set program name for log output
 */
void log_program(char *name)
{
    char *p;
    
    BUF_COPY(logprog, name);

    if( (p = getenv("LOGFILE")) )
       log_file(p);
    if( (p = getenv("FIDOGATE_LOGFILE")) )
       log_file(p);
}
