#:ts=8
#
# $Id: config.make,v 4.0 1996/04/17 18:17:32 mj Exp $
#
# FIDOGATE Makefile configuration
#

# OS2 = OS/2 with EMX GCC

# OS2: comment out
SHELL		= /bin/sh

# Directories
BINDIR		= /usr/local/bin
LIBDIR		= /usr/local/lib/fidogate
INFODIR		= /usr/local/info
SPOOLDIR	= /var/spool/fido
LOGDIR		= /var/log/fido

OUTBOUND	= /var/spool/bt
INBOUND		= /var/spool/bt/in
PINBOUND	= /var/spool/bt/pin
UUINBOUND	= /var/spool/bt/uuin

# perl binary
PERL		= /usr/bin/perl

# owner / group
OWNER		= news
GROUP		= news

# install permissions
PERM_PROG	= 755
PERM_DATA	= 644
PERM_SETUID	= 4755
PERM_DIR	= 755

# C compiler / flags
CC		= gcc
#YACC		= yacc					# Use yacc, not bison
YACC		= bison -y
AR		= ar
#RANLIB		= @echo >/dev/null			# No ranlib
RANLIB		= ranlib
#RANLIB		= ar s					# OS2

#DEBUG		= -O2
DEBUG		= -g

INCLUDE		= -I$(TOPDIR)/src/include

# ISC 3.x
#CFLAGS		= $(DEBUG) -I$(TOPDIR)/src/include -Wall -posix -DISC
# OS2			   
#CFLAGS		= $(DEBUG) -I$(TOPDIR)/src/include -Wall -DOS2
CFLAGS		= $(DEBUG) -I$(TOPDIR)/src/include -Wall

# OS2
#LFLAGS		= -Zexe $(DEBUG) -L$(TOPDIR)/src/common
LFLAGS		= $(DEBUG) -L$(TOPDIR)/src/common

# ISC 3.x
#LIBS		= -lfidogate -linet -lPW -lcposix
LIBS		= -lfidogate

# installation program
# ISC 3.x: use bsdinst
INSTALL_PROG	= install -g $(GROUP) -o $(OWNER) -m $(PERM_PROG)
INSTALL_DATA	= install -g $(GROUP) -o $(OWNER) -m $(PERM_DATA)
INSTALL_SETUID	= install -g $(GROUP) -o $(OWNER) -m $(PERM_SETUID)
INSTALL_DIR	= install -g $(GROUP) -o $(OWNER) -m $(PERM_DIR) -d

# library name
#LIB		= fidogate.a			# OS2
LIB		= libfidogate.a
