#include "cmd_args.h"

#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Simplest possible test case. */

char *new_argv [] = {NULL,
                     "--enable-bool-one",
                     "--string-two=something",
                     "--double-one=4.4",
                     NULL};

static void
make_assertions (CommandLineArgs *args)
{
	assert (args->bool_one);
	assert (!args->bool_two);
	assert (!strcmp (args->string_one, "default-value"));
	assert (!strcmp (args->string_two, "something"));
	assert (args->double_one == 4.4); /* Risky. Find a better way. */
	assert (args->long_one == 0);
}

int
main (int argc, char **argv)
{
	CommandLineArgs args;
	if (argc == 1) {
		new_argv [0] = argv [0];
		execv (argv [0], new_argv);
	} else {
		cmd_parse_args (&args, argc, argv, NULL, NULL);
		make_assertions (&args);
		printf ("SUCCESS\n");
	}
	return 0;
}
