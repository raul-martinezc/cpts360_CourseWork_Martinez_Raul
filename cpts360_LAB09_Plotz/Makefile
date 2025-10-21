# Makefile for Lab 09: Dynamically Sized 2D Arrays in C with Bounds Checking

CC = gcc
CFLAGS = -Wall -std=c99
LDFLAGS = -lm

# Build all program versions
all: plotz2_pt1 plotz2_pt2 plotz2_pt3 plotz2_pt4

# Part 1: Static 2D array
plotz2_pt1: plotz2_pt1.c
	$(CC) $(CFLAGS) plotz2_pt1.c -o plotz2_pt1 $(LDFLAGS)

# Part 2: Dynamic array with global size
plotz2_pt2: plotz2_pt2.c allocarray.h
	$(CC) $(CFLAGS) plotz2_pt2.c -o plotz2_pt2 $(LDFLAGS)

# Part 3: Dynamic array with command-line size
plotz2_pt3: plotz2_pt3.c allocarray.h
	$(CC) $(CFLAGS) plotz2_pt3.c -o plotz2_pt3 $(LDFLAGS)

# Part 4: With bounds checking using i2d.h
plotz2_pt4: plotz2_pt4.c allocarray.h i2d.h
	$(CC) $(CFLAGS) plotz2_pt4.c -o plotz2_pt4 $(LDFLAGS)

# Clean up build artifacts and generated images
clean:
	rm -f plotz2_pt1 plotz2_pt2 plotz2_pt3 plotz2_pt4 *.pgm
