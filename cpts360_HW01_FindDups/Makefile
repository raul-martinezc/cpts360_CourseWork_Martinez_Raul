# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -I/opt/homebrew/opt/openssl@3/include
LDFLAGS = -L/opt/homebrew/opt/openssl@3/lib -lcrypto

# Output binary name
TARGET = finddups

# Source file
SRC = main.c

# Default rule (compiles the program)
all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule (removes compiled binary)
clean:
	rm -f $(TARGET)
