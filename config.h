/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway software UNIX <-> FIDO
 *
 * $Id: config.h,v 4.7 1996/08/26 19:10:26 mj Exp $
 *
 * Configuration header file
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

/***** General configuration *************************************************/

/*
 * Generate local FTN addresses, e.g.
 *     user_name%p.f.n.z@host.domain
 * instead of
 *     user_name@p.f.n.z.domain
 */
/* #define LOCAL_FTN_ADDRESSES */

/*
 * Create Binkley-style BSY files for all outbound operations
 */
#define DO_BSY_FILES

/*
 * Secure permissions
 */
#define SECURE

/*
 * Default max. message size for FIDO. Due to some more brain damage
 * in FIDONET programs we have to split larger messages into several
 * smaller ones. May be set with the -M option in AREAS or MaxMsgSize
 * in CONFIG_GATE.
 */
/* < 16 K */
#define MAXMSGSIZE	14000
/* < 32 K */
/* #define MAXMSGSIZE	30000 */

/*
 * Domain for invalid FTN addresses
 */
#define FTN_INVALID_DOMAIN "INVALID_FTN_ADDRESS"



/***** ftn2rfc configuration ************************************************/

/*
 * Rewrite addresses found in ALIASES so that the sender's address is the
 * gateway address. The reverse direction requires suitable MTA aliases.
 */
/* #define ALIASES_ARE_LOCAL */

/*
 * Make the IBMPC char set the default (no ^ACHRS kludge)
 */
#define CHARSET_DEFAULT_IBMPC

/*
 * Use an 8bit char set translation table.
 * USE WITH CARE!!! ftn2rfc doesn't generate proper MIME header yet and
 * will simply put the 8bit chars into the output messages.
 */
/* #define CHARSET_8BIT */



/***** rfc2ftn configuration ************************************************/

/** Passthru operation for NetMail: FIDO->Internet->FIDO **/
#define PASSTHRU_NETMAIL
/** Passthru operation for EchoMail: FIDO->Internet->FIDO **/
#define PASSTHRU_ECHOMAIL /* Not yet implemented */

/*
 * Always generate a ^AINTL kludge for NetMails. (undef: only for mails
 * to other zones)
 */
#define FTN_FORCE_INTL

/*
 * Don't pass news control messages to FTN
 */
#define NO_CONTROL



/***** System dependend configuration ***************************************
 *
 *   DO_HAVE_GETTIMEOFDAY	Do you have gettimeofday()?
 *
 *   DO_HAVE_TM_GMTOFF		Does your (struct tm) have a tm_gmtoff field?
 *
 *
 * Define only one of DO_HAVE_TM_ZONE, DO_HAVE_STRFTIME, DO_HAVE_TZNAME!!!
 *
 *   DO_HAVE_TM_ZONE		Does your (struct tm) have a tm_zone field?
 *
 *   DO_HAVE_STRFTIME		Do you have strftime()?
 *
 *   DO_HAVE_TZNAME		Do you have extern char *tzname[2]?
 *
 *
 * Define only one of DO_HAVE_STRCASECMP, DO_HAVE_STRICMP!!!
 *
 *   DO_HAVE_STRCASECMP		Do you have strcasecmp(), strncasecmp()?
 *
 *   DO_HAVE_STRICMP		Do you have stricmp(), strnicmp()?
 *
 *
 *   DO_BINARY			Open files in binary mode
 */

#ifdef __sun__				/* SUNOS 4.1.x, GNU gcc */
# define DO_HAVE_GETTIMEOFDAY
# define DO_HAVE_TM_GMTOFF
# undef  DO_HAVE_TM_ZONE
# define DO_HAVE_STRFTIME
# undef  DO_HAVE_TZNAME
# define DO_HAVE_STRCASECMP
# undef  DO_HAVE_STRICMP
# undef  DO_BINARY
#endif

#ifdef __linux__			/* LINUX 1.x, GNU gcc */
# define DO_HAVE_GETTIMEOFDAY
# undef  DO_HAVE_TM_GMTOFF
# undef  DO_HAVE_TM_ZONE
# define DO_HAVE_STRFTIME
# undef  DO_HAVE_TZNAME
# define DO_HAVE_STRCASECMP
# undef  DO_HAVE_STRICMP
# undef  DO_BINARY
#endif

#ifdef ISC				/* ISC 3.x, GNU gcc, -DISC necessary */
# define DO_HAVE_GETTIMEOFDAY
# undef  DO_HAVE_TM_GMTOFF
# undef  DO_HAVE_TM_ZONE
# undef  DO_HAVE_STRFTIME
# define DO_HAVE_TZNAME
# undef  DO_HAVE_STRCASECMP		/* ? */
# undef  DO_HAVE_STRICMP
# undef  DO_BINARY
#endif

#ifdef MSDOS				/* MSDOS, DJGPP GNU gcc */
# define DO_HAVE_TM_GMTOFF
# undef  DO_HAVE_TM_ZONE
# define DO_HAVE_GETTIMEOFDAY
# define DO_HAVE_STRFTIME
# undef  DO_HAVE_STRCASECMP
# define DO_HAVE_STRICMP
# define DO_BINARY
#endif

#ifdef __EMX__				/* OS/2, EMX GNU gcc */
# ifndef OS2
#  define OS2
# endif
#endif
#ifdef OS2
# define DO_HAVE_GETTIMEOFDAY
# define DO_HAVE_STRFTIME
# undef  DO_HAVE_STRCASECMP
# define DO_HAVE_STRICMP
# define DO_BINARY
#endif


/***** End of configuration *************************************************/



/***** ^AMSGID/Message-ID configuration *************************************/

/*
 * Standard FIDONET domain for Z1-6 Message-IDs
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * !!! DON'T TOUCH THIS, IF YOU'RE NOT ABSOLUTELY SURE WHAT YOU'RE DOING !!!
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define MSGID_FIDONET_DOMAIN	".fidonet.org"

/*
 * Create Gatebau '93 ^AORIGID and ^AORIGREF:
 *     ^AORIGID: <id@do.main> [i/n]
 *     ^AORIGREF: <id@do.main>
 */
/* #define MSGID_ORIGID */



/****************************************************************************
 *                                                                          *
 *              Think twice before changing anything below!!!               *
 *                                                                          *
 ****************************************************************************/

/*
 * Config files
 */
#define CONFIG_GATE	"%L/config.gate"/* FIDOGATE gateway config file */
#define CONFIG_MAIN	"%L/config.main"/* FIDOGATE main AKA config file */
#define CONFIG_FFX	"%L/config.ffx"	/* FIDOGATE ffx progs config file */

/* Default values, if not set in config */
#define DEF_ALIASES	"%L/aliases"	/* Gateway user aliases config file */
#define DEF_AREAS	"%L/areas"	/* Gateway Areas <-> Newsgroups */
#define DEF_HOSTS	"%L/hosts"	/* Gateway registered host file */
#define DEF_PASSWD	"%L/passwd"	/* Password config file */
#define DEF_PACKING	"%L/packing"	/* Tosser packing config file */
#define DEF_ROUTING	"%L/routing"	/* Tosser routing config file */

/* Values retrieved from config */
#define ALIASES		cf_p_aliases()
#define AREAS		cf_p_areas()
#define HOSTS		cf_p_hosts()
#define PASSWD		cf_p_passwd()
#define PACKING		cf_p_packing()
#define ROUTING		cf_p_routing()

#define HISTORY		"history"	/* MSGID history in SPOOLDIR */

#define LOG		"log"		/* Default log file */

/* Sequencer files */
#define SEQ_MAIL	"seq.mail"	/* ftn2rfc: mail files */
#define SEQ_NEWS	"seq.news"	/* ftn2rfc: news files */
#define SEQ_MSGID	"seq.msgid"	/* local ^AMSGID */
#define SEQ_PKT		"seq.pkt"	/* rfc2ftn: outgoing packets */
#define SEQ_SPLIT	"seq.split"	/* rfc2ftn: msg # of split msgs */
#define SEQ_FF		"seq.ff"	/* ffx: control/data files */
#define SEQ_TOSS	"seq.toss"	/* ftntoss/ftnroute: output packets */
#define SEQ_PACK	"seq.pack"	/* ftnpack: archived .pkts */
#define SEQ_MQ		"seq.mq"	/* queuemail: messages */
#define SEQ_TICK	"seq.tick"	/* ftnhatch/ftntick: .tic file ids */

/* Lock files */
#define LOCK_HISTORY	"historydb"	/* Access to MSGID history database */

/*
 * Directories in SPOOLDIR
 */
#define INDIR		"in"		/* ftn2rfc: input .pkt's */
#define INSECUREDIR	"insecure"	/* ftn2rfc: insecure input */
#define INDIR_MAIL	"mail"		/* ftn2rfc: mail output, rel. INDIR */
#define INDIR_NEWS	"news"		/* ftn2rfc: news output, rel. INDIR */
#define OUTDIR		"out"		/* rfc2ftn: output .pkt's */

#define SEQ		"seq"		/* Dir for sequencer files */
#define LOCKS		"locks"		/* Dir for lock files */
#define TOSS		"toss"		/* Dir for ftntoss/ftnroute/ftnpack */
#define TOSS_IN		"toss/in"	/* ftntoss in */
#define TOSS_TMP	"toss/tmp"	/* ftntoss out, ftnroute in */
#define TOSS_OUT	"toss/out"	/* ftnroute out, ftnpack in */
#define TOSS_PACK	"toss/pack"	/* ftnpack .pkts */
#define TOSS_BAD	"toss/bad"	/* bad .pkts */
#define QUEUE		"queue"		/* queuemail: queued messages */

#define TICK_HOLD	"tick"		/* Relative to cf_outbound() */


/*
 * Permissions
 */
#ifdef SECURE		/* Secure permissions */
# define PACKET_MODE	0600		/* Mode for outbound packets */
# define BSY_MODE	0644		/* Mode for BSY files */
# define FLO_MODE	0644		/* Mode for FLO files */
# define DATA_MODE	0600		/* Mode for ffx data files */
# define DIR_MODE	0755		/* Mode for directories */
# define CONF_MODE	0644		/* Mode for written config files */
#else			/* Open permissions */
# define PACKET_MODE	0666		/* Mode for outbound packets */
# define BSY_MODE	0666		/* Mode for BSY files */
# define FLO_MODE	0666		/* Mode for FLO files */
# define DATA_MODE	0666		/* Mode for ffx data files */
# define DIR_MODE	0777		/* Mode for directories */
# define CONF_MODE	0666		/* Mode for written config files */
#endif

/*
 * RFC headers recognized at beginning of FTN message body
 */
#define FTN_RFC_HEADERS \
    "From:", "Reply-To:", "UUCPFROM:", "To:", "Cc:", "Bcc:", "Newsgroups:"

/*
 * RFC headers output for ^ARFC level 1 (partial RFC headers)
 */
#define RFC_LVL_1_HEADERS \
    "From", "Reply-To", "To", "Cc", "Newsgroups", "Date"

/*
 * Open modes for fopen(), binary for system requiring this.
 */
#ifdef DO_BINARY
# define R_MODE		"rb"
# define W_MODE		"wb"
# define A_MODE		"ab"
# define RP_MODE	"r+b"
# define WP_MODE	"w+b"
# define AP_MODE	"a+b"
#else
# define R_MODE		"r"
# define W_MODE		"w"
# define A_MODE		"a"
# define RP_MODE	"r+"
# define WP_MODE	"w+"
# define AP_MODE	"a+"
#endif /**DO_BINARY**/
#define R_MODE_T	"r"
#define W_MODE_T	"w"
#define A_MODE_T	"a"
#define RP_MODE_T	"r+"
#define WP_MODE_T	"w+"
#define AP_MODE_T	"a+"

/*
 * Format strings for RFC dates
 */
#define DATE_DEFAULT	"%a, %d %b %Y %H:%M:%S %O"
#define DATE_NEWS	"%a, %d %b %Y %H:%M:%S %O"
#define DATE_MAIL	"%a, %d %b %Y %H:%M:%S %O (%Z)"
#define DATE_FIDO	"%a %b %d %H:%M:%S %Y %Z"

/*
 * Product code for packets generated by FIDOGATE, 0xfe is used because
 * this code is reserved for new products, when the code numbers ran out.
 */
#define PRODUCT_CODE	0xfe

/*
 * Line width for formatted messages.
 */
#define MAX_LINELEN	80

/*
 * Program limits
 */
#define MAXADDRESS	32		/* Max. # of FTN address in CONFIG */

#define MAXDOSDRIVE	16		/* Max. # of DOS drives in CONFIG */

#ifndef MAXPATH				/* Already defined by DJGPP */
# define MAXPATH	128		/* Max. size of path names */
#endif

#define MAXINETADDR	128		/* Max. size of an Internet address */

#define MAXUSERNAME	128		/* Max. size of an user name */

#define MAXOPENFILES	10		/* Max. # of open packet files used
					 * by ftntoss/ftnroute, this value
					 * should work on all supported
					 * systems, it can be incremented with
					 * ftntoss/ftnroute's -M option */
