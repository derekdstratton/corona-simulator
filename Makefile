CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3 -std=c++11

TARGETS = mpi

mpi: mpi.o area.h particle.h Timer.h
	$(MPCC) -o $@ $(LIBS) $(MPILIBS) area.h particle.h Timer.h

mpi.o: mpi.cpp area.h particle.h Timer.h 
	$(MPCC) -c $(CFLAGS) mpi.cpp

clean:
	rm -f *.o $(TARGETS)
