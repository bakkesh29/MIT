

#!/bin/bash

for file in "$@"
do
    if [ -f "$file" ]
    then
        newname=$(echo "$file" | tr '[:lower:]' '[:upper:]')

        if [ -e "$newname" ]
        then
            echo "$newname already exists."
        else
            mv "$file" "$newname"
            echo "$file renamed to $newname"
        fi
    else
        echo "$file does not exist."
    fi
done
