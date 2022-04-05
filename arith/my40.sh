#! /bin/bash

#only test program on file inputs, no cin test

function decomp() {

FILE1="shellComp.ppm"
FILE2="shellDecomp.ppm"
if [ ! -f "$FILE1" ]
then
    touch $FILE1
fi

if [ ! -f "$FILE2" ]
then
    touch $FILE2
fi

echo "Running program on $1"
./40image -c "$1" > $FILE1
./40image -d  $FILE1 > $FILE2
#display $FILE2 

echo "The PPMdiff value of the original vs decompressed image is: "
./ppmdiff $1 $FILE2 
echo 

}

#if u want to use this script then make a folder that contains all your test
#images and switch the path below to the the path of your folder
search_dir=/h/czhou06/comp40/arith/ppm2

for entry in "$search_dir"/* 
do
  if [ -f "$entry" ];then
    decomp "$entry"
  fi
done
