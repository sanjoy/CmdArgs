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

#ifndef CMD_PRETTY_PRINT_OPTION_COLUMN_WIDTH_MAX
#define CMD_PRETTY_PRINT_OPTION_COLUMN_WIDTH_MAX 5
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef char* string;
typedef int bool;
typedef int toggle;

typedef enum {
	TYPE_long,
	TYPE_double,
	TYPE_string,
	TYPE_bool,
	TYPE_toggle
} CmdOptionType;

static int parse_long (char *, long *) __attribute__ ((unused));
static int parse_double (char *, double *) __attribute__ ((unused));
static int parse_string (char *, char **) __attribute__ ((unused));
static int parse_bool (char *, int *) __attribute__ ((unused));
static int parse_toggle (char *, int *) __attribute__ ((unused));

static int
parse_toggle (char *s, int *b)
{
	assert (0);
	return 0;
}

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

#define FIND_VALUE(argc, argv, result, option_name, type_id,		\
                   type, error_flag)								\
	do {															\
		int i;														\
		char *disable, *enable, *to_find;							\
		if (type_id == TYPE_bool) {									\
			disable = malloc (strlen ("--disable-") +				\
			                  strlen (option_name) + 1);			\
			enable = malloc (strlen ("--enable-") +					\
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
			} else if (type_id == TYPE_toggle) {					\
				if (!strcmp (argv [i], to_find))					\
					(result) = (type) 1;							\
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

#define DEFAULT_VALUE_VALIDATION(type_id, default_value, type) do {	\
		if (type_id == TYPE_toggle)									\
			assert (default_value == (type) 0);						\
	} while (0)

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
	
#define CMD_DEFINE_ARG(option_name, type, def_value, xxx) do {	\
		error_flag = 0;											\
		option = #option_name;									\
		DEFAULT_VALUE_VALIDATION (TYPE_##type, def_value,		\
		                          type);						\
		if (us_to_dash)											\
			option = replace_underscore_with_dash (option);		\
		FIND_VALUE (argc, argv, cmd->option_name, option,		\
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
#undef true
#undef false
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
	case TYPE_toggle:
		return "toggle";
	}
	assert (0);
	return NULL;
}

#define PRINT_N_CHARACTERS(txt, len) do {			\
		char *text = (txt);							\
		int l = (len);								\
		int i;										\
		for (i = 0; i < l && text [i] != '\0'; i++)	\
			putchar (text [i]);						\
	} while (0)

#define PRINT_N_SPACES(n) do {					\
		int i, len = (n);						\
		for (i = 0; i < len; i++)				\
			putchar (' ');						\
	} while (0)

static void
pretty_print_usage_string (char *option, char *help, int option_col_width)
{
	int help_len = strlen (help);

	PRINT_N_SPACES (2);

	printf ("%s\n", option);

	while (help_len > 0) {
		PRINT_N_SPACES (2 + option_col_width);
		PRINT_N_CHARACTERS (help, CMD_PRETTY_PRINT_COLUMN_WIDTH - option_col_width);
		help += (CMD_PRETTY_PRINT_COLUMN_WIDTH - option_col_width);
		help_len -= (CMD_PRETTY_PRINT_COLUMN_WIDTH - option_col_width);
		putchar ('\n');
	}

	putchar ('\n');
}

#define DYNAMIC_SPRINTF(buffer, buffer_len, format, ...) do {			\
		int ret = snprintf (buffer, buffer_len, format, __VA_ARGS__);	\
		if (ret >= buffer_len) {										\
			buffer = realloc (buffer, ret + 1);							\
			assert (buffer);											\
			buffer_len = ret + 1;										\
			snprintf (buffer, buffer_len, format, __VA_ARGS__);			\
		}																\
	} while (0)

#define PRINT_USAGE_FOR(option, type, usage, def_value, us_to_dash,		\
                        buffer, buffer_len)								\
	do {																\
		char *option_name = NULL;										\
		if (us_to_dash)													\
			option_name = replace_underscore_with_dash (option);		\
		else															\
			option_name = option_name;									\
		if (type == TYPE_bool) {										\
			DYNAMIC_SPRINTF (buffer, buffer_len, "--disable-%s, "		\
			                 "--enable-%s [default=%s]: ", option_name, \
			                 option_name, def_value);					\
		} else if (type == TYPE_toggle) {								\
			DYNAMIC_SPRINTF (buffer, buffer_len, "--%s [default=off]",	\
			                 option_name);								\
		} else {														\
			DYNAMIC_SPRINTF (buffer, buffer_len, "--%s=<%s value> "		\
			                 "[default=%s]: ",							\
			                 option_name, type_to_string (type),		\
			                 def_value);								\
		}																\
		pretty_print_usage_string (buffer, usage,						\
		                    CMD_PRETTY_PRINT_OPTION_COLUMN_WIDTH_MAX);	\
		if (us_to_dash)													\
			free (option_name);											\
	} while (0)

static const char *
transform_default_bool_values (const char *def, CmdOptionType type)
{
	if (type == TYPE_bool) {
		if (!strcmp (def, "true")) {
			return "enabled";
		} else {
			return "disabled";
		}
	} else {
		return def;
	}
}

void
cmd_show_usage (void)
{
#ifdef CMD_UNDERSCORE_TO_DASH
	int us_to_d = 1;
#else
	int us_to_d = 0;
#endif

	char *txt = malloc (16);
	int len = 16;

#define CMD_DEFINE_ARG(option_name, type, def_value, usage)	\
	do {													\
		const char *def_v =									\
			transform_default_bool_values (#def_value,		\
			                               TYPE_##type);	\
		PRINT_USAGE_FOR (#option_name, TYPE_##type, usage,	\
		                 def_v, us_to_d, txt, len);			\
	} while (0);

#include CMD_ARGS_OPTION_FILE

#undef CMD_DEFINE_ARG

	free (txt);
}
