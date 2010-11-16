#ifndef __CMD__LINE__H
#define __CMD__LINE__H

#ifndef CMD_ARGS_OPTION_FILE
#define CMD_ARGS_OPTION_FILE "cmd_args.opt"
#endif

#define CMD_DEFINE_ARG(a, b, c, d)
#include CMD_ARGS_OPTION_FILE
#undef CMD_DEFINE_ARG

#ifndef CMD_STRUCT_NAME
#define CMD_STRUCT_NAME CmdLineArgs
#endif

#define CMD_DEFINE_ARG(option, type, default, help_str) type option;

#define bool int
#define string char *

#ifdef CMD_TYPEDEF_STRUCT
typedef struct {
#else
struct CMD_STRUCT_NAME {
#endif
#include CMD_ARGS_OPTION_FILE
#ifdef CMD_TYPEDEF_STRUCT
} CMD_STRUCT_NAME;
#else
};
#endif

#undef CMD_DEFINE_ARG

#undef bool
#undef string

/*
 * This callback should return 0 to have CmdArgs continue parsing
 * and return 1 to stop.
 */

typedef int (*CommandLineParsingErrorFunc) (void *, char *, char *);

#ifdef CMD_TYPEDEF_STRUCT
int cmd_parse_args (CMD_STRUCT_NAME *, int, char **,
                    CommandLineParsingErrorFunc,
                    void *);
#else
int cmd_parse_args (struct CMD_STRUCT_NAME *, int, char **,
                    CommandLineParsingErrorFunc,
                    void *);
#endif

#undef CMD_STRUCT_NAME
#ifdef CMD_TYPEDEF_STRUCT
#undef CMD_TYPEDEF_STRUCT
#endif

void cmd_show_usage (void);

#endif /* __CMD__LINE__H */
