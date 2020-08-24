CFLAGS = -Wall -lm -ansi 

SRC= $(wildcard *.c)

ALL:
	gcc $(SRC) $(CFLAGS) -o uwu
