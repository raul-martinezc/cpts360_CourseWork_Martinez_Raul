PROG = $(shell basename $(PWD))

HDRS_IMPORTED =

HDRS_HERE = \
	snapshot.h

SRCS = \
	main.c \
	snapshot.c

OBJS = $(SRCS:.c=.o)

MAKETYPE = DEBUG

CC = cc

OSNAME = LINUX

SYSENV_DEFS = -DOSNAME_$(OSNAME)

CFLAGS = $(CFLAGS_$(MAKETYPE)) $(INCLIST) $(SYSENV_DEFS)
CFLAGS_DEBUG = -g $(CFLAGS_DEBUG_$(OSNAME))
CFLAGS_DEBUG_SOLARIS = -O
CFLAGS_DEBUG_LINUX = -Wall -Wstrict-prototypes
CFLAGS_OPTIMIZED = -O -DNDEBUG

LD = $(CC)

LDFLAGS = $(LDFLAGS_$(MAKETYPE)) $(LDFLAGS_$(OSNAME))
LDFLAGS_DEBUG = -g
LDFLAGS_OPTIMIZED =

default: $(PROG)

clean:
	rm -f a.out core $(OBJS) $(GENFILES) $(PROG) Makefile.BAK *.bak *~

immaculate: clean
	rm -f $(PROG)

$(OBJS): $(HDRS_HERE) $(HDRS_IMPORTED)

$(PROG): $(OBJS)
	@echo $(LD) $(LDFLAGS) -o $(PROG) $(OBJS)
	@if $(LD) $(LDFLAGS) -o $(PROG) $(OBJS) ;\
	then \
		echo $(PROG) linked ;\
	else \
		echo errors in link, $(PROG) executable removed ;\
		/bin/rm $(PROG) ;\
	fi

