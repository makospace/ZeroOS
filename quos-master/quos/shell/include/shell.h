#ifndef SHELL_H
#define SHELL_H
	#define COMMAND_MAX_LEN 512
	#define COMMAND_NAME_MAX_LEN 128
	#define COMMAND_PRE "QUOS->"

	char far * getCommand(char far * strCommand);
	int getCommandName(const char far * strCommand, char far * strName);
	void runCommand(const char far * strCommand);
	void Shell_main(void);
#endif //ifdef  TYPEDEF_H