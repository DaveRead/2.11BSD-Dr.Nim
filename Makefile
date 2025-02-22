DESTCGI= 	/home/www/cgi-bin/drnim_cgi
DESTIMG= 	/home/www/images/marble.png
CFLAGS= -O
PROGRAM=	drnim_webcgi
SRCS=		drnim_webcgi.c
OBJS=		drnim_webcgi.o
LIBS=
MARBLEPNG=	marble.png

all:	${PROGRAM}

${PROGRAM}:	${OBJS}
	${CC} ${CFLAGS} -o $@ ${OBJS} ${LIBS}

install: ${PROGRAM}
	install -s -m 755 ${PROGRAM} ${DESTCGI}
	install -m 644 ${PROGRAM} ${DESTIMG}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
