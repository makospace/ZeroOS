//文件名：panic.c
//功能：
//	错误报告相关的函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- panic.obj panic.c
//  如斯
//2007-6-16
#include "include/stdc.h"
#include "include/panic.h"
#include "include/globals.h"
#include "../include/quos.h"

void panic(DWORD errorNO)
{
	g_byCurrColor = COLOR_READ;
	printf("\nI'm sorry.I can't continue to work, because the fatal error[0x%lx]!\n", errorNO);
	_asm
	{
		HLTC:
			hlt
			jmp HLTC
	}
}
void bug(DWORD bugNO)
{
#ifdef DEBUG
	BYTE old = g_byCurrColor;
	g_byCurrColor = COLOR_GREEN;
	printf("\nPlease tell the author %s, he have a BUG which it's NO is 0x%lx!\n", (char far *)AUTHOR_NAME, bugNO);
	printf("\tHis email adress:%s\n", (char far *)AUTHOR_EMAIL);
	printf("\tcurrent version NO:%s %s\n", (char far *)QUOS_VERSION, BUILD_TIME);
	puts("Thank you very much!\n\n");
	g_byCurrColor = old;
#endif
}