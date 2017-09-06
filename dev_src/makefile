# Makefile for MASPRNG library.

# C++ compiler
#CXX = icpc
CXX = g++

# Compiler options
CXXFLAGS = -g -O2 -Wall -Wno-deprecated -march=native -std=c++11

# Linker options
LDFLAGS =

# Preprocessor definitions
# -DUSE_MPI - enable MPI support
# -DSIMPLE_SPRNG - should be set in main source file before sprng.h
# -DFLOAT_GEN - if defined, use float instead of double
# -DLONG64 - [long] [long] int, use mainly for seed and multiplier values
# -DPOINTER_SIZE - [4] [8], used only for FORTRAN interface
DEFINES = -DVERBOSE -DLONG64=long -DSIMPLE_SPRNG
#DEFINES = -DVERBOSE -DSIMPLE_SPRNG
# -DUSE_AVX
# -DUSE_SSE

# Define directories containing header files in addition to /usr/include
IFLAGS = -I. -Ilcg -Ilcg64

# Define directories containing library files in addition to /usr/lib
LFLAGS =

# Define libraries to link into executable
LIBS = -lm

# Source files to compile
#SOURCES = basic.cpp bignum.cpp bigrat.cpp communicate.cpp cputime.cpp info.cpp memory.cpp primes_32.cpp primes_64.cpp sprng.cpp sprng_cpp.cpp store.cpp
SOURCES = memory.cpp primes_32.cpp primes_64.cpp sprng.cpp sprng_cpp.cpp store.cpp communicate.cpp cputime.cpp
SOURCES += lcg/lcg.cpp
SOURCES += lcg64/lcg64.cpp

# Object files to link
OBJECTS = $(SOURCES:.cpp=.o)

# Name of static library to create 
STATLIB = libsprng.a

# Main source files
MAIN_LCG = lcg/main_lcg.cpp
MAIN_LCG64 = lcg64/main_lcg64.cpp
MAIN_LFG = lfg/main_lfg.cpp
MAIN_MLFG = mlfg/main_mlfg.cpp
MAIN_CMRG = cmrg/main_cmrg.cpp
MAIN_PMLCG = pmlcg/main_pmlcg.cpp

# Timing source files
TIME_LCG = lcg/time_lcg.cpp
TIME_LCG64 = lcg64/time_lcg64.cpp
TIME_LFG = lfg/time_lfg.cpp
TIME_MLFG = mlfg/time_mlfg.cpp
TIME_CMRG = cmrg/time_cmrg.cpp
TIME_PMLCG = pmlcg/time_pmlcg.cpp

# Executables
MAIN_EXE = main_lcg main_lcg64
TIME_EXE = time_lcg time_lcg64

#######################################

.PHONY: all clean main time


all: $(MAIN_EXE) $(TIME_EXE)


$(STATLIB): $(OBJECTS)
	ar rcs $@ $^

%.o: %.cpp makefile
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) -c $< -o $@ $(LIBS) 


main: $(MAIN_EXE)

main_lcg: $(STATLIB) $(MAIN_LCG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_LCG) -o $@ $(STATLIB) $(LIBS) 

main_lcg64: $(STATLIB) $(MAIN_LCG64)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_LCG64) -o $@ $(STATLIB) $(LIBS) 

main_lfg: $(STATLIB) $(MAIN_LFG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_LFG) -o $@ $(STATLIB) $(LIBS) 

main_mlfg: $(STATLIB) $(MAIN_MLFG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_MLFG) -o $@ $(STATLIB) $(LIBS) 

main_cmrg: $(STATLIB) $(MAIN_CMRG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_CMRG) -o $@ $(STATLIB) $(LIBS) 

main_pmlcg: $(STATLIB) $(MAIN_PMLCG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(MAIN_PMLCG) -o $@ $(STATLIB) $(LIBS) 


time: $(TIME_EXE)

time_lcg: $(STATLIB) $(TIME_LCG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_LCG) -o $@ $(STATLIB) $(LIBS) 

time_lcg64: $(STATLIB) $(TIME_LCG64)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_LCG64) -o $@ $(STATLIB) $(LIBS) 

time_lfg: $(STATLIB) $(TIME_LFG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_LFG) -o $@ $(STATLIB) $(LIBS) 

time_mlfg: $(STATLIB) $(TIME_MLFG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_MLFG) -o $@ $(STATLIB) $(LIBS) 

time_cmrg: $(STATLIB) $(TIME_CMRG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_CMRG) -o $@ $(STATLIB) $(LIBS) 

time_pmlcg: $(STATLIB) $(TIME_PMLCG)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(IFLAGS) $(LFLAGS) $(TIME_PMLCG) -o $@ $(STATLIB) $(LIBS) 


clean:
	rm -f *.o
	rm -f lcg/*.o
	rm -f lcg64/*.o
	rm -f $(MAIN_EXE) $(TIME_EXE) $(STATLIB)


