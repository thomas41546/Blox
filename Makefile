#get macports
#install libsdl

CC=gcc
CFLAGS=-I/opt/local/include -framework Cocoa -std=c99 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL -lSDLmain -I/opt/local/lib

all : blox

blox : blox.c
	$(CC) blox.c $(LDFLAGS) $(CFLAGS) -o blox

run : blox
	-./blox

clean :
	-rm -f blox
