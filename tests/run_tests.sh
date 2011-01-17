#!/bin/sh
BINDIR=test_binaries
for file in test_*.c; do
	file_root="`basename $file .c`"
	echo "Running $file_root"
    ./$BINDIR/$file_root
done
