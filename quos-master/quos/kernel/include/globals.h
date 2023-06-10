#ifndef GLOBALS_H
#define GLOBALS_H
	#include "process.h"
	extern	BYTE far * g_pVideo;		// 显存位置
	extern	BYTE g_byCursorRow;		// 当前光标位置的行号
	extern	BYTE g_byCursorCol;		// 当前光标位置的列号
	extern	BYTE g_byCurrColor;		// 当前字符显示颜色
	extern	BYTE g_byCurrProcessID;	// 当前运行的进程号
	extern	struct PCB g_Process[MAX_PROCESS];// 系统中所有进程的信息

#endif //ifdef  GLOBALS_H