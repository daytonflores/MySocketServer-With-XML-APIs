# C Compiler
CC= g++

# Check if Unix or Windows, define CLEAN based on OS
ifeq (OS, Windows_NT)
	CLEAN=del /s *.o *.d *~ $(TARGET).exe 
else
 	CLEAN=rm -rf *.o *.d *~ $(TARGET)
endif

# Header Directory
#	 Example 1 : If Makefile resides in ..Project and header files reside in ..Project, then value here would be left empty
#	 Example 2 : If Makefile resides in ..Project and header files reside in ..Project/headers, then value here would be "-Iheaders" without quotes
#	 Example 3 : If Makefile resides in ..Project/source and header files reside in ..Project/headers, then value here would be "-I../headers" without quotes
HDIR= -I../headers

# Source Directory
#	 Example 1 : If Makefile resides in ..Project and source files reside in ..Project/source, then value here would be "-Isrc" without quotes
#	 Example 2 : If Makefile resides in ..Project/source and source files reside in ..Project/source, then value here would be left empty
SRCDIR=

# Link Libraries
#	 -lm       : Link with libm
#	 -lpthread : Link with libpthread
#	 -lrt      : Link with librt
LINKLIBS=

# Compiler Flags
#	 -g      : adds debugging information to the executable file
#	 -Wall   : turns on most, but not all, compiler warnings
#	 -Wextra : turns on some, but not all, compiler warnings not controlled by -Wall
#	 -Werror : makes all warnings into errors
#	 -DDEBUG : flag to set a define for DEBUG
CFLAGS= -g -Wall -Werror ${HDIR} ${SRCDIR} -DDEBUG

# Name of Build Target
TARGET= main

# C++ Files
CPPFILES= $(wildcard *.cpp)

# Object Files
OBJS= ${CPPFILES:.c=.o}

# The first target entry in this file to be invoked when typing "make". Convention is to use "all" or "default" here
all: $(TARGET)

# To create the executable, we need all object files
$(TARGET): ${OBJS}
	$(CC) -o $@ $^ ${LINKLIBS}

# To create the object files, we need all source files + header files
%.o: %.cpp %.h
	$(CC) -o $@ -cpp $< $(CFLAGS)

# Define that if a file exists in this directory called "clean" then it will still run the clean command defined below
.PHONY: clean

# Execute below when invoking "make clean"
clean:
	$(CLEAN)