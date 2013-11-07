# Set compiler to g++, unless it is already set.
CXX ?= g++

# Flags
CXXFLAGS = -Wall -Wextra
ifeq ($(DEBUG),1)
  CXXFLAGS += -g -O0
else
  CXXFLAGS += -O3
endif
LDFLAGS =  ${CXXFLAGS}

# Define what extensions we use
.SUFFIXES : .cpp

# Name of executable
PROG_NAME = duplo

# List of object files
OBJS = StringUtil.o HashUtil.o ArgumentParser.o TextFile.o \
       SourceFile.o SourceLine.o Duplo.o FileType.o

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

