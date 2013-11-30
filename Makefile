# Set compiler to g++, unless it is already set.
CXX ?= g++

# Flags
CXXFLAGS += -Wall -Wextra
ifeq ($(DEBUG),1)
  CXXFLAGS += -g -O0
  LDFLAGS += -g
else
  CXXFLAGS += -O3
endif

# Useful for building a Windows executable.
ifeq ($(STATIC),1)
  LDFLAGS += -static -static-libstdc++
endif

# Define what extensions we use
.SUFFIXES : .cpp

.PHONY: clean all

# Name of executable
PROG_NAME = duplo

# List of object files
OBJS = StringUtil.o HashUtil.o TextFile.o FileType.o \
       SourceFile.o SourceLine.o Duplo.o main.o

# Build process

all: ${PROG_NAME}

# Link
${PROG_NAME}: ${OBJS}
	${CXX} ${LDFLAGS} -o ${PROG_NAME} ${OBJS}

# Each .cpp file compile
.cpp.o:
	${CXX} ${CXXFLAGS} -c $*.cpp -o$@

# Remove all object files and the executable
clean:	
	rm -f *.o duplo

