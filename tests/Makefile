CC=cc
CFLAGS=-g -O0 -Wall -Werror

SOURCES=test_1.c test_2.c test_3.c
INCLUDE=-I. -I../src
BINDIR=test_binaries
TARGETS=$(SOURCES:.c=)
QUOTE=\"

.SUFFIXES: .c

all: prep utils.o $(TARGETS) final

prep:
	test -d $(BINDIR) || mkdir -p $(BINDIR)

utils.o : utils.c
	$(CC) $(CFLAGS) -c utils.c

.c: $< ../src/cmd_args.c
	$(CC) -DCMD_ARGS_OPTION_FILE="$(QUOTE)$*.opt$(QUOTE)" $(CFLAGS) \
	      $(INCLUDE) utils.o ../src/cmd_args.c $< -o $(BINDIR)/$*

final:
	./run_tests.sh FROM_MAKEFILE

clean:
	rm -rf utils.o $(BINDIR)
