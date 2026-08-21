
#!/bin/bash

echo "Enter the directory name:"
read directory

if [ -d "$directory" ]
then
    echo "The .c files in $directory are:"
    find "$directory" -maxdepth 1 -type f -name "*.c"
else
    echo "The given directory does not exist."
fi
