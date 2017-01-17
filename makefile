# Eduardo Ponce
# 1/2017
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
# -mavx, -mavx2, -mavx512bw, -mavx512f, -mavx512pf, -mavx512er, -mavx512cd = SIMD extensions
# -mno-fma = disable fused-multiply add (FMA)
# -funroll-loops = enable loop unrolling
# -fopenmp, -fopenmp-simd = enable OpenMP
# -pthread = enable pthreads
# -std= = C/C++ language standard
SIMDFLAG := -msse4.1
#SIMDFLAG := -mavx2
#CFLAGS := $(SIMDFLAG) -g -pedantic -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result -std=c++11 -O3 -march=native -funroll-loops
CFLAGS := $(SIMDFLAG) -g -pedantic -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result -std=c++98 -O3 -march=native -funroll-loops
#CFLAGS += -pthread -fopenmp

# INTEL compiler and linker options
#CFLAGS := $(SIMDFLAG) -g -pedantic -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result -no-gcc -std=c++11 -O3 -march=native -funroll-loops
#CFLAGS := $(SIMDFLAG) -g -pedantic -Wall -Wextra -Wno-unknown-pragmas -Wno-unused-result -std=c++98 -O3 -march=native -funroll-loops
#CFLAGS += -pthread -openmp

# Linker options
LFLAGS :=

# Preprocessor definitions
# -DSIMD_MODE, -DAVX512BW_SPRNG, -DAVX2_SPRNG, -DSSE4_1_SPRNG = select SPRNG vector mode
# -DDEBUG = enable debugging
#
# -D_GNU_SOURCE = feature test macro (POSIX C and ISOC99)
#
# -DOMP_NUM_THREADS=x = number of OpenMP threads
# -DOMP_NESTED=TRUE = enables nested parallelism
# -DOMP_PROC_BIND=TRUE = thread/processor affinity
# -DOMP_STACKSIZE=8M = stack size for non-master threads
#DEFINES := -DLONG_SPRNG  # scalar mode
#DEFINES := -DSIMD_MODE -DLONG_SPRNG  # auto SIMD mode
#DEFINES := -DSIMD_MODE
DEFINES := -DSSE4_1_SPRNG -DLONG_SPRNG  # SSE4.1 SIMD mode
#DEFINES := -DSSE4_1_SPRNG
#DEFINES := -DAVX2_SPRNG -DLONG_SPRNG  # AVX2 SIMD mode
#DEFINES := -DAVX2_SPRNG
#DEFINES := -DAVX512_SPRNG -DLONG_SPRNG  # AVX512BW SIMD mode
#DEFINES := -DAVX512_SPRNG
#DEFINES += -DOMP_PROC_BIND=TRUE -DOMP_NUM_THREADS=8

# Define header paths in addition to /usr/include
#INCDIR := -I/dir1 -I/dir2
INCDIR := -I. -Iarch -Iinterfaces -Iprimes -Itimers -Ilcg -Iutils -Isimd -Icheck
TINCDIR := -I. -Iarch -Isimd -Itests

# Define library paths in addition to /usr/lib
#LIBDIR := -L/dir1 -L/dir2
LIBDIR :=
TLIBDIR :=

# Define libraries to link into executable
# -lm = math library
LIBS := -lm
TLIBS := -lm

# Source files to compile
#SOURCES := lcg/lcg.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp check/check.cpp
SOURCES := lcg/lcg.cpp lcg/vlcg.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp utils/vutils.cpp check/check.cpp
TSOURCES := tests/test_simd.cpp

# Set makefile's VPATH to search for target/dependency files, sort to remove duplicates
VPATH := $(sort $(dir $(SOURCES)))

# Object files to link
OBJDIR := obj
OBJECTS := $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SOURCES)))
TTOPDIR := tests
TOBJDIR := $(TTOPDIR)/$(OBJDIR)
TOBJECTS := $(patsubst %.cpp, $(TOBJDIR)/%.o, $(notdir $(TSOURCES)))

# Header files
# NOTE: allow recompile if changed
HEADERS := $(SOURCES:.cpp=.h) arch/*.h interfaces/*.h masprng.h simd/*.h primes/primelist_32.h lcg/lcg_globals.h
THEADERS := $(TSOURCES:.cpp=.h) arch/*.h simd/*.h

# Driver file
LCG_DRIVER := drivers/driver.cpp
TEST_DRIVER := $(TTOPDIR)/driver.cpp

# Executable
LCG_EXE := rng
TEST_EXE := $(TTOPDIR)/test_simd 

#######################################

# Targets that are not real files to create
.PHONY: all force debug clean asm


all: $(LCG_EXE)

test: $(TEST_EXE)

# Allows to recompile (useful to vary options using environment variables)
force:
	@touch $(MKFILE) 
	@$(MAKE) -f $(MKFILE)

debug:
	@$(MAKE) force DEFINES="-DDEBUG $(DEFINES)" -f $(MKFILE)

# Compile sources into object files
# NOTE: HEADERS and MKFILE are placed here to allow recompilation after their modification
$(OBJDIR)/%.o: %.cpp $(HEADERS) $(MKFILE)
	@test ! -d $(OBJDIR) && mkdir $(OBJDIR) || true
	$(CC) $(CFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) -c $< -o $@ $(LIBS) 

$(TOBJDIR)/%.o: $(TTOPDIR)/%.cpp $(THEADERS) $(MKFILE)
	@test ! -d $(TOBJDIR) && mkdir $(TOBJDIR) || true
	$(CC) $(CFLAGS) $(DEFINES) $(TINCDIR) $(TLIBDIR) -c $< -o $@ $(TLIBS) 

# Link object files
$(LCG_EXE): $(OBJECTS) $(LCG_DRIVER)
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) $(LCG_DRIVER) -o $@ $(OBJECTS) $(LIBS) 

$(TEST_EXE): $(TOBJECTS) $(TEST_DRIVER)
	$(CC) $(CFLAGS) $(LFLAGS) $(DEFINES) $(TINCDIR) $(TLIBDIR) $(TEST_DRIVER) -o $@ $(TOBJECTS) $(TLIBS) 

asm:
	@$(MAKE) force CFLAGS="-S $(CFLAGS)" -f $(MKFILE)

clean:
	rm -rf $(LCG_EXE) $(OBJDIR) $(TEST_EXE) $(TOBJDIR)

