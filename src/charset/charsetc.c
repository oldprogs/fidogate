/*:ts=8*/
/*****************************************************************************
 * FIDOGATE --- Gateway UNIX Mail/News <-> FTN NetMail/EchoMail
 *
 * $Id: charsetc.c,v 1.2 1998/04/03 20:15:34 mj Exp $
 *
 * Charset mapping table compiler
 *
 *****************************************************************************
 * Copyright (C) 1990-1998
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
#include "getopt.h"



#define PROGRAM		"charsetc"
#define VERSION		"$Revision: 1.2 $"



/*->structs.h----------------------------------------------------------------*/
/*---------------------------------------------------------------------------*/


/*-common--------------------------------------------------------------------*/
/*
 * Alias linked list
 */
static CharsetAlias *charset_alias_list = NULL;
static CharsetAlias *charset_alias_last = NULL;

/*
 * Table linked list
 */
static CharsetTable *charset_table_list = NULL;
static CharsetTable *charset_table_last = NULL;



/*---------------------------------------------------------------------------*/


/*->charset.c----------------------------------------------------------------*/
/*
 * Alloc new CharsetTable and put into linked list
 */
CharsetTable *charset_table_new_link(void)
{
    CharsetTable *p;

    /* Alloc and clear */
    p = (CharsetTable *)xmalloc(sizeof(CharsetTable));
    memset(p, 0, sizeof(CharsetTable));
    p->next = NULL;			/* Just to be sure */
    
    /* Put into linked list */
    if(charset_table_list)
	charset_table_last->next = p;
    else
	charset_table_list       = p;
    charset_table_last       = p;

    return p;
}



/*
 * Alloc new CharsetAlias and put into linked list
 */
CharsetAlias *charset_alias_new_link(void)
{
    CharsetAlias *p;

    /* Alloc and clear */
    p = (CharsetAlias *)xmalloc(sizeof(CharsetAlias));
    memset(p, 0, sizeof(CharsetAlias));
    p->next = NULL;			/* Just to be sure */
    
    /* Put into linked list */
    if(charset_alias_list)
	charset_alias_last->next = p;
    else
	charset_alias_list       = p;
    charset_alias_last       = p;

    return p;
}




/*---------------------------------------------------------------------------*/

/*
 * Process one line from charset.map file
 */
static int charset_do_line(char *line)
{
    static CharsetTable *pt = NULL;
    char *key, *w1, *w2;
    CharsetAlias *pa;
    int i, j;
    
    debug(16, "charset.map line: %s", line);

    key = strtok(line, " \t");

    /* Include map file */
    if(      strieq(key, "include") ) 
    {
	w1 = strtok(NULL, " \t");
	if( charset_do_file(w1) == ERROR)
	    return ERROR;
    }
    /* Define alias */
    else if( strieq(key, "alias") ) 
    {
	w1 = strtok(NULL, " \t");
	w2 = strtok(NULL, " \t");
	if(!w1 || !w2) 
	{
	    fprintf(stderr, "%s:%ld: argument(s) for alias missing\n",
		    PROGRAM, cf_lineno_get());
	    return ERROR;
	}
	
	pa = charset_alias_new_link();
	BUF_COPY(pa->alias, w1);
	BUF_COPY(pa->name, w2);
	debug(15, "new alias: alias=%s name=%s", pa->alias, pa->name);
    }
    /* Define table */
    else if( strieq(key, "table") )
    {
	w1 = strtok(NULL, " \t");
	w2 = strtok(NULL, " \t");
	if(!w1 || !w2) 
	{
	    fprintf(stderr, "%s:%ld: argument(s) for table missing\n",
		    PROGRAM, cf_lineno_get());
	    return ERROR;
	}

	pt = charset_table_new_link();
	BUF_COPY(pt->in, w1);
	BUF_COPY(pt->out, w2);
	debug(15, "new table: in=%s out=%s", pt->in, pt->out);
    }
    /* Define mapping for character(s) in table */
    else if( strieq(key, "map") )
    {
	w1 = strtok(NULL, " \t");
	if(!w1) 
	{
	    fprintf(stderr, "%s:%ld: argument for map missing\n",
		    PROGRAM, cf_lineno_get());
	    return ERROR;
	}

	/* 1:1 mapping */
	if(strieq(w1, "1:1"))
	{
	    for(i=0; i<MAX_CHARSET_IN; i++)
	    {
		if(pt->map[i][0] == 0)
		{
		    pt->map[i][0] = 0x80 + i;
		    pt->map[i][1] = 0;
		}
	    }
	}
	/* 1:1 mapping, but not for 0x80...0x9f */
	if(strieq(w1, "1:1-noctrl"))
	{
	    for(i=0x20; i<MAX_CHARSET_IN; i++)
	    {
		if(pt->map[i][0] == 0)
		{
		    pt->map[i][0] = 0x80 + i;
		    pt->map[i][1] = 0;
		}
	    }
	}
	/* Mapping for undefined characters */
	else if(strieq(w1, "default"))
	{
	    /**FIXME: not yet implemented**/
	}
	/* Normal mapping */
	else
	{
	}
    }
    /* Error */
    else 
    {
	fprintf(stderr, "%s:%ld: illegal key word %s\n",
		PROGRAM, cf_lineno_get(), key);
	return ERROR;
    }
    
    return OK;
}



/*
 * Process charset.map file
 */
static int charset_do_file(char *name)
{
    FILE *fp;
    char *p;
    long oldn;
    
    debug(14, "Reading charset.map file %s", name);
    
    oldn = cf_lineno_set(0);
    fp = fopen_expand_name(name, R_MODE_T, FALSE);
    if(!fp)
	return ERROR;
    
    while( (p = cf_getline(buffer, BUFFERSIZE, fp)) )
	charset_do_line(p);

    fclose(fp);
    cf_lineno_set(oldn);
    
    return OK;
}



/*
 * Write binary mapping file
 */
int charset_write_bin(char *name)
{
    FILE *fp;
    CharsetTable *pt;
    CharsetAlias *pa;
    
    debug(14, "Writing charset.bin file %s", name);
    
    fp = fopen_expand_name(name, W_MODE, FALSE);
    if(!fp)
	return ERROR;

    /* Write aliases */
    for(pa = charset_alias_list; pa; pa=pa->next)
    {
	fputc(CHARSET_FILE_ALIAS, fp);
	fwrite(pa, sizeof(CharsetAlias), 1, fp);
	if(ferror(fp))
	{
	    fclose(fp);
	    return ERROR;
	}
    }
    /* Write tables */
    for(pt = charset_table_list; pt; pt=pt->next)
    {
	fputc(CHARSET_FILE_TABLE, fp);
	fwrite(pt, sizeof(CharsetTable), 1, fp);
	if(ferror(fp))
	{
	    fclose(fp);
	    return ERROR;
	}
    }

    fclose(fp);
    return OK;
}



/*
 * Compile charset.map
 */
int compile_map(char *in, char *out)
{
    /* Read charset.map and compile */
    if(charset_do_file(in) == ERROR)
    {
	fprintf(stderr, "%s: compiling map file %s failed", PROGRAM, in);
	return EXIT_ERROR;
    }

    /* Write binary output file */
    if(charset_write_bin(out) == ERROR)
    {
	fprintf(stderr, "%s: writing binary map file %s failed", PROGRAM, out);
	return EXIT_ERROR;
    }

    return EXIT_OK;
}



/*
 * Usage messages
 */
void short_usage(void)
{
    fprintf(stderr, "usage: %s [-options] charset.map charset.bin\n", PROGRAM);
    fprintf(stderr, "       %s --help  for more information\n", PROGRAM);
    exit(EX_USAGE);
}


void usage(void)
{
    fprintf(stderr, "FIDOGATE %s  %s %s\n\n",
	    version_global(), PROGRAM, version_local(VERSION) );
    
    fprintf(stderr, "usage:   %s [-options] charset.map charset.bin\n\n",
	    PROGRAM);
    fprintf(stderr, "\
options:  \n\
\n\
          -v --verbose                 verbose\n\
	  -h --help                    this help\n"                     );

    exit(0);
}



/***** main() ****************************************************************/

int main(int argc, char **argv)
{
    int c;
    int ret = EXIT_OK;
    char *name_in, *name_out;
    
    int option_index;
    static struct option long_options[] =
    {

	{ "verbose",      0, 0, 'v'},	/* More verbose */
	{ "help",         0, 0, 'h'},	/* Help */
	{ 0,              0, 0, 0  }
    };

    
    log_file("stderr");
    log_program(PROGRAM);
    
    while ((c = getopt_long(argc, argv, "vh",
			    long_options, &option_index     )) != EOF)
	switch (c) {
	    
	/***** Common options *****/
	case 'v':
	    verbose++;
	    break;
	case 'h':
	    usage();
	    break;
	default:
	    short_usage();
	    break;
	}

    if(optind+2 != argc)
	short_usage();

    name_in  = argv[optind++];
    name_out = argv[optind++];

    ret = compile_map(name_in, name_out);
    
    exit(ret);

    /**NOT REACHED**/
    return 1;
}
