#!/bin/sh

# If this is invoked directly, run make first
# to make sure that the binaries are up-to-date

if [ "$#" -eq "0" ]; then
	make
fi

BINDIR=test_binaries
for file in test_*.c; do
	file_root="`basename $file .c`"
	echo "Running $file_root"
    ./$BINDIR/$file_root
done
