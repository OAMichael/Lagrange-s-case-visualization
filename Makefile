CC = g++
CFLAGS = -O3 -Wall 

all: OpenGL test

test: test.c
	$(CC) $(CFLAGS) test.c -o test

OpenGL: OpenGL.cpp
	$(CC) $(CFLAGS) OpenGL.cpp -lGL -lglut -o OpenGL.exe