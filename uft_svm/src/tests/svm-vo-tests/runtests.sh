#!/bin/bash

vofiles=($(ls *.vo))
# printf "%s\n" "${vofiles[@]}"
for i in "${vofiles[@]}"
do
    printf "$i \n"
    svm $i
    printf "\n"
done