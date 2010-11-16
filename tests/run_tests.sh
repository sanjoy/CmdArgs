#!/bin/sh

CC='cc'
CFLAGS='-g -O0'
TEST_BINARIES="test_binaries"

if [ ! -d $TEST_BINARIES ]; then
	mkdir $TEST_BINARIES
fi

for file in *.c; do
	file_root="`basename $file .c`"
	echo "Running $file_root"
	$CC $CFLAGS $file -I. -I../src ../src/cmd_args.c -DCMD_ARGS_OPTION_FILE="\"$file_root.opt\"" -o "$TEST_BINARIES/$file_root"
	$TEST_BINARIES/$file_root
done
