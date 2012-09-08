## Process this file with automake to produce Makefile.in

bin_PROGRAMS = epgdump

epgdump_SOURCES = \
aribstr.c \
eit.c \
epgdump.c \
nit.c \
sdt.c \
bit.c \
ts.c \
tdt.c \
util.c

noinst_HEADERS = \
aribstr.h \
eit.h \
nit.h \
sdt.h \
bit.h \
tdt.h \
util.h \
ts.h \
ts_ctl.h \
xmldata.c

EXTRA_DIST = \
readme.txt

epgdump_LDFLAGS = @LIBICONV@
epgdump_CFLAGS =

