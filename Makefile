CC = gcc
CFLAGS = -Wall `pkg-config --cflags openssl`
LDFLAGS = `pkg-config --libs openssl`
TARGET = lc19
PREFIX = /usr/local

objects = lc19.o args.o ssl.o endpoint.o

${TARGET}: ${objects}
	${CC} ${CFLAGS} -o ${TARGET} ${objects} ${LDFLAGS}

args.o lc19.o: args.h
ssl.o lc19.o: ssl.h 
endpoint.o lc19.o: endpoint.h

.PHONY: clean install
clean:
	rm -f ${objects} ${TARGET}

install: ${TARGET}
	cp ${TARGET} ${PREFIX}/bin/
