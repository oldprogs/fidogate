/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX <-> FIDO
 *
 * $Id: fidogate.h,v 4.11 2001/05/28 18:00:28 mj Exp $
 *
 * Common header file
 *
 *****************************************************************************
 * Copyright (C) 1990-2001
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

/*
 * Configuration header files
 */
#include "config.h"
#include "paths.h"		    /* Automatically generated by Makefile */


/*
 * heavy includin' ...
 */

/***** System *****/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#ifdef OS2
# include <io.h>
# include <process.h>
#endif
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <dirent.h>

#ifdef HAS_SYSEXITS_H

# include <sysexits.h>			/* EX_* defines */

#else /**!HAS_SYSEXITS_H**/

/* BSD error codes (used by sendmail */
# define EX_OK		 0		/* successful termination */

# define EX_USAGE	64		/* command line usage error */
# define EX_DATAERR	65		/* data format error */
# define EX_NOINPUT	66		/* cannot open input */
# define EX_NOHOST	68		/* host name unknown */
# define EX_UNAVAILABLE	69		/* service unavailable */
# define EX_SOFTWARE	70		/* internal software error */
# define EX_OSERR	71		/* system error (e.g., can't fork) */
# define EX_OSFILE	72		/* critical OS file missing */
# define EX_CANTCREAT	73		/* can't create (user) output file */
# define EX_IOERR	74		/* input/output error */

#endif /**HAS_SYSEXITS_H**/



/*
 * Exit codes used by FIDOGATE
 */
#define EXIT_OK		0		/* successful */
#define EXIT_ERROR	1		/* error */
#define EXIT_BUSY	2		/* program already running */
#define EXIT_CONTINUE	3		/* still work to do */

#define EXIT_KILL	32		/* killed: exit code = 32 + signum */



/*
 * Values
 */
#define TRUE		1
#define FALSE		0
#define FIRST		1
#define NEXT		0
#define OK		0
#define ERROR		(-1)
#define EMPTY		(-1)
#define INVALID		(-1)
#define WILDCARD	(-2)

/*
 * We supply our own version of the toupper()/tolower()
 * macros, because the exact behaviour of those in
 * <ctype.h> varies among systems.
 */
#undef _toupper
#undef _tolower
#undef toupper
#undef tolower

#define _toupper(c) ((c)-'a'+'A')
#define _tolower(c) ((c)-'A'+'a')
#define toupper(c)  (islower(c) ? _toupper(c) : (c))
#define tolower(c)  (isupper(c) ? _tolower(c) : (c))


/*
 * Function declarations for systems like SUNOS, where the standard
 * header files don't have them.
 */
#include "declare.h"

/*
 * Data types
 */
#include "node.h"			/* Node, LON definitions */
#include "packet.h"			/* Packet, Message definitions */
#include "structs.h"

/*
 * Function prototypes
 */
#include "prototypes.h"
#include "cf_funcs.h"
