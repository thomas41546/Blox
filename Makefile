#get macports
#install libsdl

CC=g++
CFLAGS=-I/opt/local/include -framework Cocoa -g -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL -lSDLmain -I/opt/local/lib 

all : blox

blox : blox.cpp
	$(CC) blox.cpp $(LDFLAGS) $(CFLAGS) -o blox

run : blox
	-./blox

clean :
	-rm -f blox
