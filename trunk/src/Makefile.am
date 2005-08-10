## Process this file with automake to produce Makefile.in

## Created by Anjuta - will be overwritten
## If you don't want it to overwrite it,
## 	Please disable it in the Anjuta project configuration

INCLUDES = $(PACKAGE_CFLAGS) $(WNCK_CFLAGS)

AM_CFLAGS = -W -Wall -g

bin_PROGRAMS = gkey

gkey_SOURCES = \
	gkey-binding.c \
	gkey-binding.h \
	gkey-activator.c \
	gkey-activator.h \
	gkey-action.c \
	gkey-action.h \
	gkey-mousemover.c \
	gkey-mousemover.h \
	eggaccelerators.c \
	eggaccelerators.h \
	config-parser.c \
	config-parser.h \
	gkey.c

gkey_LDFLAGS = 

gkey_LDADD = $(PACKAGE_LIBS)

