/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: charset.h,v 4.0 1996/04/17 18:17:41 mj Exp $
 *
 * ^ACHARSET conversion tables
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
