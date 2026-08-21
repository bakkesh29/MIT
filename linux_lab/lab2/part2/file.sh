#!/bin/bash

echo "Program: $0"
echo "Number of arguments : $#"
echo "Arguments are : $*"

# Check if exactly 2 arguments were provided
if [ $# -ne 2 ]; then
    echo "Error: Please provide exactly 2 arguments (pattern and file)."
    echo "Usage: $0 <pattern> <filename>"
    exit 1
fi

# Search for the pattern in the file
grep "$1" "$2"
echo "job over"
