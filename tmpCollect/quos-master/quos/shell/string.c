//文件名：string.c
//功能：
//	字符串处理函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- string.obj string.c
//  如斯
//2007-6-3
#include "include/stdc.h"
void puts(const char far * pStr)
{
	int i;
	for(i = 0; pStr[i] != 0; i++)
	{
		if(pStr[i] == '\n')
		{
			putchar('\n');
			putchar('\r');
		}
		else if(pStr[i] == '\t')
		{
			puts("    ");
		}
		else
		{
			putchar(pStr[i]);
		}
	}
}

char far * gets(char far * pStrOut)
{
	char far * pWork = pStrOut;
	char temp = getch();
	DWORD d = farToLine(pStrOut);
	while(temp != '\n' && temp != '\r')
	{
		if(temp == '\b')
		{
			if((DWORD)farToLine(pWork) > d)
			{
				pWork--;
				putchar('\b');
				putchar(' ');
				putchar('\b');
			}
		}
		else
		{
			putchar(temp);
			*pWork = temp;
			pWork++;
		}
		temp = getch();
	}
	*pWork = '\0';
	putchar('\n');
	putchar('\r');
	return pStrOut;
}

int strcmp(const char far * pStr1, const char far * pStr2)
{
	int j = strlen(pStr1), i;
	for(i=0; i<=j; i++)
	{
		if(pStr1[i] > pStr2[i]) return 1;
		else if(pStr1[i] < pStr2[i]) return -1;
	}
	return 0;
}

unsigned int strlen(char far * pStr)
{
	int i = 0;
	if(pStr == NULL) return 0;
	while(pStr[i] != '\0')i++;
	return i;
}

char far * strcpy(char far * pStrOut, const char far * pStrIn)
{
	char far *pStrTo = pStrOut;
	char far *pStrFrom = pStrIn;
	if(pStrTo == NULL || pStrFrom == NULL )
	{
		return pStrOut;
	}
	while(1)
	{
		if(*pStrFrom == 0)
		{				
			*pStrTo = 0;
			break;
		}
		*pStrTo = *pStrFrom;		
		pStrTo++;
		pStrFrom++;
	}
	return pStrOut;
}
char far * strncpy(char far * pStrOut, const char far * pStrIn, DWORD count)
{
	char far *pStrTo = pStrOut;
	char far *pStrFrom = pStrIn;
	if(pStrTo == NULL || pStrFrom == NULL )
	{
		return pStrOut;
	}
	while(count--)
	{
		if(*pStrFrom == 0)
		{				
			*pStrTo = 0;
			break;
		}
		*pStrTo = *pStrFrom;		
		pStrTo++;
		pStrFrom++;
	}
	return pStrOut;
}

char far * strcat(char far * pStrOut, const char far * pStrIn)
{
	BYTE far * pStrTo = pStrOut;
	BYTE far * pStrFrom = pStrIn;
	if(pStrTo == NULL || pStrFrom == NULL) return pStrOut;
	while(1)
	{
		if(*pStrTo == 0)break;
		pStrTo++;
	}
	while(1)
	{
		*pStrTo = *pStrFrom;
		if(*pStrFrom == 0)break;
		pStrTo++;
		pStrFrom++;
	}
	return pStrOut;
}
// 去到字符串左边的空格
char far * strLTrim(char far * pStr)
{
	char far * pWork = pStr;
	while(1)
	{
		if(*pWork != ' ')
			break;
		pWork++;
	}
	while(*pWork != '\0')
	{
		*pStr = *pWork;
		pStr++;
		pWork++;
	}
	*pStr = '\0';
}
DWORD strnchr(const char far * pStrOut, char ch, DWORD len)
{
	DWORD i;
	for (i=0; (pStrOut[i]!='\0' && i<len); i++)
	{
		if(pStrOut[i] == ch)
		{
			return i+1;// 从1开始算起
		}
	}
	return 0;
}