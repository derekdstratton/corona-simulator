CC = g++
MPCC = mpicxx
OPENMP = -fopenmp
CFLAGS = -O3 -std=c++11

TARGETS = mpi

# it only imports openmp for compatibility with particle.h seeding the RNG with the thread
serial: serial.o area.o particle.o Timer.o
    $(CC) -o $@ $(OPENMP) serial.o area.o particle.o Timer.o

serial.o: serial.cpp area.cpp particle.cpp Timer.cpp
    $(CC) -c $(OPENMP) $(CFLAGS) serial.cpp

mpi: mpi.o area.o particle.o Timer.o
	$(MPCC) -o $@ $(LIBS) $(OPENMP) $(MPILIBS) mpi.o area.o particle.o Timer.o

mpi.o: mpi.cpp area.cpp particle.cpp Timer.cpp
	$(MPCC) -c $(OPENMP) $(CFLAGS) mpi.cpp

clean:
	rm -f *.o $(TARGETS)
