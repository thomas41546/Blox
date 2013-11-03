#get macports
#install libsdl

CC=g++
CFLAGS=-I/opt/local/include -framework Cocoa -O2 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL -lSDLmain -I/opt/local/lib 

all : blox

blox : blox.cpp Entity.cpp CellMatrix.cpp Entity.h CellMatrix.h Globals.h
	$(CC) *.cpp $(LDFLAGS) $(CFLAGS) -o blox

run : blox.cpp blox
	-./blox

clean :
	-rm -f blox
