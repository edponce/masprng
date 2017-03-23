#!/bin/bash

DIR=$1

if [ -z $DIR ]; then
    echo "ERROR! need to provide a directory to search recursively for C++ files."
    exit 0
fi

# List files recursively, ignore hidden files
# Filter source files and count
NUMFILES=`ls -R "$DIR" | grep -E '(\.cpp|\.h)$' | wc -l`

echo "Number of C++ files: $NUMFILES"

