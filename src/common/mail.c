/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: mail.c,v 4.2 1997/08/10 17:34:22 mj Exp $
 *
 * Create RFC messages in mail/news dir
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



char mail_dir[MAXPATH];
char news_dir[MAXPATH];

static char m_name[MAXPATH];
static char m_tmp [MAXPATH];
static FILE *m_file = NULL;



/*
 * Open RFC mail file
 */
int mail_open(int sel)
{
    long n;
    
    switch(sel)
    {
    case 'm':
    case 'M':
	n = sequencer(SEQ_MAIL);
	sprintf(m_tmp,  "%s/%08ld.tmp", mail_dir, n);
	sprintf(m_name, "%s/%08ld.msg", mail_dir, n);
	break;
	
    case 'n':
    case 'N':
	n = sequencer(SEQ_NEWS);
	sprintf(m_tmp,  "%s/%08ld.tmp", news_dir, n);
	sprintf(m_name, "%s/%08ld.msg", news_dir, n);
	break;

    default:
	log("mail_open(%d): illegal value", sel);
	return ERROR;
	break;
    }

    
    m_file = fopen(m_tmp, W_MODE);
    if(!m_file) {
	log("$Can't create mail file %s", m_tmp);
	return ERROR;
    }

    return OK;
}



/*
 * Return mail tmp name
 */
char *mail_name(void)
{
    return m_tmp;
}



/*
 * Return mail file pointer
 */
FILE *mail_file(void)
{
    return m_file;
}



/*
 * Kill mail file
 */
int mail_kill(void)
{
    int err;
    
    fclose(m_file);
    err = unlink(m_tmp);
    if(err == ERROR) 
	log("$Can't unlink mail file %s", m_tmp);
    
    m_tmp[0]  = 0;
    m_name[0] = 0;
    m_file    = NULL;

    return err;
}



/*
 * Close mail file
 */
int mail_close(void)
{
    int err;
    
    fclose(m_file);
    err = rename(m_tmp, m_name);
    if(err == ERROR)
	log("$Can't rename mail file %s to %s", m_tmp, m_name);
    
    m_tmp[0]  = 0;
    m_name[0] = 0;
    m_file    = NULL;

    return err;
}
