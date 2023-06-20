//文件名：shell.c
//功能：
//	shell
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- shell.obj shell.c
//  如斯
//2007-7-8
#include "include/stdc.h"
#include "include/shell.h"
#include "include/command.h"
char far * getCommand(char far * strCommand)
{
	char chTmp = 0;
	int iCommandLen = 0;// 光标位置
	int n = 0;
	puts(COMMAND_PRE);// 显示命令提示符
	while(1)
	{
		chTmp = getch();
		if(chTmp == '\n' || chTmp == '\r')
		{
			puts("\n");
			break;
		}
		else if(chTmp == '\b')
		{
			if(iCommandLen <= 0)
			{
				continue;
			}
			else
			{
				putchar(chTmp);
				putchar(' ');
				putchar(chTmp);
				strCommand[iCommandLen] = 0;
				iCommandLen--;
			}
		}
		else
		{
			if(iCommandLen >= COMMAND_MAX_LEN)
			{
				continue;
			}
			else
			{
				putchar(chTmp);
				strCommand[iCommandLen] = chTmp;
				iCommandLen++;
			}
		}
	}
}
int getCommandName(const char far * strCommand, char far * strName)
{
	int i;
	char far * pDesc = strName;
	for(i=0; i<strLen(strCommand); i++)
	{
		if((*(strCommand + i)!=' ') && (*(strCommand + i)!='\0') && (*(strCommand + i)!='\t') && (*(strCommand + i)!='\r') && (*(strCommand + i)!='\n'))
		{
			*pDesc = *(strCommand + i);
			pDesc++;
		}
		else
		{
			break;
		}
	}
	*pDesc = '\0';
	return strName;
}
void runCommand(const char far * strCommand)
{
	char strName[COMMAND_NAME_MAX_LEN];
	if(strlen((char far *)strCommand) <= 0) return;
	getCommandName(strCommand, (char far *)strName);
	//printf("strName=%s, cmp=%d\n", (char far *)strName, strcmp((char far *)strName, "help"));
	if(strcmp((char far *)strName, "help") == 0)
		commandHelp(strCommand);
	else if(strcmp((char far *)strName, "ver") == 0)
		commandVer();
	else if(strcmp((char far *)strName, "type") == 0)
		commandType(strCommand);
	else if(strcmp((char far *)strName, "dir") == 0)
		commandDir();
	else if(strcmp((char far *)strName, "ls") == 0)
		commandDir();
	else if(strcmp((char far *)strName, "mv") == 0)
		commandMv(strCommand);
	else if(strcmp((char far *)strName, "del") == 0)
		commandDel(strCommand);
	else if(strcmp((char far *)strName, "rm") == 0)
		commandRm(strCommand);
	else
	{
		puts("I'm sorry.I don't know what's mean about '");
		puts((char far *)strName);
		puts("'.\n");
	}
}
//shell程序入口
void Shell_main(void)
{
	char strCommand[COMMAND_MAX_LEN];
	while(1)
	{
		memset((char far *)strCommand, 0, COMMAND_MAX_LEN);
		getCommand((char far *)strCommand);	
		runCommand((char far *)strCommand);
		puts("\n");
	}
}