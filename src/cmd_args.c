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

										// E stands for "expand", R stands for "raw"
#define XSTRR(X) #X						// Wrap a token in quotes - don't expand macro params.
#define XSTRE(X) XSTRR(X)				// Wrap a token in quotes - expand macro params.
#define XJNR(X,Y) X##Y					// Join two tokens - don't expand macro params.
#define XJNE(X,Y) XJNR(X,Y)				// Join two tokens - expand macro params
#define XTYPID(TYP) XJNR(TYPE_,TYP)		// Generate an identifier for one of the CmdOptionType values.
#define XTYPS(TYP) XJNE(TYPESTR_,TYP)	// Generate an identifier for one of the TYPESTR_<type> macros.

#define TYPESTR_bool "boolean"
#define TYPESTR_string "string"
#define TYPESTR_double "floating point"
#define TYPESTR_long "integer"
#define TYPESTR_toggle "toggle"

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

#ifdef _MSC_VER
#define strdup _strdup
#define snprintf _snprintf
#else
static int parse_long (char *, long *) __attribute__ ((unused));
static int parse_double (char *, double *) __attribute__ ((unused));
static int parse_string (char *, char **) __attribute__ ((unused));
static int parse_bool (char *, int *) __attribute__ ((unused));
static int parse_toggle (char *, int *) __attribute__ ((unused));
#endif

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

#define GET_FUNC(type) XJNR(parse_,type)

// For boolean options, we created a fixed char arrays to store the
// --enable-option, --disable-option strings. All other options will
// be formatted as --option, so we know that the --enable-option
// buffer will be large enough to hold them.
// 
// For index ->
//   sizeof("optionA") = 8
//   strlen("--optionA=") = 10
//   arg[sizeof(option_name)] + 1 == '='
//   index = &arg[sizeof(option_name) + 2]
#define FIND_VALUE(argc, argv, result, option_name, type_id,		\
                   type, error_flag)								\
	do {															\
		int i;														\
		char *to_find,												\
			 disable[sizeof(option_name)+10] = "--disable-",		\
			 enable [sizeof(option_name)+ 9] = "--",				\
			 option[sizeof(option_name)] = option_name;				\
																	\
		to_find = enable;											\
		if(us_to_dash) replace_underscore_with_dash(option);		\
		if(type_id == TYPE_bool) {									\
			strcat(disable, option);								\
			strcat(enable, "enable-");								\
			strcat(enable,  option);								\
		} else {													\
			strcat(to_find, option);								\
		}															\
																	\
		for (i = 0; i < argc; i++) {								\
			size_t arglen = strlen(argv [i]);						\
			if(arglen < (sizeof(option_name) + 1)) continue;		\
			if (type_id == TYPE_bool) {								\
				if (strcmp (argv [i], disable) == 0) {				\
					(result) = (type) 0;							\
				} else if (strcmp (argv [i], enable) == 0) {		\
					(result) = (type) 1;							\
				}													\
			} else if (type_id == TYPE_toggle) {					\
				if (!strcmp (argv [i], to_find))					\
					(result) = (type) 1;							\
			} else if(arglen > (sizeof(option_name)+2)) {			\
				if ((strncmp(										\
				  argv [i], to_find, sizeof(option_name) + 1 		\
				) == 0) && (										\
				  (argv [i][sizeof(option_name) + 1]) == '='		\
				)) {												\
					type value;										\
					char* index = 									\
					  &((argv [i])[sizeof(option_name)+2]);			\
					if (*index && GET_FUNC (type) (index, &value))	\
						(result) = value;							\
					else {											\
						error_flag = 1;								\
						error_option = strdup (to_find + 2);		\
						error_value = strdup (index);				\
					}												\
				}													\
			}														\
		}															\
	} while (0)

static void replace_underscore_with_dash (char *str)
{
	char* p = str;
	while(*p) {
		if(*p == '_') *p = '-';
		p++;
	}
}

#define DEFAULT_VALUE_VALIDATION(type_id, default_value, type) do {	\
		if (type_id == TYPE_toggle)									\
			assert (default_value == (type) 0);						\
	} while (0)

int
cmd_parse_args (STRUCT_TYPE *cmd, int argc, char **argv,
                CommandLineParsingErrorFunc func, void *error_data)
{
	char *error_option = NULL, *error_value = NULL;
	int error_flag = 0,
#ifdef CMD_UNDERSCORE_TO_DASH
	    us_to_dash = 1;
#else
	    us_to_dash = 0;
#endif

	/* Remove the filename */
	argc--;
	argv++;

#define true 1
#define false 0

// Note: The following doesn't appear to work as intended for strings
//       when compiling with MSVC
#define CMD_DEFINE_ARG(option, type, def_value, help_str) (cmd->option = def_value);
#include CMD_ARGS_OPTION_FILE
#undef CMD_DEFINE_ARG

#define CMD_DEFINE_ARG(option_name, type, def_value, xxx) do {	\
		error_flag = 0;											\
		DEFAULT_VALUE_VALIDATION (XTYPID(type), def_value,		\
		                          type);						\
		FIND_VALUE (argc, argv, cmd->option_name, 				\
		            XSTRE(option_name), XTYPID(type),			\
		            type, error_flag);							\
		if (error_flag)	{										\
			if (func == NULL ||									\
			    func (error_data, error_option, error_value))	\
				return 0;										\
			if(error_option) free((void*)error_option);			\
			if(error_option) free((void*)error_value);			\
		}														\
	} while (0);

#include CMD_ARGS_OPTION_FILE

#undef CMD_DEFINE_ARG
#undef true
#undef false
	return 1;
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

#undef NULL
#define true "\x1\x1x1\x1\x1\x1\x1"		// strlen(enabled)
#define false "\0\0\0\0\0\0\0\0"		// strlen(disabled)
#define NULL "\0\0\0\0"

// Leave off the spaces on "[default=]:" to compensate for it adding a 
// terminating zero to the option and the value.
#define XTYPVAL(TYP) "=<" XTYPS(TYP) " value>"							// Generate the "=<typename value> " string.
#define XSZM(opt,inc,mult) (((sizeof(opt) - 1) * mult) + inc + 1)		// Length of (opt * mult) + inc with trailing 0
#define XSZ(opt,inc) XSZM(opt,inc,1)									// Length of opt + inc with trailing 0
#define XDEFLEN(dv) (sizeof(dv) + sizeof("[default=]:"))				// Length for default values string
#define XVALLEN(dv) XDEFLEN(XSTRE(dv))									// Length for numerical default values.
#define XTYPLEN(typ) (sizeof(XTYPVAL(typ)) - 1)							// Length of the "=<typename value> " string.

// Length of buffer for options that aren't booleans or toggles.
#define XTYPSTRLEN(opt,type,dv) 										\
		( XSZ(opt,2) + XTYPLEN(type) + XVALLEN(dv) )					

// Add the "[default=DEFAULT_VALUE]: " string, then print the buffer.
#define PRINT_DEFAULT_VAL(buf,dv,use)									\
		strcat(buf, " [default=" dv "]: ");								\
		pretty_print_usage_string(buf, use,								\
				CMD_PRETTY_PRINT_OPTION_COLUMN_WIDTH_MAX)

#define PRINT_USAGE_FOR(option, type, usage, def_value,					\
                        us_to_dash)										\
	do {																\
		type def_val_check_t = (type)(def_value);						\
		char option_name[sizeof(option)] = option,						\
		     *defval_check_s = *((char**)&def_val_check_t);				\
		if (us_to_dash)													\
			replace_underscore_with_dash (option_name);					\
		if (XTYPID(type) == TYPE_bool) {								\
			char buffer[XSZM(option,21,2) + XDEFLEN(def_value)] = "";	\
			sprintf(buffer, "--disable-%s, --enable-%s", option_name,	\
			                                     option_name);			\
			if(*defval_check_s) {										\
				PRINT_DEFAULT_VAL(buffer, "enabled", usage);			\
			} else {													\
				PRINT_DEFAULT_VAL(buffer, "disabled", usage);			\
			}															\
		} else if (XTYPID(type) == TYPE_toggle) {						\
			char buffer[XSZ(option,2)+XDEFLEN(def_value)] = "--";		\
			strcat(buffer, option_name);								\
			if(*defval_check_s) {										\
				PRINT_DEFAULT_VAL(buffer, "enabled", usage);			\
			} else {													\
				PRINT_DEFAULT_VAL(buffer, "disabled", usage);			\
			}															\
		} else {														\
			char buffer[XTYPSTRLEN(option, type, def_value)] = "";		\
			sprintf(buffer, "--%s" XTYPVAL(type), option_name);			\
			if (XTYPID(type) == TYPE_string) {							\
				if (!(*defval_check_s)) {								\
					PRINT_DEFAULT_VAL(buffer, "NULL", usage);			\
				} else {												\
					PRINT_DEFAULT_VAL(buffer, XSTRE(def_value), usage);	\
				}														\
			} else {													\
				PRINT_DEFAULT_VAL(buffer, XSTRE(def_value), usage);		\
			}															\
		}																\
	} while (0)

void
cmd_show_usage (void)
{
#ifdef CMD_UNDERSCORE_TO_DASH
	int us_to_d = 1;
#else
	int us_to_d = 0;
#endif

#define CMD_DEFINE_ARG(option_name, type, def_value, usage)				\
		PRINT_USAGE_FOR(XSTRE(option_name), type, usage, def_value,		\
		                us_to_d);
#include CMD_ARGS_OPTION_FILE
#undef CMD_DEFINE_ARG

}

#undef true
#undef false
#undef NULL
#define NULL ((void*)0)
