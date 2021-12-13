CC = g++

CFLAGS = -O3 -Wall 

all: OpenGL

OpenGL: 
	$(CC) $(CFLAGS) OpenGL.cpp -lGL -lglut -o OpenGL.exe

