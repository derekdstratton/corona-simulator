CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3 -std=c++11

TARGETS = mpi

mpi: mpi.o area.o particle.o Timer.o
	$(MPCC) -o $@ $(LIBS) $(OPENMP) $(MPILIBS) mpi.o area.o particle.o Timer.o

mpi.o: mpi.cpp area.cpp particle.cpp Timer.cpp
	$(MPCC) -c $(OPENMP) $(CFLAGS) mpi.cpp

clean:
	rm -f *.o $(TARGETS)
