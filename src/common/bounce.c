/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: bounce.c,v 4.0 1996/04/17 18:17:38 mj Exp $
 *
 * Bounce mails for various reasons, using LIBDIR/bounce.XXX messages
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
 * Set CC address for bounced messages
 */
static char *bounce_ccmail = NULL;

void bounce_set_cc(char *cc)
{
    bounce_ccmail = cc;
}



/*
 * Print text file with substitutions for %x
 */
int print_file_subst(FILE *in, FILE *out, Message *msg, char *rfc_to, Textlist *body)
{
    int c;
    
    while((c = getc(in)) != EOF)
    {
	if(c == '%') 
	{
	    c = getc(in);
	    switch(c) 
	    {
	    case 'F':			/* From node */
		fputs( node_to_asc(&msg->node_from, TRUE), out);	break;
	    case 'T':			/* To node */
		fputs( node_to_asc(&msg->node_to, TRUE), out);		break;
	    case 'O':			/* Orig node */
		fputs( node_to_asc(&msg->node_orig, TRUE), out);	break;
	    case 'd':			/* Date */
		fputs( date(NULL, &msg->date), out);			break;
	    case 't':			/* To name */
		fputs( msg->name_to, out);				break;
	    case 'f':			/* From name */
		fputs( msg->name_from, out);				break;
	    case 's':			/* Subject */
		fputs( msg->subject, out);				break;
	    case 'R':			/* RFC To: */
		fputs( rfc_to, out);					break;
	    case 'M':			/* Message */
		tl_print(body, out);				break;
	    }
	}
	else 
	    putc(c, out);
    }
    
    return ferror(in);
}



/*
 * Create header for bounced mail
 */
int bounce_header(char *to)
             				/* To: */
{
    /*
     * Open new mail
     */
    if(mail_open() == ERROR)
	return ERROR;

    /*
     * Create RFC header
     */
    fprintf(mail_file(),
	    "From Mailer-Daemon %s\n", date("%a %b %d %H:%M:%S %Y", NULL) );
    fprintf(mail_file(),
	    "Date: %s\n", date(NULL, NULL) );
    fprintf(mail_file(),
	    "From: Mailer-Daemon@%s (Mail Delivery Subsystem)\n", cf_fqdn() );
    fprintf(mail_file(), "To: %s\n", to);
    if(bounce_ccmail)
	fprintf(mail_file(), "Cc: %s\n", bounce_ccmail);
    /* Additional header may follow in message file */

    return OK;
}



/*
 * Bounce mail
 */
int bounce_mail(char *reason, RFCAddr *addr_from, Message *msg, char *rfc_to, Textlist *body)
{
    char *to;
    FILE *in;
    
    to = rfcaddr_to_asc(addr_from, TRUE);

    if(bounce_header(to) == ERROR)
	return ERROR;

    BUF_COPY2(buffer, "bounce.", reason);
    
    in = libfopen(buffer, R_MODE);
    print_file_subst(in, mail_file(), msg, rfc_to, body);
    fclose(in);
    
    return mail_close();
}


