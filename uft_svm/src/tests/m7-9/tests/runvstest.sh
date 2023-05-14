#!/bin/bash

FILES=*.scm

for f in $FILES
do
    printf "running %s\n" $f
    OUT=$(( uft ho-vs $f | uft vs-vs > "/dev/null") 2>&1)
    if [ -z "$OUT" ]
    then
        echo "no errors in vs-vs translation"
        echo "Correctness:"
        uft ho-vs $f | uft vs-vo | svm predef.vo -
    else
        echo $OUT
    fi
    printf "\n"
done