# -*- Makefile -*-
#
# $Id: config.make,v 4.11 1998/01/13 20:33:45 mj Exp $
#
# FIDOGATE Makefile configuration
#

##############################################################################
#
# Settings used by subst.pl, default values for directories
# (subst.pl will use DEFAULT_XXX macros for substition of <XXX>)
#
# Directory		Compile-time		Run-time	Abbrev
# ---------		------------		--------	------
# Config		DEFAULT_CONFIGDIR	ConfigDir	%C
# Main lib		DEFAULT_LIBDIR		LibDir		%L
# Admin utilities	DEFAULT_BINDIR		BinDir		%N
# Log			DEFAULT_LOGDIR		LogDir		%G
# Var lib		DEFAULT_VARDIR		VarDir		%V
# Lock files		DEFAULT_LOCKDIR		LockDir		%K
# Spool			DEFAULT_SPOOLDIR	SpoolDir	%S
# Outbound/inbound base	DEFAULT_BTBASEDIR	BTBaseDir	%B %O
#
# Inbound		DEFAULT_INBOUND		Inbound		%I
# Protected inbound	DEFAULT_PINBOUND	PInbound	%P
# Uuencode inbound	DEFAULT_UUINBOUND	UUInbound	%U
# FTP inbound		DEFAULT_FTPINBOUND	FTPInbound
#
# INN config		DEFAULT_NEWSETCDIR
# INN var lib		DEFAULT_NEWSVARDIR
# INN main lib		DEFAULT_NEWSLIBDIR
# INN spool		DEFAULT_NEWSSPOOL
#
# Ifmail main lib	DEFAULT_IFMAILDIR
#

DEFAULT_CONFIGDIR	= /etc/fidogate
DEFAULT_LIBDIR		= /usr/lib/fidogate
DEFAULT_BINDIR		= $(DEFAULT_LIBDIR)/bin
DEFAULT_LOGDIR		= /var/log/fidogate
DEFAULT_VARDIR		= /var/lib/fidogate
DEFAULT_LOCKDIR		= /var/lock/fidogate
DEFAULT_SPOOLDIR	= /var/spool/fidogate
DEFAULT_BTBASEDIR	= /var/spool/bt

DEFAULT_INBOUND		= $(DEFAULT_BTBASEDIR)/in
DEFAULT_PINBOUND	= $(DEFAULT_BTBASEDIR)/pin
DEFAULT_UUINBOUND	= $(DEFAULT_BTBASEDIR)/uuin
DEFAULT_FTPINBOUND	= $(DEFAULT_BTBASEDIR)/ftpin

DEFAULT_NEWSETCDIR	= /etc/news
DEFAULT_NEWSVARDIR	= /var/lib/news
DEFAULT_NEWSLIBDIR	= /usr/lib/news
DEFAULT_NEWSSPOOLDIR	= /var/spool/news

DEFAULT_IFMAILDIR       = /usr/local/lib/ifmail

#
# In addition, the perl interpreter used by subst.pl
#
# perl
PERL		= /usr/bin/perl

##############################################################################


# FIDOGATE Directories
BINDIR		= /usr/local/bin
LIBDIR		= /usr/local/lib/fidogate
SPOOLDIR	= /var/spool/fido
LOGDIR		= /var/log/fido

# Doc directories
INFODIR		= /usr/local/info
HTMLDIR		= /home/mj/public_html/fidogate

# Outbound *base* directory, i.e. the actual outbound is OUTBOUND/out.xxx
OUTBOUND	= /var/spool/bt
# Normal (i.e. insecure) inbound
INBOUND		= /var/spool/bt/in
# Protected inbound
PINBOUND	= /var/spool/bt/pin
# Inbound for uuencoded mails (used by recvuu)
UUINBOUND	= /var/spool/bt/uuin

# INN directories
NEWSETCDIR	= /etc/news
NEWSVARDIR	= /var/lib/news
NEWSLIBDIR	= /usr/lib/news
NEWSSPOOLDIR	= /var/spool/news

# Ifmail directory
IFMAILDIR       = /usr/local/lib/ifmail

# OS2: comment out
 SHELL		= /bin/sh

# m4
 M4		= m4

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
# EXE		= .exe
 EXE		=
 INSTALL_PROG	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_PROG)
 INSTALL_DATA	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_DATA)
 INSTALL_SETUID	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_SETUID)
 INSTALL_DIR	= $(INSTALL) -g $(GROUP) -o $(OWNER) -m $(PERM_DIR) -d
# extra prefix for installation
 PREFIX         =

# library name
# LIB		= fidogate.a			# OS2
 LIB		= libfidogate.a
