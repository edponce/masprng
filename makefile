# Eduardo Ponce
# 9/2018
# Makefile for MASPRNG library.

# Get name of makefile
MKFILE := $(MAKEFILE_LIST)

# C/C++ compiler
CXX ?= g++
#CXX ?= icpc
#CXX ?= clang++
#CXX ?= pgc++

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
# -fopenmp = enable OpenMP, implies -pthread and -fopenmp-simd
# -pthread = enable pthreads
# -std= = C/C++ language standard

ifeq ($(CXX),g++)
#SIMDFLAG := -mno-sse2 -mno-sse4.1 -mno-avx -mno-avx2
#SIMDFLAG := -msse2
#SIMDFLAG := -msse4.1
#SIMDFLAG := -mavx2
SIMDFLAG := -march=native
#CFLAGS := $(SIMDFLAG) -Wpedantic -pedantic-errors -Wall -Wextra -std=c++98 -O2
CFLAGS := $(SIMDFLAG) -Wpedantic -pedantic-errors -Wall -Wextra -Werror -std=c++98 -O2
CFLAGS += -funroll-loops
#CFLAGS += -fopenmp
LFLAGS :=


# INTEL compiler and linker options
else ifeq ($(CXX),icpc)
#SIMDFLAG := -no-simd
#SIMDFLAG := -xSSE2
#SIMDFLAG := -xSSE4.1
#SIMDFLAG := -xAVX
#SIMDFLAG := -xCORE-AVX-I
#SIMDFLAG := -xCORE-AVX2
SIMDFLAG := -xHost
CFLAGS := $(SIMDFLAG) -Wall -Wextra -Werror -std=c++98 -O2
# Flag -no-gcc disable gcc predefined macros
CFLAGS += -no-gcc -nostdinc -nostdinc++
CFLAGS += -funroll-loops
#CFLAGS += -qopenmp
LFLAGS :=

# Clang compiler and linker options
else ifeq ($(CXX),clang++)
#SIMDFLAG := -mno-sse2 -mno-sse4.1 -mno-avx -mno-avx2
#SIMDFLAG := -msse2
#SIMDFLAG := -msse4.1
#SIMDFLAG := -mavx2
SIMDFLAG := -march=native
#CFLAGS := $(SIMDFLAG) -pedantic -pedantic-errors -Weverything -Werror -Wno-reserved-id-macro -std=c++98 -O2
CFLAGS := $(SIMDFLAG) -pedantic -pedantic-errors -Weverything -std=c++98 -O2
CFLAGS += -funroll-loops
#CFLAGS += -fopenmp
LFLAGS :=

# PGI compiler and linker options
else ifeq ($(CXX),pgc++)
SIMDFLAG := -fast -Mvect=simd:128
#SIMDFLAG := -fast -Mvect=simd:256
#CFLAGS := $(SIMDFLAG) --pedantic -Minform=warn -std=c++03 -O2
CFLAGS := $(SIMDFLAG) --pedantic -Minform=inform -std=c++03 -O2
#CFLAGS += -Mnostdinc
CFLAGS += -Munroll
#CFLAGS += -mp
LFLAGS :=
endif

# Preprocessor definitions
# -DSIMD_MODE, -DAVX512BW_SPRNG, -DAVX2_SPRNG, -DSSE4_1_SPRNG = select SPRNG vector mode
# -DDEBUG = enable debugging
#
# -D_GNU_SOURCE = feature test macro (POSIX C and ISOC99)
# -D_POSIX_C_SOURCE=200112L
#
# -DOMP_NUM_THREADS=x = number of OpenMP threads
# -DOMP_NESTED=TRUE = enables nested parallelism
# -DOMP_PROC_BIND=TRUE = thread/processor affinity
# -DOMP_STACKSIZE=8M = stack size for non-master threads
#DEFINES := -DLONG_SPRNG  # scalar mode
#DEFINES := -DSIMD_MODE -DLONG_SPRNG  # auto SIMD mode
DEFINES := -DSIMD_MODE
#DEFINES := -DSSE4_1_SPRNG -DLONG_SPRNG  # SSE4.1 SIMD mode
#DEFINES := -DSSE4_1_SPRNG
#DEFINES := -DAVX2_SPRNG -DLONG_SPRNG  # AVX2 SIMD mode
#DEFINES := -DAVX2_SPRNG
#DEFINES := -DAVX512_SPRNG -DLONG_SPRNG  # AVX512BW SIMD mode
#DEFINES := -DAVX512_SPRNG
#DEFINES := -DOMP_PROC_BIND=TRUE -DOMP_NUM_THREADS=8
DEFINES += -D_POSIX_C_SOURCE=200112L

# Define header paths in addition to /usr/include
#INCDIR := -I/dir1 -I/dir2
INCDIR := -I. -Iarch -Iinterfaces -Iprimes -Itimers -Ilcg -Iutils -Isimd -Icheck
TINCDIR := -I. -Iarch -Isimd -Itests -Iutils
ifeq ($(CXX),icpc)
# If using standard headers, include path for bits/c++-config.h
#INCDIR += -I/opt/intel/include/icc -I/usr/include/x86_64-linux-gnu/c++/8
#TINCDIR += -I/opt/intel/include/icc -I/usr/include/x86_64-linux-gnu/c++/8
# Instead use -nostdinc++ flag and use Intel headers
#INCDIR += -I/opt/intel/include/icc
#TINCDIR += -I/opt/intel/include/icc
# Also use -nostdinc flag and use Intel headers
INCDIR += -I/usr/include -I/opt/intel/include -I/opt/intel/include/icc
TINCDIR += -I/usr/include -I/opt/intel/include -I/opt/intel/include/icc
endif

# Define library paths in addition to /usr/lib
#LIBDIR := -L/dir1 -L/dir2
LIBDIR :=
TLIBDIR :=
ifeq ($(CXX),icpc)
LIBDIR += -L/opt/intel/compilers_and_libraries/linux/lib/intel64
TLIBDIR += -L/opt/intel/compilers_and_libraries/linux/lib/intel64
endif

# Define libraries to link into executable
# -lm = math library
LIBS := -lm
TLIBS := -lm

# Source files to compile
#SOURCES := lcg/lcg.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp check/check.cpp
SOURCES := lcg/lcg.cpp lcg/vlcg.cpp primes/primes_32.cpp timers/timers.cpp utils/utils.cpp utils/vutils.cpp check/check.cpp
TSOURCES := tests/test_simd.cpp tests/test_utils.cpp

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
THEADERS := $(TSOURCES:.cpp=.h) arch/*.h simd/*.h $(TTOPDIR)/test_suite.h

# Driver file
LCG_DRIVER := drivers/driver.cpp
TEST_DRIVER := $(TTOPDIR)/test_suite.cpp

# Executable
LCG_EXE := rng
TEST_EXE := $(TTOPDIR)/testsuite

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
	$(CXX) $(CFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) -c $< -o $@

$(TOBJDIR)/%.o: $(TTOPDIR)/%.cpp $(THEADERS) $(MKFILE)
	@test ! -d $(TOBJDIR) && mkdir $(TOBJDIR) || true
	$(CXX) $(CFLAGS) $(DEFINES) $(TINCDIR) $(TLIBDIR) -c $< -o $@

# Link object files
$(LCG_EXE): $(OBJECTS) $(LCG_DRIVER)
	$(CXX) $(CFLAGS) $(LFLAGS) $(DEFINES) $(INCDIR) $(LIBDIR) $(LCG_DRIVER) -o $@ $(OBJECTS) $(LIBS)

$(TEST_EXE): $(OBJECTS) $(TOBJECTS) $(TEST_DRIVER)
	$(CXX) $(CFLAGS) $(LFLAGS) $(DEFINES) $(TINCDIR) $(TLIBDIR) $(TEST_DRIVER) -o $@ $(OBJECTS) $(TOBJECTS) $(TLIBS)

asm:
	@$(MAKE) force CFLAGS="-S $(CFLAGS)" -f $(MKFILE)

clean:
	rm -rf *.o $(LCG_EXE) $(OBJDIR) $(TEST_EXE) $(TOBJDIR)

