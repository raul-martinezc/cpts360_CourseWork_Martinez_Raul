# Force the use of GCC
CC = gcc

# Define the executables
BINS = perm permute

# Compiler flags
CFLAGS = -g -Wall -Wstrict-prototypes
LDFLAGS = -g

.PHONY: default
default: $(BINS)

# Compile perm
perm.o: perm.c
	$(CC) $(CFLAGS) -c $< -o $@

perm: perm.o
	$(CC) $(LDFLAGS) $^ -o $@

# Compile permute
permute.o: permute.c gen_perms.h
	$(CC) $(CFLAGS) -c $< -o $@

gen_perms.o: gen_perms.c gen_perms.h
	$(CC) $(CFLAGS) -c $< -o $@

permute: permute.o gen_perms.o
	$(CC) $(LDFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f core* *.o *~

.PHONY: immaculate
immaculate: clean
	rm -f $(BINS)
