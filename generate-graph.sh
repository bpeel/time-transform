#!/bin/bash

ROOT_DIR=`dirname $0`
ROOT_DIR=`cd "$ROOT_DIR" && pwd`
PROG="$ROOT_DIR/time-transform"

echo -n "n_verts"

"$PROG" 1 | \
    sed -rn \
    -e 's/^([a-z0-9_]+) time .*/\1/' \
    -e '$ b done' \
    -e 'T' \
    -e 'H' \
    -e 'b' \
    -e ': done' \
    -e 'x' \
    -e 's/\n/,/gp'

step=0

for ((i = 1; i < 256; i += step)); do
    echo -n $i
    "$PROG" $i | \
        sed -rn \
        -e 's/^.* time = ([0-9\.]+).*/\1/' \
        -e '$ b done' \
        -e 'T' \
        -e 'H' \
        -e 'b' \
        -e ': done' \
        -e 'x' \
        -e 's/\n/,/gp';
    step=`expr $step + 1`
done;
