#!/bin/sh

echo "Enter the pattern to be searched : "
read pname 

echo "Enter the file to be used : "
read fname

echo "Searching for $pname from file $fname"

# Added a space between the pattern and the filename
grep "$pname" "$fname"

echo "Selected records shown above"

