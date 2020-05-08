CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3

TARGETS = mpi

mpi: mpi.o common.o
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) mpi.o common.o

mpi.o: mpi.cpp common.h
	$(MPCC) -c $(CFLAGS) mpi.cpp

clean:
	rm -f *.o $(TARGETS)