#! /bin/bash

if [[ $# -lt 1 ]] ; then
    >&2 echo 'Format the C++ source files in the given directories.'
    >&2 echo "Usage: $0 <dirs...>"
    exit 1
fi

for i in $("$(dirname $0)"/list.sh "$@") ; do
    >&2 echo clang-format -i "$i"
    clang-format -i "$i"
done
