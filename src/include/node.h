/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FIDO NetMail/EchoMail
 *
 * $Id: node.h,v 4.1 1996/12/17 17:19:53 mj Exp $
 *
 * Node structure (zone, net, node, point, domain)
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

#define MAX_DOMAIN  32


/*
 * FTN node address
 */
typedef struct st_node
{
    int zone;
    int net;
    int node;
    int point;
    char domain[MAX_DOMAIN];
}
Node;



/*
 * Linked node entry for list of nodes
 */
typedef struct st_lnode
{
    Node node;
    struct st_lnode *next, *prev;
}
LNode;

/*
 * List of nodes
 */
typedef struct st_lon
{
    int size;
    Node **sorted;
    LNode *first, *last;
}
LON;



