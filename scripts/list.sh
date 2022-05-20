#! /bin/bash

if [[ $# -lt 1 ]] ; then
    >&2 echo 'List C++ files in the given directories.'
    >&2 echo "Usage: $0 <dirs...>"
    exit 1
fi

find "$@" -type f -name '*.c' -o -name '*.cpp' -o -name '*.h' -o -name '*.hpp' | sort

