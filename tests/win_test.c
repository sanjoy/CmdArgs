#ifdef CMD_ARGS_OPTION_FILE
#define CMD_STRUCT_NAME CmdLineArgs
#define CMD_TYPEDEF_STRUCT
#define CMD_UNDERSCORE_TO_DASH

CMD_DEFINE_ARG(str_arg_null, string, NULL, "Should show NULL for its default value.")
CMD_DEFINE_ARG(str_arg_value, string, "a string", "Should show \"a string\" for its default value.")
CMD_DEFINE_ARG(boolean_arg_false, bool, false, "Should show up as disabled by default.")
CMD_DEFINE_ARG(boolean_arg_true, bool, true, "Should show up as enabled by default.")
CMD_DEFINE_ARG(toggle_arg, bool, false, "Should show up as disabled by default.")
CMD_DEFINE_ARG(double_arg, double, 5.0, "Should show up as 5.0 by default.")
CMD_DEFINE_ARG(long_arg, long, 200, "Should show up as 200 by default.")
#else
#define CMD_ARGS_OPTION_FILE "win_test.c"
#include "cmd_args.c"

int main(int argc, char* argv[])
{
	CmdLineArgs args = {0};
	if(argc == 0) {
		cmd_show_usage();
		return 0;
	} else {
		cmd_parse_args (&args, argc, argv, NULL, NULL);
		printf("Arguments:\n");
#		define PRINTOPT3(OPT,FMT,VAL) \
			printf("  " XSTRR(OPT) " = " FMT "\n", VAL)
#		define PRINTOPT2(OPT,FMT) \
			PRINTOPT3(OPT, FMT, args.OPT)
#		define PRINTBOOL(OPT) \
			PRINTOPT3(OPT,"%s", args.OPT ? "true" : "false")
		PRINTOPT2(str_arg_null, "%s");
		PRINTOPT2(str_arg_value, "%s");
		PRINTBOOL(boolean_arg_false);
		PRINTBOOL(boolean_arg_true);
		PRINTBOOL(toggle_arg);
		PRINTOPT2(double_arg, "%lf");
		PRINTOPT2(long_arg, "%ld");
	}
	return 0;
}

#endif /* CMD_ARGS_OPTION_FILE */
