#get macports
#install libsdl

CC=g++
CFLAGS=-I/opt/local/include -framework Cocoa -O2 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL -lSDLmain -I/opt/local/lib 

all : blox


blox : Blox.cpp  Window.cpp Entity.cpp CellMatrix.cpp  
	$(CC) *.cpp $(LDFLAGS) $(CFLAGS) -o blox

run : blox.cpp blox
	-./blox

clean :
	-rm -f blox
