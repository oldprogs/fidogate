/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: log.c,v 4.5 1997/06/28 16:26:35 mj Exp $
 *
 * Log and debug functions
 *
 *****************************************************************************
 * Copyright (C) 1990-1997
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



/*
 * Debug level -v --verbose option
 */
int verbose = 0;

/*
 * Inhibit debug output
 */
int no_debug = FALSE;


/*
 * Log file name
 */
static char logname[MAXPATH] = "";

/*
 * Log file
 */
static FILE *logfile = NULL;

/*
 * Log program
 */
static char logprog[MAXPATH] = "FIDOGATE";

/*
 * Debug file
 */
static FILE *debugfile = NULL;



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

    if(logfile)
	/* Use set FILE (stdout or stderr) */
	fp = logfile;
    else
    {
	/* Open logname[] or default */
	if(!logname[0])
	{
	    BUF_COPY3(logname, cf_logdir(), "/", LOG);
	}
	if((fp = fopen(logname, A_MODE)) == NULL)
	{
	    fprintf(stderr, "log(): can't open log file %s\n", logname);
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

    /*
     * if verbose is set, print also to LOG_OUTPUT (without date)
     */
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

    va_end(args);
}



/*
 * Debug output with debug level (verbose)
 */
void debug(int lvl, const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

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

    va_end(args);
}



/*
 * Set file name for log output
 */
void log_file(char *name)
{
    logfile   = NULL;
    debugfile = stderr;

    if(!strcmp(name, "stdout"))
    {
	logfile = debugfile = stdout;
	logname[0] = 0;
    }
    else if(!strcmp(name, "stderr"))
    {
	logfile = debugfile = stderr;
	logname[0] = 0;
    }
    else
    {
	str_expand_name(logname, sizeof(logname), name);
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
}
