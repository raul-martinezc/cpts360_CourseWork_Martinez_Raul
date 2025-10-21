CC = gcc
CFLAGS = -Wall -Wextra -g

all: raw_copy

raw_copy: raw_copy.c syscall_check.h allocarray.h
	$(CC) $(CFLAGS) -o raw_copy raw_copy.c

clean:
	rm -f raw_copy
