PREFIX          = /usr/local
TARGETS		= epgdump
OBJ_TARGETS	= epgdump.o aribstr.o eit.o ts.o util.o sdt.o tdt.o
HEDDERDEPEND	= eit.h sdt.h aribstr.h ts.h util.h xmldata.c

LANG=C
CC		= gcc
#CFLAGS		=  -std=c99 -O2 -Wall -g -Werror -Wno-return-type -I/usr/local/include -DDEBUG
CFLAGS		=  -std=c99 -O2 -Wall -g -Wno-return-type -I/usr/local/include -DDEBUG1
#LDFLAGS		= -pthread
#LIBS		=

.c.o:
	${CC} ${CFLAGS} -c $<

all:		${TARGETS}


${TARGETS}:	${OBJ_TARGETS}
		${CC} ${CFLAGS} ${OBJ_TARGETS} -o $@ ${LDFLAGS} ${LIBS}

${OBJ_TARGETS}:	${HEDDERDEPEND}

clean:
		rm -f core ${TARGETS} *.o

install:
	install -m 755 ${TARGETS} ${PREFIX}/bin
