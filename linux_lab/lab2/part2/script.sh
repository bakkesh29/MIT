#!/bin/bash

echo "Today's date: $(date)"
echo "This month's calendar:"
cal $(date "+%m %Y")
echo "My shell: $SHELL"

