/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: charset.c,v 4.2 1996/10/22 19:58:22 mj Exp $
 *
 * ^ACHARSET handling stuff as described in FSC-0054
 *
 *****************************************************************************
 * Copyright (C) 1990-1996
 *  _____ _____
 * |	 |___  |   Martin Junius	     FIDO:	2:2452/110
 * | | | |   | |   Republikplatz 3	     Internet:	mj@fido.de
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FIDOGATE; see the file COPYING.  If not, write to the Free
 * Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *****************************************************************************/

#include "fidogate.h"



#define MAXCHARSET	7

#define CHARSET_N	4

#define CHARSET_NONE	0
#define CHARSET_LATIN1	1
#define CHARSET_IBMPC	2
#define CHARSET_MAC	3

#ifdef CHARSET_DEFAULT_IBMPC
/* Default charset ist "IBMPC" */
# define CHARSET_DEFAULT	CHARSET_IBMPC
#else
/* Default charset ist "NONE" */
# define CHARSET_DEFAULT	CHARSET_NONE
#endif


static char *level_2_opt[MAXCHARSET] = {
    "ASCII",	    "LATIN-1",	    "IBMPC",	    "MAC",
    "VT100",	    "PC-8",	    "AMIGA"
};

static int   level_2_chrs[MAXCHARSET] = {
    CHARSET_NONE,   CHARSET_LATIN1, CHARSET_IBMPC,  CHARSET_MAC,
    CHARSET_NONE,   CHARSET_IBMPC,  CHARSET_LATIN1
};

static int charset = CHARSET_DEFAULT;



/*
 * Translation table
 */
static char *xtab[3][CHARSET_N][128] = {
/* xtab[0][][] = US-ASCII */
{
/***** Output: ASCII 7bit translations ***************************************/
{
	/***** Default - common characters ********/
	"Ae", "ue", "e", "a", "ae", "Oe", "Ue", "c", 
	"e", "e", "ae", "i", "i", "i", "Ae", "A", 
	"E", "ae", "AE", "o", "oe", "o", "u", "ue", 
	"y", "Oe", "Ue", "c", "#", "Y", "ss", "ss", 
	"a", "i", "o", "u", "n", "N", "a", "o", 
	"?", "-", "!", ".5", "?", "!", "<<", ">>", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "Ae", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "Oe", "|", 
	"+", "+", "+", "#", "Ue", "|", "|", "ss", 
	"a", "ss", "?", "pi", "ae", "?", "mu", "?", 
	"?", "?", "O", "d", "?", "o", "?", "?", 
	"==", "+-", ">=", "<=", "?", "?", "oe", "~=", 
	"?", ".", "-", "?", "ue", "^2", "o", " "
},
{
	/***** ISO LATIN-1 ************************/
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	" ", "!", "c", "#", "?", "Y", "|", "S", 
	"?", "c", "a", "<<", "!", "-", "R", "?", 
	"o", "+-", "^2", "^3", "'", "mu", "?", "o", 
	",", "^1", "o", ">>", "?", ".5", "?", "?", 
	"A", "A", "A", "A", "Ae", "A", "AE", "C", 
	"E", "E", "E", "E", "I", "I", "I", "I", 
	"D", "N", "O", "O", "O", "O", "Oe", "x", 
	"0", "U", "U", "U", "Ue", "Y", "?", "ss", 
	"a", "a", "a", "a", "ae", "a", "ae", "c", 
	"e", "e", "e", "e", "i", "i", "i", "i", 
	"?", "n", "o", "o", "o", "o", "oe", "/", 
	"o", "u", "u", "u", "ue", "y", "?", "y"
},
{
	/***** IBM PC *****************************/
	"C", "ue", "e", "a", "ae", "a", "a", "c", 
	"e", "e", "e", "i", "i", "i", "Ae", "A", 
	"E", "ae", "AE", "o", "oe", "o", "u", "u", 
	"y", "Oe", "Ue", "c", "#", "Y", "Pt", "f", 
	"a", "i", "o", "u", "n", "N", "a", "o", 
	"?", "-", "!", ".5", "?", "!", "<<", ">>", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "-", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "+", "|", 
	"+", "+", "+", "#", "n", "|", "|", "~", 
	"a", "ss", "?", "pi", "?", "?", "mu", "?", 
	"?", "?", "O", "d", "?", "o", "?", "?", 
	"==", "+-", ">=", "<=", "?", "?", "/", "~=", 
	"?", ".", "-", "?", "^n", "^2", "o", " "
},
{
	/***** MACINTOSH **************************/
	"Ae", "A", "C", "E", "N", "Oe", "Ue", "a", 
	"a", "a", "ae", "a", "a", "c", "e", "e", 
	"e", "e", "i", "i", "i", "i", "n", "o", 
	"o", "o", "oe", "o", "u", "u", "u", "ue", 
	"+", "o", "c", "#", "S", "o", "?", "ss", 
	"R", "c", "TM", "'", "?", "<>", "AE", "0", 
	"?", "+-", "<=", ">=", "Y", "mu", "d", "?", 
	"?", "pi", "?", "a", "o", "O", "ae", "o", 
	"?", "!", "!", "?", "f", "~=", "?", ">>", 
	"<<", "..", " ", "A", "A", "O", "OE", "oe", 
	"-", "-", "\"", "\"", "`", "'", "/", "?", 
	"y", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?"
}
},
/* xtab[1][][] = ISO-8859-1 8BIT */
{
/***** Output: ISO-8859-1 8bit chars *****************************************/
{
	/***** Default - common characters ********/
	"\307", "\374", "\351", "\342", "\344", "\340", "\345", "\347", 
	"\352", "\353", "\350", "\357", "\356", "\354", "\304", "\305", 
	"\311", "\346", "\306", "\364", "\366", "\362", "\373", "\371", 
	"\377", "\326", "\334", "\242", "\243", "\245", "Pt", "f",
	"\341", "\355", "\363", "\372", "\361", "\321", "\252", "\272", 
	"\277", "-", "\254", "\275", "\274", "\241", "\253", "\273", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "\304", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "\326", "|", 
	"+", "+", "+", "#", "\334", "|", "|", "\337", 
	"alpha", "\337", "Gamma", "pi", "\344", "sigma", "\265", "tau", 
	"Phi", "Theta", "Omega", "delta", "infty", "phi", "in", "cap", 
	"==", "\261", ">=", "<=", "integ1", "integ2", "\366", "~=", 
	"\260", ".", "\267", "sqrt", "\374", "\262", "o", " "
},
{
	/***** ISO LATIN-1 ************************/
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	" ", "\241", "\242", "\243", "\244", "\245", "\246", "\247", 
	"\250", "\251", "\252", "\253", "\254", "\255", "\256", "\257", 
	"\260", "\261", "\262", "\263", "\264", "\265", "\266", "\267", 
	"\270", "\271", "\272", "\273", "\274", "\275", "\276", "\277", 
	"\300", "\301", "\302", "\303", "\304", "\305", "\306", "\307", 
	"\310", "\311", "\312", "\313", "\314", "\315", "\316", "\317", 
	"\320", "\321", "\322", "\323", "\324", "\325", "\326", "\327", 
	"\330", "\331", "\332", "\333", "\334", "\335", "\336", "\337", 
	"\340", "\341", "\342", "\343", "\344", "\345", "\346", "\347", 
	"\350", "\351", "\352", "\353", "\354", "\355", "\356", "\357", 
	"\360", "\361", "\362", "\363", "\364", "\365", "\366", "\367", 
	"\370", "\371", "\372", "\373", "\374", "\375", "\376", "\377", 
},
{
	/***** IBM PC *****************************/
	"\307", "\374", "\351", "\342", "\344", "\340", "\345", "\347", 
	"\352", "\353", "\350", "\357", "\356", "\354", "\304", "\305", 
	"\311", "\346", "\306", "\364", "\366", "\362", "\373", "\371", 
	"\377", "\326", "\334", "\242", "\243", "\245", "Pt", "f",
	"\341", "\355", "\363", "\372", "\361", "\321", "\252", "\272", 
	"\277", "-", "\254", "\275", "\274", "\241", "\253", "\273", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "-", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "+", "|", 
	"+", "+", "+", "#", "n", "|", "|", "~", 
	"alpha", "\337", "Gamma", "pi", "Sigma", "sigma", "\265", "tau", 
	"Phi", "Theta", "Omega", "delta", "infty", "phi", "in", "cap", 
	"==", "\261", ">=", "<=", "integ1", "integ2", "\367", "~=", 
	"\260", ".", "\267", "sqrt", "^n", "\262", "o", " "
},
{
	/***** MACINTOSH **************************/
	"\304", "\305", "\307", "\311", "\321", "\326", "\334", "\341", 
	"\340", "\342", "\344", "\343", "\345", "\347", "\351", "\350", 
	"\352", "\353", "\355", "\354", "\356", "\357", "\361", "\363", 
	"\362", "\364", "\366", "\365", "\372", "\371", "\373", "\374", 
	"\276", "\260", "\242", "\243", "\247", "o", "\266", "\337", 
	"\256", "\251", "TM", "\264", "\250", "\255", "\306", "\330", 
	"?", "\261", "\262", "\263", "\245", "\265", "\246", "?", 
	"?", "\271", "\274", "\252", "\272", "\275", "\346", "\370", 
	"\277", "\241", "\254", "?", "f", "~=", "?", "\253", 
	"\273", "..", " ", "\300", "\303", "\325", "OE", "oe", 
	"\320", "-", "\"", "\"", "`", "'", "/", "\327", 
	"\377", "?", "?", "\244", "?", "\335", "\336", "?", 
	"?", "\267", "?", "?", "?", "\302", "\312", "\301", 
	"\313", "\310", "\315", "\316", "\317", "\314", "\323", "\324", 
	"\360", "\322", "\332", "\333", "\331", "?", "?", "?", 
	"\257", "?", "?", "?", "\270", "\375", "\376", "?"
}
},
/* xtab[2][][] = ISO-8859-2 QUOTED-PRINTABLE */
{
/***** Output: ISO-8859-1 quoted-printable chars ****************************/
{
	/***** Default - common characters ********/
	"=C7", "=FC", "=E9", "=E2", "=E4", "=E0", "=E5", "=E7", 
	"=EA", "=EB", "=E8", "=EF", "=EE", "=EC", "=C4", "=C5", 
	"=C9", "=E6", "=C6", "=F4", "=F6", "=F2", "=FB", "=F9", 
	"=FF", "=D6", "=DC", "=A2", "=A3", "=A5", "Pt", "f",
	"=E1", "=ED", "=F3", "=FA", "=F1", "=D1", "=AA", "=BA", 
	"=BF", "-", "=AC", "=BD", "=BC", "=A1", "=AB", "=BB", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "=C4", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "=D6", "|", 
	"+", "+", "+", "#", "=DC", "|", "|", "=DF", 
	"alpha", "=DF", "Gamma", "pi", "=E4", "sigma", "=B5", "tau", 
	"Phi", "Theta", "Omega", "delta", "infty", "phi", "in", "cap", 
	"==", "=B1", ">=", "<=", "integ1", "integ2", "=F6", "~=", 
	"=B0", ".", "=B7", "sqrt", "=FC", "=B2", "o", " "
},
{
	/***** ISO LATIN-1 ************************/
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "?", "?", "?", "?", "?", "?", "?", 
	"?", "=A1", "=A2", "=A3", "=A4", "=A5", "=A6", "=A7", 
	"=A8", "=A9", "=AA", "=AB", "=AC", "=AD", "=AE", "=AF", 
	"=B0", "=B1", "=B2", "=B3", "=B4", "=B5", "=B6", "=B7", 
	"=B8", "=B9", "=BA", "=BB", "=BC", "=BD", "=BE", "=BF", 
	"=C0", "=C1", "=C2", "=C3", "=C4", "=C5", "=C6", "=C7", 
	"=C8", "=C9", "=CA", "=CB", "=CC", "=CD", "=CE", "=CF", 
	"=D0", "=D1", "=D2", "=D3", "=D4", "=D5", "=D6", "=D7", 
	"=D8", "=D9", "=DA", "=DB", "=DC", "=DD", "=DE", "=DF", 
	"=E0", "=E1", "=E2", "=E3", "=E4", "=E5", "=E6", "=E7", 
	"=E8", "=E9", "=EA", "=EB", "=EC", "=ED", "=EE", "=EF", 
	"=F0", "=F1", "=F2", "=F3", "=F4", "=F5", "=F6", "=F7", 
	"=F8", "=F9", "=FA", "=FB", "=FC", "=FD", "=FE", "=FF", 
},
{
	/***** IBM PC *****************************/
	"=C7", "=FC", "=E9", "=E2", "=E4", "=E0", "=E5", "=E7", 
	"=EA", "=EB", "=E8", "=EF", "=EE", "=EC", "=C4", "=C5", 
	"=C9", "=E6", "=C6", "=F4", "=F6", "=F2", "=FB", "=F9", 
	"=FF", "=D6", "=DC", "=A2", "=A3", "=A5", "Pt", "f",
	"=E1", "=ED", "=F3", "=FA", "=F1", "=D1", "=AA", "=BA", 
	"=BF", "-", "=AC", "=BD", "=BC", "=A1", "=AB", "=BB", 
	"#", "#", "#", "|", "|", "|", "|", "+", 
	"+", "|", "|", "+", "+", "+", "+", "+", 
	"+", "-", "-", "|", "-", "+", "|", "|", 
	"+", "+", "=", "=", "|", "=", "+", "=", 
	"-", "=", "-", "+", "+", "+", "+", "|", 
	"+", "+", "+", "#", "n", "|", "|", "~", 
	"alpha", "=DF", "Gamma", "pi", "Sigma", "sigma", "=B5", "tau", 
	"Phi", "Theta", "Omega", "delta", "infty", "phi", "in", "cap", 
	"==", "=B1", ">=", "<=", "integ1", "integ2", "=F7", "~=", 
	"=B0", ".", "=B7", "sqrt", "^n", "=B2", "o", " "
},
{
	/***** MACINTOSH **************************/
	"=C4", "=C5", "=C7", "=C9", "=D1", "=D6", "=DC", "=E1", 
	"=E0", "=E2", "=E4", "=E3", "=E5", "=E7", "=E9", "=E8", 
	"=EA", "=EB", "=ED", "=EC", "=EE", "=EF", "=F1", "=F3", 
	"=F2", "=F4", "=F6", "=F5", "=FA", "=F9", "=FB", "=FC", 
	"=BE", "=B0", "=A2", "=A3", "=A7", "o", "=B6", "=DF", 
	"=AE", "=A9", "TM", "=B4", "=A8", "=AD", "=C6", "=D8", 
	"?", "=B1", "=B2", "=B3", "=A5", "=B5", "=A6", "?", 
	"?", "=B9", "=BC", "=AA", "=BA", "=BD", "=E6", "=F8", 
	"=BF", "=A1", "=AC", "?", "f", "~=", "?", "=AB", 
	"=BB", "..", " ", "=C0", "=C3", "=D5", "OE", "oe", 
	"=D0", "-", "\"", "\"", "`", "'", "/", "=D7", 
	"=FF", "?", "?", "=A4", "?", "=DD", "=DE", "?", 
	"?", "=B7", "?", "?", "?", "=C2", "=CA", "=C1", 
	"=CB", "=C8", "=CD", "=CE", "=CF", "=CC", "=D3", "=D4", 
	"=F0", "=D2", "=DA", "=DB", "=D9", "?", "?", "?", 
	"=AF", "?", "?", "?", "=B8", "=FD", "=FE", "?"
}
}
};



/*
 * Reset char set
 */
void charset_reset(void)
{
    charset = CHARSET_DEFAULT;
}



/*
 * Set character set from ^ACHARSET kludge line
 */
void charset_set(char *s)
{
    char *name;
    char *p;
    int i=0;
    int level;
    
    while(is_space(*s))
	s++;

    debug(5, "^ACHARSET: %s", s);

    s = strsave(s);
    name  = strtok(s,	 " ");
    if(!name)
	return;
    p	  = strtok(NULL, " ");
    if(!p)
	/*
	 * In this case it's a FSC-0050 kludge without the class code.
	 * Treat it like FSC-0054 level 2.
	 */
	level = 2;
    else
	level = atoi(p);
    
    charset = CHARSET_NONE;
    if(level == 2)
	for(charset=i=0; i<MAXCHARSET; i++)
	    if(!strcmp(name, level_2_opt[i])) {
		charset = level_2_chrs[i];
		break;
	    }
    xfree(s);

    if(charset != CHARSET_NONE)
	debug(5, "charset=%d (%s)", charset, level_2_opt[i]);
}



/*
 * Get translated char
 */
char *charset_xlate(int c, int cidx)
{
    if(charset>=0 && charset<CHARSET_N)
	return xtab[cidx][charset][c & 0x7f];
    else
	return NULL;
}
