files = main.c
out = retrocomp

all:
	gcc -std=c99 $(shell pkg-config sdl2 --cflags --libs) -o $(out) $(files)
