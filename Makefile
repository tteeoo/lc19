CC = gcc
CFLAGS = -Wall -pedantic -L/usr/lib -lssl -lcrypto
OBJFILES = lc19.o args.o ssl.o endpoint.o
TARGET = lc19
PREFIX = /usr/local

.phony: all clean install

all: ${TARGET}

clean:
	rm -f ${OBJFILES} ${TARGET}

install: ${TARGET}
	cp ${TARGET} ${PREFIX}/bin/

${TARGET}: ${OBJFILES}
	${CC} -o ${TARGET} ${OBJFILES} ${CFLAGS}
