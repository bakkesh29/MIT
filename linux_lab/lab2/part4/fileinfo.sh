
#!/bin/bash

for file in "$@"
do
    if [ -e "$file" ]
    then
        ls -lu -lh "$file"
    else
        echo "$file does not exist"
    fi
done
