CC = gcc
TARGET = lc19
OBJFILES = lc19.o args.o ssl.o

PREFIX = /usr/local

CFLAGS = -Wall -pedantic -L/usr/lib -lssl -lcrypto

.phony: all clean install

all: ${TARGET}

clean:
	rm -f ${OBJFILES} ${TARGET}

install:
	cp ${TARGET} ${PREFIX}/bin/

${TARGET}: ${OBJFILES}
	${CC} -o ${TARGET} ${OBJFILES} ${CFLAGS}
