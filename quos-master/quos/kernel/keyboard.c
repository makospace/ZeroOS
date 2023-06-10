//文件名：keyboard.c
//功能：
//	通用键盘驱动
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- keyboard.obj keyboard.c
//  如斯
//2007-6-3
#include "include/keyboard.h"
BYTE keyBroadState(void)
{
	BYTE byState;
	_asm
	{
		mov     ah, 02h
		int     16h
		mov     byState, al
	}
	return byState;
}
BYTE getKeyChar(void)
{
	return (BYTE)(getKeyCodeChar()<<8>>8);
}
BYTE getKeyCode(void)
{
	return (BYTE)(getKeyCodeChar()>>8);
}
WORD getKeyCodeChar(void)
{
	WORD wKey;
	WORD wZF = TRUE;
	_asm
	{
		mov     ah, 01h
		int     16h
		jnz		NoZero
		mov		wZF, FALSE
	NoZero://如果有字符输入，返回获取到的字符，并把键盘缓冲区清空，因为中断不会自动清空
		mov     wKey, ax

		//清空键盘缓冲区
		mov		ax, 0x40
		mov		es, ax
		mov		ax, es:0x1a
		mov		es:0x1c, ax
	}
	
	if(wZF)
		return wKey;
	else
		return wZF;
}
BYTE getKeyCharWait(void)
{
	return (BYTE)(getKeyCodeCharWait()<<8>>8);
}
BYTE getKeyCodeWait(void)
{
	return (BYTE)(getKeyCodeCharWait()>>8);
}
WORD getKeyCodeCharWait(void)
{
	WORD wKey;
	_asm
	{
		mov     ah, 00h
		int     16h
		mov     wKey, ax
	}
	return wKey;
}
char CodeToChar(BYTE byCode)
{
	//switch(by)
		//case 1:return 
		return 0;
}