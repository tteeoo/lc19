CC = gcc
TARGET = lc19
OBJFILES = lc19.o args.o ssl.o

PREFIX = /usr/local

CFLAGS = -Wall -std=c99 -pedantic -I. -L/usr/lib -lssl -lcrypto

.phony: all clean install

all: ${TARGET}

clean:
	rm -f ${OBJFILES} ${TARGET}

install:
	cp ${TARGET} ${PREFIX}/bin/

${TARGET}: ${OBJFILES}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJFILES}
