CC = g++
CFLAGS = -O3 -Wall 

all: OpenGL Main

Main: Main.c
	$(CC) $(CFLAGS) Main.c -o Main.exe

OpenGL: OpenGL.cpp
	$(CC) $(CFLAGS) OpenGL.cpp -lGL -lglut -o OpenGL.exe