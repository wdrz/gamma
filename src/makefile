CFLAGS = -Wall -Wextra -std=c11 -O2
.PHONY: all clean

all: gamma

gamma: gamma.c gamma.h
	gcc -o gamma gamma.c

clean:
	rm -rf *.o gamma
