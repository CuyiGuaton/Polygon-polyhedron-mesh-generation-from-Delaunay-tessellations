CFLAGS = -Wall -lm -std=gnu99

SRC= $(wildcard *.c)

ALL:
	gcc $(SRC) $(CFLAGS) -o uwu
