files = main.c
out = emu65o2

all:
	gcc -std=c99 $(shell pkg-config sdl2 --cflags --libs) -o $(out) $(files)
