//文件名：convert.c
//功能：
//	数据的转换
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- convert.obj convert.c
//  如斯
//2007-6-3 
#include "include/stdc.h"
WORD atoi(const char far * pStrIn)
{
	return (WORD)atol(pStrIn);
}
DWORD atol(const char far * pStrIn)
{
	DWORD dwRet = 0;
	BYTE byLen = strlen(pStrIn), i = 0;
	if(pStrIn==NULL)return 0;
	for(i=0; i<=byLen; i++)
	{
		if(pStrIn[i]>='0' && pStrIn[i]<='9')
			dwRet = dwRet * 10 + pStrIn[i] - '0';
		else
			break;
	}
	return dwRet;
}
char far * itoa(WORD wNumber, char far * pStrOut)
{
	ltoa((DWORD)wNumber, pStrOut);
}
char far * ltoa(DWORD dwNumber, char far * pStrOut)
{
	int nTemp, i = 0, j;

	if(dwNumber == 0)
	{
		pStrOut[0]	='0';
		pStrOut[1]	='\0';
		return pStrOut;
	}

	while (dwNumber!=0)
	{
		nTemp		= (int)(dwNumber%10);
		dwNumber	= dwNumber / 10;
		for (j = i; j > 0; j--)
		{
			pStrOut[j] = pStrOut[j-1];
		}
		pStrOut[0] = '0' + nTemp;
		i++;
	}
	pStrOut[i]	= '\0';
	return pStrOut;
}
DWORD xtol(char far * pStrOut)
{
	DWORD dwNumber = 0;
	WORD nTemp = 0, i = 0, len = strlen(pStrOut)-1;
	while(pStrOut[i]!='\0')
	{
		DWORD basic = pow(16, len-i);
		if(pStrOut[i]>='0' && pStrOut[i]<='9')
		{
			nTemp = pStrOut[i] - '0';
		}
		else if(pStrOut[i]>='a' && pStrOut[i]<='f')
		{
			nTemp = pStrOut[i] - 'a' + 10;
		}
		else if(pStrOut[i]>='A' && pStrOut[i]<='F')
		{
			nTemp = pStrOut[i] - 'A' + 10;
		}
		else if(pStrOut[i]=='x' || pStrOut[i]=='X')
		{
			nTemp = 0;
		}
		else
		{
			dwNumber = 0;
			break;
		}
		dwNumber += nTemp * basic;
		i++;
	}
	return dwNumber;
}

//线性地址转化成长指针地址
void far * lineToFar(DWORD dwIn)
{
	DWORD fp = dwIn >> 4;
	DWORD dw= (dwIn & 0xf);
	fp <<= 16;
	fp += dw;
	return (void far *)fp;
}

//长指针地址转化成线性地址
DWORD farToLine(const void far * pIn)
{
	DWORD seg = FP_SEG(pIn);
	WORD off = FP_OFF(pIn);
	seg <<= 4;
	//off &= 0xF; 
	seg += off;
	return seg;
}

char far * strTOSuper(char far * pString)
{
	int i;
	for(i=0; i<= strLen(pString); i++)
	{
		if(*(pString+i)>='a' && *(pString+i)<='z')
			*(pString+i) -= 32;
	}
	return pString;
}
char far * strTOLower(char far * pString)
{
	int i;
	for(i=0; i<= strLen(pString); i++)
	{
		if(*(pString+i)>='A' && *(pString+i)<='Z')
			*(pString+i) += 32;
	}
	return pString;
}
char super(char ch)
{
	if(ch>='a' && ch<='z')
		return ch - 32;
}
char lower(char ch)
{
	if(ch>='A' && ch<='Z')
		return ch + 32;
}