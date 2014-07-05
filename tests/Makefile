CC=cc
CFLAGS=-g -O0 -Wall -Werror
INCLUDE=-I. -I../src

BINDIR=build
OBJECTS=$(addprefix $(BINDIR)/, utils.o)
TARGETS=$(addprefix $(BINDIR)/, test_1 test_2 test_3)

all: $(TARGETS) test

.SECONDEXPANSION:
$(TARGETS): $$(@F).c utils.h ../src/cmd_args.h $(OBJECTS) | $(BINDIR)
	$(CC) -DCMD_ARGS_OPTION_FILE="\"$(@F).opt\"" $(CFLAGS) \
	      $(INCLUDE) ../src/cmd_args.c $^ -o $@

$(BINDIR)/%.o: %.c | $(BINDIR)
	$(CC) $(CFLAGS) $(INCLUDE) -o $@ -c $^

$(BINDIR):
	mkdir $(BINDIR)

.PHONY: test clean

test: $(TARGETS)
	@for i in $^; do ./$$i; done

clean:
	rm -rf $(BINDIR)
