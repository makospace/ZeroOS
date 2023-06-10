#ifndef COMMAND_H
#define COMMAND_H
	char far *getParameter(char far * strCommand, int iCommandLen, char far * strArg);//获取命令行参数
	void commandVer();//显示版本信息
	void commandHelp(const char far *strCommand);//显示帮助信息
	void commandType(char far * strCommand);//显示文本文件内容
	void commandDir();
	void commandMv(char far * strCommand);
	void commandDel(char far * strCommand);
#endif //ifdef  COMMAND_H