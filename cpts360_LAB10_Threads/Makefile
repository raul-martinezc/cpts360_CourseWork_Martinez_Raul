CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -lrt -lm -lpthread

all: experiment_tmm_pt1_tplt experiment_tmm_pt2_tplt

experiment_tmm_pt1_tplt: experiment_tmm_pt1_tplt.o mat_mul_pt1_tplt.o tspec_diff.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

experiment_tmm_pt2_tplt: experiment_tmm_pt2_tplt.o mat_mul_pt2_tplt.o tspec_diff.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

experiment_tmm_pt1_tplt.o: experiment_tmm_pt1_tplt.c a2d.h eprintf.h mat_mul_pt1_tplt.h
mat_mul_pt1_tplt.o: mat_mul_pt1_tplt.c mat_mul_pt1_tplt.h a2d.h tspec_diff.h
experiment_tmm_pt2_tplt.o: experiment_tmm_pt2_tplt.c a2d.h eprintf.h mat_mul_pt2_tplt.h
mat_mul_pt2_tplt.o: mat_mul_pt2_tplt.c mat_mul_pt2_tplt.h a2d.h tspec_diff.h
tspec_diff.o: tspec_diff.c tspec_diff.h

clean:
	rm -f *.o experiment_tmm_pt1_tplt experiment_tmm_pt2_tplt
