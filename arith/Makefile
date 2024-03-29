# Makefile for arith (Comp 40 Assignment 4)
# 
# Includes build rules for sudoku, unblackedges, my_useuarray2, and my_usebit2.
#
# This Makefile is more verbose than necessary.  In each assignment
# we will simplify the Makefile using more powerful syntax and implicit rules.
#
# New syntax rules added:  wildcards (%.o, %.c) and special variables:
# $@ - the target of the rule
# $< - the first dependency
# $^ - all dependencies
#
# Last updated: February 4, 2016


############## Variables ###############

CC = gcc # The compiler being used

# Updating include path to use Comp 40 .h files and CII interfaces
IFLAGS = -I. -I/comp/40/build/include -I/usr/sup/cii40/include/cii

# Compile flags
# Set debugging information, allow the c99 standard,
# max out warnings, and use the updated include path -Werror
CFLAGS = -g -std=c99 -Wall -Wextra -Werror -Wfatal-errors -pedantic $(IFLAGS)

# Linking flags
# Set debugging information and update linking path
# to include course binaries and CII implementations
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64

# Libraries needed for linking
# Both programs need cii40 (Hanson binaries) and *may* need -lm (math)
# Only brightness requires the binary for pnmrdr.
LDLIBS = -lcii40 -l40locality -larith40 -lm -lnetpbm -lrt -lpnm

# Collect all .h files in your directory.
# This way, you can never forget to add
# a local .h file in your dependencies.
INCLUDES = $(shell echo *.h)

 ############### Rules ###############

all: ppmdiff 40image bitpack 40image-6


## Compile step (.c files -> .o files)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c $(INCLUDES)
	$(CC) $(CFLAGS) -c $< -o $@


## Linking step (.o -> executable program)

bitpack: bitpacktesting.o bitpack.o quantization.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

ppmdiff: ppmdiff.o a2plain.o uarray2.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

40image: 40image.o compress40.o a2plain.o uarray2.o DCT.o CVRGB.o bitprep.o \
		  quantization.o bitpack.o reader.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

40image-6: 40image.o compress40.o a2plain.o uarray2.o DCT.o CVRGB.o bitprep.o \
		  quantization.o bitpack.o reader.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

clean:
	rm -f ppmdiff 40image 40image-6 bitpack *.o

cleanppm:
	rm -r *.ppm

