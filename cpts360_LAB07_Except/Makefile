# Compiler
CC = gcc
CFLAGS = -Wall -Wextra -g

# Object files
OBJS = except.o

# Part 1: Compile except_demo
except_demo: except_demo.o $(OBJS)
	$(CC) $(CFLAGS) -o except_demo except_demo.o $(OBJS)

except_demo.o: except_demo.c except.h
	$(CC) $(CFLAGS) -c except_demo.c

# Part 2: Compile find_file
find_file: find_file.o $(OBJS)
	$(CC) $(CFLAGS) -o find_file find_file.o $(OBJS)

find_file.o: find_file.c except.h
	$(CC) $(CFLAGS) -c find_file.c

# Compile exception handling code
except.o: except.c except.h
	$(CC) $(CFLAGS) -c except.c

# Clean build files
clean:
	rm -f *.o except_demo find_file
