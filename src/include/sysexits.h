/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: sysexits.h,v 4.2 1996/12/17 17:19:54 mj Exp $
 *
 * Exit status for systems that don't have /usr/include/sysexits.h
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

/*
 * BSD error codes (used by sendmail)
 */
#define EX_OK		 0		/* successful termination */

#define EX_USAGE	64		/* command line usage error */
#define EX_DATAERR	65		/* data format error */
#define EX_NOINPUT	66		/* cannot open input */
#define EX_NOHOST	68		/* host name unknown */
#define EX_UNAVAILABLE	69		/* service unavailable */
#define EX_SOFTWARE	70		/* internal software error */
#define EX_OSERR	71		/* system error (e.g., can't fork) */
#define EX_OSFILE	72		/* critical OS file missing */
#define EX_CANTCREAT	73		/* can't create (user) output file */
#define EX_IOERR	74		/* input/output error */

/*
 * Exit codes used by FIDOGATE
 */
#define EXIT_OK		0		/* successful */
#define EXIT_ERROR	1		/* error */
#define EXIT_BUSY	2		/* program already running */
#define EXIT_CONTINUE	3		/* still work to do */

#define EXIT_KILL	32		/* killed: exit code = 32 + signum */
