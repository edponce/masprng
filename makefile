# Eduardo Ponce
# 12/2016
# Makefile for MASPRNG library.

# Get name of makefile
MKFILE := $(MAKEFILE_LIST)

# C/C++ compiler
CC := g++
#CC := icpc

# GNU compiler and linker options
# -Wall, -Wextra = enable warnings
# -Werror = consider warnings as errors
# -g = debug info
# -O1, -O2, -O3 = optimization levels
# -march= = target architecture (implies -mtune=cpu-type)
# -mmx, -msse, -msse2, -msse3, -mssse3, -msse4.1, -msse4.2, -msse4 = SIMD extensions
# -mavx, -mavx2, -mavx512, -mavx512f, -mavx512pf, -mavx512er, -mavx512cd = SIMD extensions
# -funroll-loops = enable loop unrolling
# -fopenmp, -fopenmp-simd = enable OpenMP
# -pthread = enable pthreads
# -std= = C/C++ language standard
CFLAGS := -pedantic -Wall -Wextra -Wno-unknown-pragmas -std=c++11 -O3 -march=native -funroll-loops
#CFLAGS += -pthread
#CFLAGS += -pthread -fopenmp

# INTEL compiler and linker options
#CFLAGS := -pedantic -Wall -Wextra -Wno-unknown-pragmas -no-gcc -std=c++11 -O3 -msse4 -funroll-loops
#CFLAGS := -pedantic -Wall -Wextra -Wno-unknown-pragmas -no-gcc -std=c++11 -O3 -march=core-avx2 -funroll-loops
#CFLAGS := -pedantic -Wall -Wextra -Wno-unknown-pragmas -no-gcc -std=c++11 -O3 -march=native -funroll-loops
#CFLAGS += -pthread -openmp

# Linker options
LFLAGS :=

# Preprocessor definitions
# -DAVX_SPRNG, -DSSE_SPRNG = select SPRNG vector mode
# -DDEBUG = enable debugging
#
# -D_GNU_SOURCE = feature test macro (POSIX C and ISOC99)
#
# -DOMP_NUM_THREADS=x = number of OpenMP threads
# -DOMP_NESTED=TRUE = enables nested parallelism
# -DOMP_PROC_BIND=TRUE = thread/processor affinity
# -DOMP_STACKSIZE=8M = stack size for non-master threads
#DEFINES := -DLONG_SPRNG
DEFINES := -DSSE_SPRNG -DLONG_SPRNG
#DEFINES := -DSSE_SPRNG
#DEFINES := -DAVX_SPRNG -DLONG_SPRNG
#DEFINES += -DOMP_PROC_BIND=TRUE -DOMP_NUM_THREADS=8

# Define header paths in addition to /usr/include
#INCDIR := -I/dir1 -I/dir2
#INCDIR := -I. -Iinterfaces -Iprimes -Itimers -Ilcg -Iutils -Icheck
INCDIR := -I. -Iinterfaces -Iprimes -Itimers -Ilcg -Iutils -Isimd -Icheck

# Define library paths in addition to /usr/lib
#LIBDIR := -L/dir1 -L/dir2
LIBDIR :=

# Define libraries to link into executable
# -lm = math library
LIBS := -lm

# Source files to compile
#SOURCES := masprng.cpp lcg/lcg.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp
SOURCES := lcg/lcg.cpp lcg/lcg_simd.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp simd/sse.cpp check/check.cpp

# Object files to link
OBJECTS := $(SOURCES:.cpp=.o)

# Header files (allow recompile if changed)
HEADERS := $(SOURCES:.cpp=.h) sprng_config.h sprng.h interfaces/ssprng.h interfaces/vsprng.h primes/primelist_32.h lcg/lcg_config.h

# Driver file
#LCG_DRIVER := drivers/main_lcg.cpp
LCG_DRIVER := drivers/driver.cpp

# Executable
LCG_EXE := rng

#######################################

# Targets that are not real files to create
.PHONY: all force debug clean asm


all: $(LCG_EXE)

# Allows to recompile (useful to vary options using environment variables)
force:
	@touch $(MKFILE) 
	@$(MAKE) -f $(MKFILE)

debug:
	@$(MAKE) force DEFINES="-DDEBUG $(DEFINES)" -f $(MKFILE)

# Compile sources into object files
%.o: %.cpp $(LCG_DRIVER) $(HEADERS) $(MKFILE) 
	$(CC) $(CFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) -c $< -o $@ $(LIBS) 

# Link object files
$(LCG_EXE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) $(LCG_DRIVER) -o $@ $(OBJECTS) $(LIBS) 

asm:
	@$(MAKE) force CFLAGS="-S $(CFLAGS)" -f $(MKFILE)

clean:
	rm -f $(LCG_EXE) $(OBJECTS)

