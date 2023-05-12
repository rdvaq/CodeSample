#! /bin/bash
# input="./test"
input="./https_urls"

while read -r line
do  
    printf "$line\n"
    # printf "simple curl\n"
    # /usr/bin/time -f "%e" curl -s "$line" > /dev/null 
    # printf "through proxy\n"
    # /usr/bin/time -f "%e" curl -s -k -x localhost:9086 "$line" > /dev/null
    curl -s -k -x localhost:9086 "$line" > /dev/null
    # printf "\n"
    
done < "$input"
