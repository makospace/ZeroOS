//文件名：screes.c
//功能：
//	屏幕驱动
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- screes.obj screes.c
//  如斯
//2007-6-16
#include "include/stdc.h"
#include "include/screen.h"

BYTE far * g_pVideo = 0;		// 显存位置
BYTE g_byCursorRow = 0;			// 当前光标位置的行号
BYTE g_byCursorCol = 0;			// 当前光标位置的列号
BYTE g_byCurrColor = COLOR_GRAY;// 当前字符显示颜色
BYTE g_bySystemColor = COLOR_GRAY;// 仅仅用于备份
void setScreenColor(BYTE byColor)
{
	g_byCurrColor = byColor;
}
BYTE getScreenColor()
{
	return g_byCurrColor;
}
void getComebackScreenColor()
{
	g_byCurrColor = g_bySystemColor;
}

//文本模式初始化
void screenInit(BYTE byModeNO)
{
	WORD far * pCursor = lineToFar(0x450);
	_asm
	{
		mov al, byModeNO
		mov ah, 00
		int 10h
	}
	// 初始化输入输出
	g_byCursorRow = *pCursor>>8;
	g_byCursorCol = (*pCursor&0xff) * 2;
	g_pVideo = lineToFar(0xb8000);
	g_byCurrColor = COLOR_GRAY;	
}

void screenShowChar(char ch)
{
	WORD far * pCursor = lineToFar(0x450);
	DWORD dwCharAdd = 0;
	g_byCursorRow = *pCursor >> 8;
	g_byCursorCol = (*pCursor & 0xff) * 2;
	dwCharAdd = g_byCursorRow * (MAX_COL + 1) * 2 + g_byCursorCol;
	// 处理光标位置
	switch (ch)
	{
		case '\n':
			g_byCursorRow++;
			break;
		case '\r':
			g_byCursorCol=0;
			break;
		case '\b':
			if(g_byCursorCol>=2)
			{
				g_byCursorCol-=2;
			}
			else
			{
				g_byCursorRow--;
				g_byCursorCol = MAX_COL * 2;
			}
			break;
		default:
			g_pVideo[dwCharAdd] = ch;
			dwCharAdd++;
			g_pVideo[dwCharAdd] = g_byCurrColor;
			dwCharAdd++;
			g_byCursorCol += 2;
	}
	update_cursor();
}
void update_cursor()
{
	// 检查是否越界
	if(g_byCursorCol > MAX_COL * 2)// 换行处理
	{
		g_byCursorCol=0;
		g_byCursorRow++;
	}
	if(g_byCursorRow > 24)// 滚屏处理
	{
		screenUp(1);
	}
	_asm
	{
		mov ah, 2
		mov bh, 0
		mov dh, g_byCursorRow
		mov dl, g_byCursorCol
		shr dl, 1
		int 10h
	}
}
//上卷屏
void screenUp(BYTE byCount)
{
	_asm
	{
		mov	ah, 6
		mov	al, byCount
		mov	bh, g_byCurrColor
		mov	ch, 1 ;// 最上面一行不滚屏
		mov	cl, 0
		mov dh, MAX_ROW
		mov dl, MAX_COL
		int 10h
	}
	g_byCursorRow--;
	update_cursor();
}
//文本模式清屏
void screenClear(void)
{
	_asm
	{
		mov	ah, 6
		mov	al, 0
		mov	bh, 7
		mov	ch, 0
		mov	cl, 0
		mov dh, MAX_ROW
		mov dl, MAX_COL
		int 10h
	}
	g_byCursorCol = 0;
	g_byCursorRow = 0;
	update_cursor();
}