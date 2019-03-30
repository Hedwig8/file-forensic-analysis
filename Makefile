#
# This is an example Makefile for a countwords program.  This
# program uses both the scanner module and a counter module.
# Typing 'make' or 'make count' will create the executable file.
#

# define some Makefile variables for the compiler and compiler flags
# to use Makefile variables later in the Makefile: $()
#
#
#
#  JUSR RUN 'make' IN TERMINAL/CONSOLE AND THIS WILL BE COMPILED
#
#
#
#
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#
CC = gcc
CFLAGS  = -g -Wall -Wextra -Werror

# typing 'make' will invoke the first target entry in the file 
# (in this case the default target entry)
# you can name this target entry anything, but "default" or "all"
# are the most commonly used names by convention
#
default: forensic

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
forensic:  forensic.o argumentHandler.o
	$(CC) $(CFLAGS) -o forensic forensic.c argumentHandler.c

# To create the object file countwords.o, we need the source
# files countwords.c, scanner.h, and counter.h:
#
forensic.o:  forensic.c argumentHandler.h
	$(CC) $(CFLAGS) -c forensic.c

# To create the object file counter.o, we need the source files
# counter.c and counter.h:
#
argumentHandler.o:  argumentHandler.c argumentHandler.h 
	$(CC) $(CFLAGS) -c argumentHandler.c

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) forensic *.o *~