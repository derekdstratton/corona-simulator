CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3 -std=c++11

TARGETS = mpi

mpi: mpi.o area.o particle.o Timer.o
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) area.o particle.o Timer.o

mpi.o: mpi.cpp area.h particle.h Timer.h 
	$(MPCC) -c $(CFLAGS) mpi.cpp

clean:
	rm -f *.o $(TARGETS)
