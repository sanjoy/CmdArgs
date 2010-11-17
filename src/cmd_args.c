#include "cmd_args.h"

#include <assert.h>

#ifndef CMD_ARGS_OPTION_FILE
#define CMD_ARGS_OPTION_FILE "cmd_args.opt"
#endif

#define CMD_DEFINE_ARG(a, b, c, d)
#include CMD_ARGS_OPTION_FILE
#undef CMD_DEFINE_ARG

#ifndef CMD_STRUCT_NAME
#define CMD_STRUCT_NAME CmdLineArgs
#endif

#ifdef CMD_TYPEDEF_STRUCT
#define STRUCT_TYPE CMD_STRUCT_NAME
#else
#define STRUCT_TYPE struct CMD_STRUCT_NAME
#endif

#ifndef CMD_PRETTY_PRINT_COLUMN_WIDTH
#define CMD_PRETTY_PRINT_COLUMN_WIDTH 80
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef char* string;
typedef int bool;

typedef enum {
	TYPE_long,
	TYPE_double,
	TYPE_string,
	TYPE_bool
} CmdOptionType;

static int parse_long (char *, long *) __attribute__ ((unused));
static int parse_double (char *, double *) __attribute__ ((unused));
static int parse_string (char *, char **) __attribute__ ((unused));
static int parse_bool (char *, int *) __attribute__ ((unused));

static int
parse_bool (char *s, int *b)
{
	assert (0);
	return 0;
}

static int
parse_long (char *s, long *l)
{
	return sscanf (s, "%ld", l) == 1;
}

static int
parse_double (char *s, double *d)
{
	return sscanf (s, "%lf", d) == 1;
}

static int
parse_string (char *s, char **dest)
{
	*dest = strdup (s);
	return 1;
}

#define GET_FUNC(type) parse_##type

#define find_value(argc, argv, result, option_name, type_id,		\
                   type, error_flag)								\
	do {															\
		int i;														\
		char *disable, *enable, *to_find;							\
		if (type_id == TYPE_bool) {									\
			disable = malloc (strlen ("--disable") +				\
			                  strlen (option_name) + 1);			\
			enable = malloc (strlen ("--enable") +					\
			                 strlen (option_name) + 1);				\
			strcpy (disable, "--disable-");							\
			strcat (disable, option_name);							\
			strcpy (enable, "--enable-");							\
			strcat (enable, option_name);							\
		} else {													\
			to_find = malloc (strlen (option_name) + 3);			\
			strcpy (to_find, "--");									\
			strcat (to_find, option_name);							\
		}															\
																	\
		for (i = 0; i < argc; i++) {								\
			if (type_id == TYPE_bool) {								\
				if (strcmp (argv [i], disable) == 0) {				\
					(result) = (type) 0;							\
				} else if (strcmp (argv [i], enable) == 0) {		\
					(result) = (type) 1;							\
				}													\
			} else {												\
				if (strstr (argv [i], to_find) == argv [i]) {		\
					char *index = strstr (argv [i], "=");			\
					if (index != NULL) {							\
						type value;									\
						index++;									\
						if (GET_FUNC (type) (index, &value))		\
							(result) = value;						\
						else {										\
							error_flag = 1;							\
							error_option = strdup (to_find + 2);	\
							error_value = index;					\
						}											\
					}												\
				}													\
			}														\
		}															\
																	\
		if (type_id == TYPE_bool) {									\
			free (disable);											\
			free (enable);											\
		} else {													\
			free (to_find);											\
		}															\
	} while (0)

static char *replace_underscore_with_dash (char *str)
{
	char *x = malloc (strlen (str) + 1);
	char *i = x;

	while (*str) {
		if (*str == '_') {
			*i++ = '-';
			str++;
		} else {
			*i++ = *str++;
		}
	}
	*i = '\0';
	return x;
}

int
cmd_parse_args (STRUCT_TYPE *cmd, int argc, char **argv,
                CommandLineParsingErrorFunc func, void *error_data)
{
	char *option, *error_option, *error_value;
	int error_flag = 0;

#ifdef CMD_UNDERSCORE_TO_DASH
	int us_to_dash = 1;
#else
	int us_to_dash = 0;
#endif

	/* Remove the filename */
	argc--;
	argv++;

#define true 1
#define false 0
#define CMD_DEFINE_ARG(option, type, def_value, help_str) (cmd->option = def_value);
#include CMD_ARGS_OPTION_FILE
#undef CMD_DEFINE_ARG
#undef true
#undef false
	
#define CMD_DEFINE_ARG(option_name, type, def_value, xxx) do {	\
		error_flag = 0;											\
		option = #option_name;									\
		if (us_to_dash)											\
			option = replace_underscore_with_dash (option);		\
		find_value (argc, argv, cmd->option_name, option,		\
		            TYPE_##type, type, error_flag);				\
		if (error_flag)	{										\
			if (func == NULL ||									\
			    func (error_data, error_option, error_value))	\
				return 0;										\
			free (error_option);								\
		}														\
		if (us_to_dash)											\
			free (option);										\
	} while (0);

#include CMD_ARGS_OPTION_FILE

#undef CMD_DEFINE_ARG
	return 1;
}

static char *
type_to_string (CmdOptionType type)
{
	switch (type) {
	case TYPE_bool:
		return "boolean";
	case TYPE_string:
		return "string";
	case TYPE_double:
		return "floating point";
	case TYPE_long:
		return "integer";
	}
	assert (0);
	return NULL;
}

#define CMD_PRETTY_PRINT_COLUMN_WIDTH_DELTA 20

#ifdef CMD_PRETTY_PRINT
static void
pretty_print_usage_string (char *str, int margin)
{
	int prn_len = CMD_PRETTY_PRINT_COLUMN_WIDTH - margin;

	if (prn_len < CMD_PRETTY_PRINT_COLUMN_WIDTH_DELTA) {
		printf ("\n");
		printf ("%s\n", str);
	} else {
		int i, len = strlen (str);
		for (i = 0; i < len; i++) {
			printf ("%c", str [i]);
			if (i != 0 && (i % prn_len) == 0) {
				int j = 0;
				printf ("\n");

				for (j = 0; j < (margin - 1); j++) {
					printf (" ");
				}
			}
		}
		if (((i - 1) % prn_len) != 0)
			printf ("\n");
	}
}
#else
static void
pretty_print_usage_string (char *str, int margin)
{
	printf ("%s\n", str);
}
#endif

#define print_usage_for(option, type, usage, def_value, us_to_dash)		\
	do {																\
		char *option_name;												\
		int margin = 0;													\
		if (us_to_dash)													\
			option_name = replace_underscore_with_dash (option);		\
		else															\
			option_name = option_name;									\
		if (type == TYPE_bool) {										\
			margin = printf ("--disable-%s, --enable-%s"				\
			                 " [default=%s]: ", option_name,			\
			                 option_name, def_value);					\
		} else {														\
			margin = printf ("--%s=<%s value> [default=%s]: ",			\
			                 option_name, type_to_string (type),		\
			                 def_value);								\
		}																\
		pretty_print_usage_string (usage, margin);						\
		if (us_to_dash)													\
			free (option_name);											\
	} while (0)

void
cmd_show_usage (void)
{
#ifdef CMD_UNDERSCORE_TO_DASH
	int us_to_d = 1;
#else
	int us_to_d = 0;
#endif

#define CMD_DEFINE_ARG(option_name, type, def_value, usage)	\
	print_usage_for (#option_name, TYPE_##type, usage,		\
	#def_value, us_to_d);

#include CMD_ARGS_OPTION_FILE
}
