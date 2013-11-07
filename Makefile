#get macports
#install libsdl

CC=g++

LIB_PATHS= \
-L/usr/local/Cellar/boost/1.53.0/lib \
-L/usr/local/Cellar/boost/1.54.0/lib \
-L/usr/local/Cellar/sdl_ttf/2.0.11/lib \
-L/usr/local/Cellar//sdl_image/1.2.12/lib 
INCLUDE_PATHS= \
-I/usr/local/Cellar/sdl/1.2.15/include/sdl \
-I/usr/local/Cellar/sdl_ttf/2.0.11/include \
-L/usr/local/Cellar/sdl/1.2.15/lib \
-I/usr/local/Cellar/boost/1.53.0/include \
-I/usr/local/Cellar/boost/1.54.0/include \
-I/usr/local/Cellar/sdl/1.2.15/include/ \
-I/usr/local/Cellar//sdl_image/1.2.12/include/ \
-I/opt/local/include \
-I/opt/local/lib


CFLAGS=-framework Cocoa -framework OpenGL -O3 -Wall -mmacosx-version-min=10.5
LDFLAGS=-lSDL_image -lboost_system-mt -lboost_thread-mt -lSDL -lSDL_ttf -lSDLmain

CPPFILES=$(wildcard *.cpp)
HPPFILES=$(wildcard *.hpp)

all : blox

blox : $(CPPFILES) $(HPPFILES) 
	$(CC) $(INCLUDE_PATHS) $(LIB_PATHS) $(LDFLAGS) $(CFLAGS) $(CPPFILES) -o blox

tex : tex.c
	$(CC) $(INCLUDE_PATHS) $(LIB_PATHS) $(LDFLAGS) $(CFLAGS) tex.c -o tex

run : blox.cpp blox
	-./blox

clean :
	-rm -f blox
