# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDES = -I/opt/homebrew/opt/openssl@3/include

# Executables
EXEC1 = killme_pt1
EXEC2 = killme_pt2
EXEC3 = killme_pt3

# Source files
SRC1 = killme_pt1.c
SRC2 = killme_pt2.c
SRC3 = killme_pt3.c
SRC4 = critical_section.c

# Object files
OBJ1 = killme_pt1.o
OBJ2 = killme_pt2.o
OBJ3 = killme_pt3.o
OBJ4 = critical_section.o

# Default target: Build all parts
all: $(EXEC1) $(EXEC2) $(EXEC3)

# Compile Part 1
$(EXEC1): $(OBJ1)
	$(CC) $(CFLAGS) -o $(EXEC1) $(OBJ1)

$(OBJ1): $(SRC1)
	$(CC) $(CFLAGS) -c $(SRC1)

# Compile Part 2
$(EXEC2): $(OBJ2)
	$(CC) $(CFLAGS) -o $(EXEC2) $(OBJ2)

$(OBJ2): $(SRC2)
	$(CC) $(CFLAGS) -c $(SRC2)

# Compile critical_section.o (needed for Part 3)
$(OBJ4): $(SRC4)
	$(CC) $(CFLAGS) -c $(SRC4)

# Compile Part 3 (links with critical_section.o)
$(EXEC3): $(OBJ3) $(OBJ4)
	$(CC) $(CFLAGS) -o $(EXEC3) $(OBJ3) $(OBJ4)

$(OBJ3): $(SRC3)
	$(CC) $(CFLAGS) -c $(SRC3)

# Clean up compiled files
clean:
	rm -f $(EXEC1) $(EXEC2) $(EXEC3) $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
