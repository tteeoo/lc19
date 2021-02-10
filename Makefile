CC = gcc
TARGET = lc19
CFLAGS = -Wall -I. -L/usr/lib -lssl -lcrypto
OBJFILES = lc19.o args.o

.phony: all clean install

all: ${TARGET}

clean:
	rm -f ${OBJFILES} ${TARGET}

install:
	cp ${TARGET} /usr/local/bin/

${TARGET}: ${OBJFILES}
	${CC} ${CFLAGS} -o ${TARGET} ${OBJFILES}
