# -*- Makefile -*-
#
# $Id: config.make,v 1.1 1997/08/20 21:05:00 mj Exp $
#
# FIDOGATE Makefile configuration for GNU-WIN32 / Perl for WIN32
#

##############################################################
# Settings to be used by subst.pl must start in column 1 and #
# use uppercase A-Z letters!!!                               #
##############################################################


# OS2 = OS/2 with EMX GCC

# FIDOGATE Directories
BINDIR		= /fidogate/bin
LIBDIR		= /fidogate/lib
SPOOLDIR	= /fidogate/spool
LOGDIR		= /fidogate/log

# Doc directories
INFODIR		= /fidogate/doc
HTMLDIR		= /fidogate/doc

# Outbound *base* directory, i.e. outbound is OUTBOUND/out.xxx
OUTBOUND	= /fidogate/bt
# Normal (i.e. insecure) inbound
INBOUND		= /fidogate/bt/in
# Protected inbound
PINBOUND	= /fidogate/bt/pin
# Inbound for uuencoded mails (used by recvuu)
UUINBOUND	= /fidogate/bt/uuin

# INN directories
# (not used by GNU-Win32)
NEWSETCDIR	= /etc/news
NEWSVARDIR	= /var/lib/news
NEWSLIBDIR	= /usr/lib/news
NEWSSPOOLDIR	= /var/spool/news

# Ifmail directory
# (not used by GNU-Win32)
IFMAILDIR       = /usr/local/lib/ifmail

# perl
PERL		= /usr/bin/perl

# OS2: comment out
# SHELL		= /bin/sh

# m4
 M4		= m4

# owner / group
 OWNER		= 0
 GROUP		= 0

# install permissions
 PERM_PROG	= 755
 PERM_DATA	= 644
 PERM_SETUID	= 4755
 PERM_DIR	= 755

# C compiler / flags
 CC		= gcc
# YACC		= yacc					# Use yacc, not bison
 YACC		= bison -y
 AR		= ar
# RANLIB	= @echo >/dev/null			# No ranlib
 RANLIB		= ranlib
# RANLIB	= ar s					# OS2

# DEBUG		= -O2
 DEBUG		= -g

 INCLUDE	= -I$(TOPDIR) -I$(TOPDIR)/src/include

# NEXTSTEP 3.3
# CFLAGS	= $(DEBUG) $(INCLUDE) -Wall -posix
# ISC 3.x
# CFLAGS	= $(DEBUG) $(INCLUDE) -Wall -posix -DISC
# OS2			   
# CFLAGS	= $(DEBUG) $(INCLUDE) -Wall -DOS2
# Linux, SunOS
 CFLAGS		= $(DEBUG) $(INCLUDE) -Wall

# NEXTSTEP 3.3
# LFLAGS	= $(DEBUG) -L$(TOPDIR)/src/common -posix
# OS2
# LFLAGS	= -Zexe $(DEBUG) -L$(TOPDIR)/src/common
 LFLAGS		= $(DEBUG) -L$(TOPDIR)/src/common

# ISC 3.x
# LIBS		= -lfidogate -linet -lPW -lcposix
 LIBS		= -lfidogate

# installation program
# ISC 3.x: use bsdinst
 INSTALL	= install
# MSDOS, OS2, WIN32
 EXE		= .exe
# EXE		=
 INSTALL_PROG	= install -c -g $(GROUP) -o $(OWNER) -m $(PERM_PROG)
 INSTALL_DATA	= install -c -g $(GROUP) -o $(OWNER) -m $(PERM_DATA)
 INSTALL_SETUID	= install -c -g $(GROUP) -o $(OWNER) -m $(PERM_SETUID)
 INSTALL_DIR	= install -g $(GROUP) -o $(OWNER) -m $(PERM_DIR) -d

# library name
# LIB		= fidogate.a			# OS2
 LIB		= libfidogate.a