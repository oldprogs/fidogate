/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: parsenode.c,v 4.2 1996/05/12 15:35:06 mj Exp $
 *
 * Parse FTN address strings (Z:N/F.P)
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
#include "shuffle.h"



/*
 * Return: >=0 number, ERROR (-1) error, WILDCARD (-2) "*" or "All"
 */
int znfp_get_number(char **ps)
{
    char *s = *ps;
    int val = 0;

    if(*s == '*')
    {
	s++;
	val = WILDCARD;
    }
    else if(strnieq(s, "all", 3))
    {
	s += 3;
	val = WILDCARD;
    }
    else if(!isdigit(*s))
	return ERROR;
    else
	while(isdigit(*s))
	    val = val*10 + *s++ - '0';

    *ps = s;
    
    return val;
}



/*
 * Convert partial Z:N/F.P address to Node, allowing "*" or "all" as wildcard
 */
int znfp_parse_partial(char *asc, Node *node)
{
    Node n;
    char *s = asc;
    int val1, val;

    val1    = -1;

    /* Set Node n to empty */
    n.zone = n.net = n.node = n.point = EMPTY;
    n.domain[0] = 0;
    
    /* Special case global wildcard "*", "All", or "World" */
    if(streq(asc, "*") || strieq(asc, "all") || strieq(asc, "world"))
    {
	n.zone = n.net = n.node = n.point = WILDCARD;
	if(node)
	    *node = n;
	return OK;
    }
    
    /* Now for the dirty parsing ... ;-) */
    if(!*s)
	return ERROR;

    /* A number must come first, exception point address only .N */
    if(*s != '.')
	if( (val1 = znfp_get_number(&s)) == ERROR )
	    return ERROR;

    if(*s == ':')			/* val1 is zone */
    {
	s++;
	if(val1 != -1)
	{
	    n.zone = val1;
	    val1   = -1;
	}
	if( (val = znfp_get_number(&s)) == ERROR )
	    return ERROR;
	n.net = val;
    }
    if(*s == '/')			/* val1 is net */
    {
	s++;
	if(val1 != -1)
	{
	    n.net = val1;
	    val1  = -1;
	}
	if( (val = znfp_get_number(&s)) == ERROR )
	    return ERROR;
	n.node = val;
    }
    if(*s == '.')			/* val1 is node */
    {
	s++;
	if(val1 != -1)
	{
	    n.node = val1;
	    val1   = -1;
	}
	/* Point address after "." */
	if( (val = znfp_get_number(&s)) == ERROR )
	    return ERROR;
	n.point = val;
    }
    if(val1 != -1)			/* number only: val1 is node */
	n.node = val1;
    
    if(*s == '@')			/* Domain address may follow */
    {
	s++;
	BUF_COPY(n.domain, s);
    }
    else if(*s)
	return ERROR;

    if(node)
	*node = n;
    return OK;
}



/*
 * Convert partial Z:N/F.P address to Node, using previous node address
 */
int znfp_parse_diff(char *asc, Node *node, Node *oldnode)
{
    /* Parse it ... */
    if(znfp_parse_partial(asc, node) == ERROR)
	return ERROR;

    /* Replace empty parts (-1) with value from oldnode */
    if(node)
    {
	if(node->zone == EMPTY)
	{
	    /* No zone, use old zone address */
	    node->zone = oldnode->zone;
	    if(node->net == EMPTY)
	    {
		/* No net, use old net address */
		node ->net = oldnode->net;
		if(node->node == EMPTY)
		{
		    node->node = oldnode->node;
		}
	    }
	}
    }
    
    return OK;
}



/*
 * Convert field value
 */
char *znfp_put_number(int val, int wildcards)
{
    static char buf[16];

    if(wildcards && val==WILDCARD)
	BUF_COPY(buf, "*");
    else
	sprintf(buf, "%d", val);

    return buf;
}

	

/*
 * Output Node struct
 */
char *znfp_print(Node *node, int point0, int wildcards)
{
    SHUFFLEBUFFERS;

    /* Initialize to empty string */
    tcharp[0] = 0;
    
    /* Always display point address if wildcards==TRUE */
    if(wildcards)
	point0 = TRUE;
    
    /* Invalid address */
    if(node->zone==INVALID && node->net==INVALID &&
       node->node==INVALID && node->point==INVALID  )
    {
	str_copy(tcharp, MAX_CONVERT_BUFLEN, "INVALID");
	return tcharp;
    }

    /* Global wildcard */
    if(wildcards &&
       node->zone==WILDCARD && node->net==WILDCARD &&
       node->node==WILDCARD && node->point==WILDCARD  )
    {
	str_copy(tcharp, MAX_CONVERT_BUFLEN, "*");
	return tcharp;
    }

    
    /* Zone */
    if(node->zone != EMPTY)
    {
	str_append(tcharp, MAX_CONVERT_BUFLEN,
		   znfp_put_number(node->zone, wildcards));
	str_append(tcharp, MAX_CONVERT_BUFLEN, ":");
    }
    /* Net */
    if(node->net != EMPTY)
    {
	str_append(tcharp, MAX_CONVERT_BUFLEN,
		   znfp_put_number(node->net, wildcards));
	if(node->node != EMPTY)
	    str_append(tcharp, MAX_CONVERT_BUFLEN, "/");
    }
    /* Node */
    if(node->node != EMPTY)
    {
	str_append(tcharp, MAX_CONVERT_BUFLEN,
		   znfp_put_number(node->node, wildcards));
    }
    /* Point */
    if(! (node->point==EMPTY || (node->point==0 && !point0)) )
    {
	str_append(tcharp, MAX_CONVERT_BUFLEN, ".");
	str_append(tcharp, MAX_CONVERT_BUFLEN,
		   znfp_put_number(node->point, wildcards));
    }

    /* Domain */
    if(node->domain[0])
    {
	str_append(tcharp,  MAX_CONVERT_BUFLEN, "@");
	str_append(tcharp, MAX_CONVERT_BUFLEN, node->domain);
    }

    return tcharp;
}



/*
 * Standard output function for Node
 */
char *znfp(Node *node)
{
    return znfp_print(node, TRUE, TRUE);
}




#ifdef TEST
/*
 * Parser test
 */
int main(int argc, char *argv[])
{
    Node o, n;
    
    if(argc!=2 && argc!=3)
    {
	fprintf(stderr,
		"usage: testparse Z:N/F.P\n"
		"       testparse Z:N/F.P Z:N/F.P\n");
	exit(1);
    }

    /* Single FTN address */
    if(argc == 2)
    {
	if(znfp_parse_partial(argv[1], &n) == ERROR)
	{
	    fprintf(stderr, "testparse: can't parse %s\n", argv[1]);
	    exit(1);
	}
	printf("testparse 1: val=%d:%d/%d.%d\n",
	       n.zone, n.net, n.node, n.point   );
	printf("             str=%s\n", znfp(&n));
	exit(0);
    }
    
    /* Old and new FTN address */
    if(argc == 3)
    {
	if(znfp_parse_partial(argv[1], &o) == ERROR)
	{
	    fprintf(stderr, "testparse: can't parse %s\n", argv[1]);
	    exit(1);
	}
	if(znfp_parse_diff(argv[2], &n, &o) == ERROR)
	{
	    fprintf(stderr, "testparse: can't parse %s\n", argv[2]);
	    exit(1);
	}
	printf("testparse 2: val1=%d:%d/%d.%d val2=%d:%d/%d.%d\n",
	       o.zone, o.net, o.node, o.point, n.zone, n.net, n.node, n.point);
	printf("             str1=%s str2=%s\n", znfp(&o), znfp(&n));
	exit(0);
    }

    /**NOT REACHED**/
    return 1;
}
#endif /**TEST**/