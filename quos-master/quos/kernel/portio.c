//文件名：portio.c
//功能：
//	端口读写相关函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- portio.obj portio.c
//  如斯
//2007-6-3 
#include "include/stdc.h"
void outb(WORD port, BYTE databyte)
{
	_asm
	{
		mov	al,databyte
		mov	dx,port
		out	dx,al
	}
}

void outw(WORD port, WORD dataword)
{
	_asm
	{
		mov	ax,dataword
		mov	dx,port
		out	dx,ax
	}
}
BYTE inb(WORD port)
{
	BYTE byRet = 0;
	_asm
	{
		mov	dx,port
		in	al,dx
		xor	ah,ah
		mov byRet, al
	}
	return byRet;
}
WORD inw(WORD port)
{
	WORD wRet = 0;
	_asm
	{
		mov	dx,port
		in	al,dx
		mov wRet, ax
	}
	return wRet;
}