#include "cmd_args.h"

#include <stdio.h>

int
main (int argc, char **argv)
{
	printf ("Sample usage: \n\n");
	cmd_show_usage ();
	return 0;
}
