#!/bin/bash

ERRFILES=*.error


for it in $ERRFILES
do
    printf "$it:\n"
    ERROUT=$(uft ho-vo $it | env SVMDEBUG=stacktrace svm predef.vo - 2>&1)
    echo "$ERROUT"
    printf "/////////////////////////////////////////////////\n\n"
done
