# -*- Makefile -*-
#
# $Id: config.make,v 4.13 1998/01/18 10:57:55 mj Exp $
#
# FIDOGATE main configuration
#

##############################################################################
##############################################################################
#
# Settings used by subst.pl, default values for directories
# (subst.pl will use DEFAULT_XXX and ABBREV_XXX macros)
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
# Outbound/inbound base	DEFAULT_BTBASEDIR	BTBaseDir	%B
#
# Inbound		DEFAULT_INBOUND		Inbound		%I
# Protected inbound	DEFAULT_PINBOUND	PInbound	%P
# Uuencode inbound	DEFAULT_UUINBOUND	UUInbound	%U
# FTP inbound		DEFAULT_FTPINBOUND	FTPInbound
#
# INN config		DEFAULT_NEWSETCDIR	NewsEtcDir
# INN var lib		DEFAULT_NEWSVARDIR	NewsVarDir
# INN main lib		DEFAULT_NEWSLIBDIR	NewsLibDir
# INN spool		DEFAULT_NEWSSPOOL	NewsSpoolDir
#
# Ifmail main lib	DEFAULT_IFMAILDIR	IfmailDir
#
# DON'T REMOVE ANY OF THE DEFINITIONS BELOW!!!
# C CODE IS AUTOMATICALLY GENERATED FOR ALL CONFIG PARAMETERS DEFINED HERE!!!
#

# directories
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

# directory abbreviations
ABBREV_CONFIGDIR	= %C
ABBREV_LIBDIR		= %L
ABBREV_BINDIR		= %N
ABBREV_LOGDIR		= %G
ABBREV_VARDIR		= %V
ABBREV_LOCKDIR		= %K
ABBREV_SPOOLDIR		= %S
ABBREV_BTBASEDIR	= %B

ABBREV_INBOUND		= %I
ABBREV_PINBOUND		= %P
ABBREV_UUINBOUND	= %U

# files
DEFAULT_CONFIG_GATE	= %C/config.gate
DEFAULT_CONFIG_MAIN	= %C/config.main
DEFAULT_CONFIG_FFX	= %C/config.ffx

DEFAULT_ALIASES		= %C/aliases
DEFAULT_AREAS		= %C/areas
DEFAULT_HOSTS		= %C/hosts
DEFAULT_PASSWD		= %C/passwd
DEFAULT_PACKING		= %C/packing
DEFAULT_ROUTING		= %C/routing
DEFAULT_HISTORY		= %V/history
DEFAULT_LOGFILE		= %G/log

#
# In addition, the perl interpreter used by subst.pl
#
# perl
PERL		= /usr/bin/perl

##############################################################################
##############################################################################

# The old names back again ... (to be out-phased)
BINDIR		= $(DEFAULT_BINDIR)
LIBDIR		= $(DEFAULT_LIBDIR)
SPOOLDIR	= $(DEFAULT_SPOOLDIR)
LOGDIR		= $(DEFAULT_LOGDIR)
OUTBOUND	= $(DEFAULT_BTBASEDIR)
INBOUND		= $(DEFAULT_INBOUND)
PINBOUND	= $(DEFAULT_PINBOUND)
UUINBOUND	= $(DEFAULT_UUINBOUND)
NEWSETCDIR	= $(DEFAULT_NEWSETCDIR)
NEWSVARDIR	= $(DEFAULT_NEWSVARDIR)
NEWSLIBDIR	= $(DEFAULT_NEWSLIBDIR)
NEWSSPOOLDIR	= $(DEFAULT_NEWSSPOOLDIR)
IFMAILDIR       = $(DEFAULT_IFMAILDIR)


# Directories for installing documentation
INFODIR		= /usr/local/info
HTMLDIR		= /home/mj/public_html/fidogate

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

INCLUDE		= -I$(TOPDIR) -I$(TOPDIR)/src/include

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
INSTALL		= install
# MSDOS, OS2, WIN32
# EXE		= .exe
EXE		=
INSTALL_PROG	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_PROG)
INSTALL_DATA	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_DATA)
INSTALL_SETUID	= $(INSTALL) -c -g $(GROUP) -o $(OWNER) -m $(PERM_SETUID)
INSTALL_DIR	= $(INSTALL) -g $(GROUP) -o $(OWNER) -m $(PERM_DIR) -d
# extra prefix for installation
PREFIX		=

# library name
# LIB		= fidogate.a			# OS2
LIB		= libfidogate.a
