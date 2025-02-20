DESTDIRCGI= /home/www/cgi-bin
DESTDIRIMG=	/home/www/images
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
	mkdir -p ${DESTDIRCGI}
	chmod 755 ${DESTDIRCGI}
	install -s -m 755 ${PROGRAM} ${DESTDIRCGI}
	mkdir -p ${DESTDIRIMG}
	chmod 755 ${DESTDIRIMG}
	cp ${MARBLEPNG} ${DESTDIRIMG}
	chmod 644 ${DESTDIRIMG}/${MARBLEPNG}

tags:
	ctags -tdw *.c

clean:
	rm -f a.out core *.o ${PROGRAM}
