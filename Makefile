#get macports
#install libsdl

CC=g++

INCLUDE_PATHS=-L/usr/local/Cellar/boost/1.53.0/lib -L/usr/local/Cellar/boost/1.54.0/lib -L/usr/local/Cellar/sdl_ttf/2.0.11/lib -I/usr/local/Cellar/sdl/1.2.15/include/sdl -I/usr/local/Cellar/sdl_ttf/2.0.11/include -L/usr/local/Cellar/sdl/1.2.15/lib -I/usr/local/Cellar/boost/1.54.0/include -I/usr/local/Cellar/sdl/1.2.15/include/ -I/opt/local/include 

CFLAGS=-framework Cocoa -O3 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lboost_system-mt -lboost_thread-mt  -lSDL -lSDL_ttf -lSDLmain -I/opt/local/lib 

CPPFILES=$(wildcard *.cpp)
HPPFILES=$(wildcard *.hpp)

all : blox

blox : $(CPPFILES) $(HPPFILES) 
	$(CC) $(INCLUDE_PATHS) $(LDFLAGS) $(CFLAGS) $(CPPFILES) -o blox

run : blox.cpp blox
	-./blox

clean :
	-rm -f blox
