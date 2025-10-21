CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: tattle

tattle: tattle.o
	$(CC) $(CFLAGS) -o tattle tattle.o

tattle.o: tattle.c
	$(CC) $(CFLAGS) -c tattle.c

clean:
	rm -f tattle *.o
