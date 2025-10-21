CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = whocan
OBJ = whocan.o permissions.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

whocan.o: whocan.c permissions.h
	$(CC) $(CFLAGS) -c whocan.c

permissions.o: permissions.c permissions.h
	$(CC) $(CFLAGS) -c permissions.c

clean:
	rm -f $(TARGET) $(OBJ)
