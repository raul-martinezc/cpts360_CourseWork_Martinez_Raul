# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# Executable names
TARGETS = codestats_pt1 codestats_pt2 codestats_pt3 codestats_pt4 codestats_pt5 codestats_pt6 codestats_pt7

# Default rule: build all executables
all: $(TARGETS)

# Compile each part
codestats_pt1: codestats_pt1.c
	$(CC) $(CFLAGS) -o codestats_pt1 codestats_pt1.c

codestats_pt2: codestats_pt2.c
	$(CC) $(CFLAGS) -o codestats_pt2 codestats_pt2.c

codestats_pt3: codestats_pt3.c
	$(CC) $(CFLAGS) -o codestats_pt3 codestats_pt3.c

codestats_pt4: codestats_pt4.c
	$(CC) $(CFLAGS) -o codestats_pt4 codestats_pt4.c

codestats_pt5: codestats_pt5.c
	$(CC) $(CFLAGS) -o codestats_pt5 codestats_pt5.c

codestats_pt6: codestats_pt6.c
	$(CC) $(CFLAGS) -o codestats_pt6 codestats_pt6.c

codestats_pt7: codestats_pt7.c
	$(CC) $(CFLAGS) -o codestats_pt7 codestats_pt7.c

# Clean build artifacts
clean:
	rm -f $(TARGETS)
