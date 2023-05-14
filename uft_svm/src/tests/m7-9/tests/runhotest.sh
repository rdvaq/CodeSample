#!/bin/bash
#valgrind will complain if errors appear

TESTFILES=*.scm

for file in $TESTFILES
do
    printf "running %s\n" $file
    printf "ho-vo > svm:\n"
    vscheme -predef | uft ho-vo - $file | valgrind --leak-check=full -q svm 
    printf "\n"
done