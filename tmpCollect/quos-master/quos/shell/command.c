//文件名：command.c
//功能：
//	shell命令
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- command.obj command.c
//  如斯
//2007-7-8

#include "include/stdc.h"
#include "include/shell.h"
#include "../include/quos.h"
#include "include/command.h"
//获取命令行参数
char far *getParameter(char far * strCommand, int iCommandLen, char far * strArg)
{
	int i, j = 0;
	if(strlen(strCommand) == iCommandLen)return strArg;
	for(i=iCommandLen+1; i<strlen(strCommand); i++)
	{
		strArg[j++] = strCommand[i];
	}
	return strArg;
}
//显示帮助信息
void commandHelp(const char far *strCommand)
{
	char strAgr[COMMAND_MAX_LEN];
	if(strlen(strCommand) == 4)
	{
			puts("help [command]\n    command - displays help information on that command.\n\tcommand:\n");
			puts("\t    ver    help    type    dir\n");
			puts("\t    ls     mv      del     rm\n");
	}
	else
	{
		memset((char *)strAgr, 0, COMMAND_MAX_LEN);
		getParameter(strCommand, strlen("help"), strAgr);
		if(!strcmp(strAgr, "-h") || !strcmp(strAgr, "help") || !strcmp(strAgr, "-help"))
		{
			puts("help [command]\n    command - displays help information on that command.\n");
		}
		else if(!strcmp(strAgr, "ver"))
		{
			puts("ver\n    Displays the QUOS version.\n");
		}
		else if(!strcmp(strAgr, "type"))
		{
			puts("type fileName\n    Displays a file's content.\n");
		}
		else if(!strcmp(strAgr, "dir"))
		{
			puts("dir\n    Displays all file in currently directory.\n");
		}
		else if(!strcmp(strAgr, "ls"))
		{
			puts("ls\n    Displays all file in currently directory.\n");
		}
		else if(!strcmp(strAgr, "ls"))
		{
			puts("mv srcFile desFile\n    Move source file to destination file.\n");
		}
		else if(!strcmp(strAgr, "del"))
		{
			puts("del fileName\n    Delect a file!\n");
		}
		else if(!strcmp(strAgr, "rm"))
		{
			puts("rm fileName\n    Remove a file!\n");
		}
		else
		{
			puts("I'm sorry.\nI don't know what's mean about '");
			puts(strAgr);
			puts("'.\nTry \"");
			puts(strAgr);
			puts(" -help\" or \"");
			puts(strAgr);
			puts(" -h\", please.\n");
		}
	}

}
//显示版本信息
void commandVer()
{
	puts("Author:  ");
	puts(AUTHOR_NAME);
	puts("\nEmail:   ");
	puts(AUTHOR_EMAIL);
	puts("\nVersion: ");
	puts(QUOS_VERSION);
	puts("\nDate:    ");
	puts(BUILD_TIME);
	puts("\n");
}
//显示文本文件内容
void commandType(char far * strCommand)
{
	char strAgr[COMMAND_MAX_LEN];
	char strBuffer[514];
	FILE fi;
	int fileSize = 0;
	memset((char *)strAgr, 0, COMMAND_MAX_LEN);
	getParameter(strCommand, strlen("type"), strAgr);
	if(strlen(strAgr)<=0)
	{
		puts("Please input a file name like:\n\ttype fileName");
		return;
	}
	fi = fopen((char far *)strAgr, "");
	if(!fi)
	{
		puts("I can't found the file named");
		puts((char far *)strAgr);
		puts("!\n");
		return;
	}
	fileSize = fseek(fi, 0, SEEK_END);
	fseek(fi, 0, SEEK_SET);
	if(fileSize<=0)
		puts("error:when read file!\n");
	while(fileSize>0)
	{
		fileSize -= 512;
		memset((char far *)strBuffer, 0, 514);
		fread((char far *)strBuffer, 512, 1, fi);
		puts(strBuffer);
	}
	fclose(fi);
	puts("\n");
}
//显示当前目录中的所有文件
void commandDir()
{
	fls("");
}
//显示当前目录中的所有文件
void commandMv(char far * strCommand)
{
	char strOldName[FILE_NAME_SIZE+FILE_EXT_SIZE+1];
	char strNewName[FILE_NAME_SIZE+FILE_EXT_SIZE+1];
	DWORD dwIndex = 0;
	int newLen;
	FILE fi;
	strCommand += 3;
	strLTrim(strCommand);
	dwIndex = strnchr(strCommand, ' ', FILE_NAME_SIZE+FILE_EXT_SIZE+1);
	if(dwIndex<=0)
	{
		puts("Please input the source file!\n");
		return;
	}
	if(dwIndex>strlen(strCommand))
	{
		puts("Please input the destination file!\n");
		return;
	}
	if(dwIndex>FILE_NAME_SIZE+FILE_EXT_SIZE)
	{
		dwIndex = FILE_NAME_SIZE+FILE_EXT_SIZE;
	}
	memcpy((char far *)strOldName, strCommand, sizeof(char) * (dwIndex-1));
	strOldName[dwIndex-1] = 0;
	strCommand += dwIndex;
	strLTrim(strCommand);
	newLen = strlen(strCommand)<(FILE_NAME_SIZE+FILE_EXT_SIZE)?strlen(strCommand):(FILE_NAME_SIZE+FILE_EXT_SIZE);
	memcpy((char far *)strNewName, strCommand, newLen);
	strNewName[newLen] = 0;
	fi = fopen((char far *)strOldName, "");
	if(!fi)
	{
		puts("Sorry, I can't find the file named [");
		puts((char far *)strOldName);
		puts("]!\n");
		return;
	}
	if(frename(fi, (char far *)strNewName))
	{
		puts("Succeed!  Already move ");
		puts((char far *)strOldName);
		puts(" to ");
		puts((char far *)strNewName);
	}
	else
	{
		puts("Failed!  I can't move ");
		puts((char far *)strOldName);
		puts(" to ");
		puts((char far *)strNewName);
	}
	puts(".\n");
	fclose(fi);
}

//显示文本文件内容
void commandDel(char far * strCommand)
{
	char strAgr[COMMAND_MAX_LEN];
	char strBuffer[514];
	FILE fi;
	int fileSize = 0;
	memset((char *)strAgr, 0, COMMAND_MAX_LEN);
	getParameter(strCommand, strlen("del"), strAgr);
	if(strlen(strAgr)<=0)
	{
		puts("Please input a file name like:\n\ttype fileName");
		return;
	}
	fi = fopen((char far *)strAgr, "");
	if(!fi)
	{
		puts("I can't found the file named");
		puts((char far *)strAgr);
		puts("!\n");
		return;
	}
	if(fdelect(fi))
	{
		puts("Succeed!");
	}
	else
	{
		puts("Failed! I can't delect it. ");
	}
	fclose(fi);
	puts("\n");
}
//显示文本文件内容
void commandRm(char far * strCommand)
{
	char strAgr[COMMAND_MAX_LEN];
	char strBuffer[514];
	FILE fi;
	int fileSize = 0;
	memset((char *)strAgr, 0, COMMAND_MAX_LEN);
	getParameter(strCommand, strlen("rm"), strAgr);
	if(strlen(strAgr)<=0)
	{
		puts("Please input a file name like:\n\ttype fileName");
		return;
	}
	fi = fopen((char far *)strAgr, "");
	if(!fi)
	{
		puts("I can't found the file named");
		puts((char far *)strAgr);
		puts("!\n");
		return;
	}
	if(fdelect(fi))
	{
		puts("Succeed!");
	}
	else
	{
		puts("Failed! I can't delect it. ");
	}
	fclose(fi);
	puts("\n");
}