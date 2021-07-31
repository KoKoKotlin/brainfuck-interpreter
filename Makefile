CC 	= /usr/bin/gcc
CFLAGS1 = -Wall -o bin/bint
CFLAGS2 = -Wall -o bin/compile
LFLAGES = -lncurses

.PHONY: all test clean

interpret:
	$(CC) $(CFLAGS1) file_io.c interpret.c $(LFLAGES)

compile:
	$(CC) $(CFLAGS2) file_io.c compile.c 

all:
	$(CC) $(CFLAGS1) interpret.c file_io.c $(LFLAGES)
	$(CC) $(CFLAGS2) compile.c file_io.c

clean:
	rm -f bin/*