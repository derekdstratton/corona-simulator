# Sets the cmake version required
cmake_minimum_required(VERSION 2.8)

# Names the project
project(corona)

# Binary files will output to the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

# Sets flags, to be used for different targets
set(COMMON_FLAGS, " -std=c++11 -O3 -Wall -Wextra -pedantic ") # ALL will have these
set(SERIAL_FLAGS, "")
set(OPENMP_FLAGS, " -fopenmp ")
set(MPI_FLAGS, "")
set(GPU_FLAGS, "")
set(AUTOGRADER_FLAGS, "")

# For changing compiler
set(COMMON_COMPILER, "mpixx")

# Actually set common flags
add_compile_options("${COMMON_FLAGS}")

# Set compiler to g++
#set(CMAKE_CXX_COMPILER, "${COMMON_COMPILER}")

# Set shared libraries to be compiled only once.
#   STATIC -> .a
#   SHARED -> .so
add_library(area STATIC area.cpp area.h particle.h Timer.cpp Timer.h)

add_executable(serial serial.cpp area.h particle.cpp particle.h Timer.cpp Timer.h)

find_package(MPI REQUIRED)

include_directories(${MPI_INCLUDE_PATH})

# Make target serial
add_executable(mpi mpi.cpp area.h particle.cpp particle.h Timer.cpp Timer.h)

target_link_libraries(mpi common ${MPI_LIBRARIES})
if(MPI_COMPILE_FLAGS)
    set_target_properties(mpi PROPERTIES
            COMPILE_FLAGS "${MPI_COMPILE_FLAGS}")
endif()

if(MPI_LINK_FLAGS)
    set_target_properties(mpi PROPERTIES
            LINK_FLAGS "${MPI_LINK_FLAGS}")
endif()









