//文件名：process.c
//功能：
//	进程管理相关的函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- process.obj process.c
//  如斯
//2007-6-16
#include "include/process.h"
#include "include/file.h"
#include "include/stdc.h"
#include "include/errorNO.h"
#include "include/memManage.h"
BYTE g_byCurrProcessID = 0;		// 当前运行的进程号
struct PCB g_Process[MAX_PROCESS];// 系统中所有进程的信息
BOOL processManageInit()
{
	BYTE i = 0;
	g_byCurrProcessID = 0;
	for(i=0; i<MAX_PROCESS; i++)
		memset(lineToFar(farToLine((struct PCB far *)g_Process) + i), 0, sizeof(struct PCB));

	// 初始化系统进程
	strcpy((char far *)g_Process[0].strRootPath, "a:\\");
}
struct PCB far * allocProcessNO()
{
	int i = 1;
	for(i=1; i<MAX_PROCESS; i++)
	{
		if(g_Process[i].byProcessNO==0)
		{
			g_Process[i].byProcessNO = i;
			return (struct PCB far *)&g_Process[i];
		}
	}
	return 0;
}
void freeProcessNO(BYTE byProcessNO)
{	
	if(g_Process[byProcessNO].byProcessNO == 0)
	{// 释放没有使用的进程控制块或系统进程控制块，这是不允许的，肯定是BUG。当然，也有可能是崩溃了
		bug(BUG_freeProcessNO_1);
	}
	else if(g_Process[byProcessNO].byProcessNO == byProcessNO)
	{
		g_Process[byProcessNO].byProcessNO = 0;
	}
	else
	{// 如果进程号既不是0也不是正确的值，那肯定系统被破坏了，没有办法只好死机了
		panic(PANIC_freeProcessNO_1);
	}
	return 0;
}
int exec(char far * strProgameName, char far * args)
{
	DWORD dwProgSize = 0, dwHeadSize = 0, dwBodySize = 0;
    WORD far *pBody = 0, far *pProgFile = 0, far *p = 0, far *pReallocTable = 0;
	BYTE far *pBuf = 0;
    WORD i = 0, wSeg = 0, wOff = 0;
	struct exeHead far *pHead = 0;
	struct PCB far *pPCB;
	BYTE byRet = 0;
	FILE FileNO = 0;
	
	// 将程序读入内存
	FileNO = fopen(strProgameName, "r");
	if(!FileNO)
	{
		#ifdef DEBUG_PROCESS
		puts("\texec:Error when open the progame file.\n");
		#endif
		goto error;
	}
	dwProgSize = fseek(FileNO, 0, SEEK_END);
	fseek(FileNO, 0, SEEK_SET);
	pBuf = memMalloc(dwProgSize+0x10);// 多申请16个字节，以防止修正指针后读取文件会溢出
	if(!pBuf)
	{
		#ifdef DEBUG_PROCESS
		puts("\texec:Error when alloc the progame space.\n");
		#endif
		goto error;
	}

	wSeg = FP_SEG(pBuf);
	wOff = FP_OFF(pBuf);
	// 修正内存指针
	if(wOff)
	{
		wSeg++;
		pProgFile = MK_FP(wSeg, 0);
	}
	else
	{
		pProgFile = pBuf;
	}
	byRet = fread(pProgFile, dwProgSize, 1, FileNO);
	if(!byRet)
	{
		#ifdef DEBUG_PROCESS
		puts("\texec:Error when read the progame to memory.\n");
		#endif
		goto errorFree;
	}
	fclose(FileNO);


	// 解析程序
	pHead = (struct exeHead far *)pProgFile;
	// 验证幻数是否有效
	if(pHead->wMagic != 0x5A4D)
	{
		// 也就是说是COM程序，也就无需解析直接跳转过去就可以了
		#ifdef DEBUG_PROCESS
		printf("\texec:This file isn't a dos-EXE program! pHead->wMagic=0x%x\n", pHead->wMagic);
		#endif
		goto ToUserProcess;
	}
	
		#ifdef DEBUG_PROCESS
		printf("\texec:This file is a dos-EXE program! pHead->wMagic=0x%x\n", pHead->wMagic);
		#endif
	//计算exe头的大小
	dwHeadSize = pHead->wHeadSize * 0x10;
	dwBodySize = dwProgSize - dwHeadSize;
	pBody = lineToFar(farToLine(pProgFile) + dwHeadSize);
	// 解决重定向问题，将需要重定向的地方加上程序的段地址
	pReallocTable = (WORD far *)(pHead->wFirstRealloc + (DWORD)pHead);
    for (i=0; i<pHead->wReallocCount; i++)
    {
        p = (WORD far *)lineToFar(farToLine(pBody) + (DWORD)(pReallocTable[2*i] + pReallocTable[2*i+1] * 0x10));
				#ifdef DEBUG_PROCESS
				printf("%d[%ld]  *p=%04x   ", i, (DWORD)(pReallocTable[2*i] + pReallocTable[2*i+1] * 0x10), *(WORD far *)p, farToLine(p));
				#endif
				*p = *p + wSeg + 0x20;
				#ifdef DEBUG_PROCESS
				printf("%04x  %04x\n", *(WORD far *)p, wSeg);
				#endif
    }
	// 计算用户程序目标位置
	wSeg = FP_SEG(pBody);
	wOff = FP_OFF(pBody);

ToUserProcess:
	// 创建进程管理信息
	pPCB = allocProcessNO();
	if(!pPCB)
	{
		#ifdef DEBUG_PROCESS
		puts("\texec:Error when alloc the process NO.\n");
		#endif
		goto errorFree;
	}
	strcpy((char far *)pPCB->strRootPath, "a:\\");
	strcpy((char far *)pPCB->strArg, args);
	// 修改当前进程号
	g_byCurrProcessID = pPCB->byProcessNO;
	#ifdef DEBUG_PROCESS
	printf("\texec:jmp to user progame 0x%04x:0x%04x\n", wSeg, wOff);
	#endif
	_asm
	{
		;// 保持当前状态，以便从用户进程返回的时候恢复
		mov g_Process[0].r.x.ax, ax
		mov g_Process[0].r.x.bx, bx
		mov g_Process[0].r.x.cx, cx
		mov g_Process[0].r.x.dx, dx
		mov g_Process[0].r.x.si, si
		mov g_Process[0].SP, sp
		mov g_Process[0].BP, bp
		mov g_Process[0].r.x.di, di
		pushf
		pop g_Process[0].r.x.cflag
		push ds
		pop g_Process[0].sr.ds
		push es
		pop g_Process[0].sr.es
		push cs
		pop g_Process[0].sr.cs
		push ss
		pop g_Process[0].sr.ss
		mov g_Process[0].IP, offset RETURN_ADD
		
		// 跳转到用户进程
		push wSeg;
		push wOff;
		retf
	RETURN_ADD:
		mov byRet, al
	}
	// 释放用户进程的管理信息块
	freeProcessNO(g_byCurrProcessID);
	
	// 修改当前进程号为系统进程
	g_byCurrProcessID = 0;

	// 释放掉用户程序所使用的内存
	memFree(pBuf);
	#ifdef DEBUG_PROCESS
	printf("\n\texec:return from user progame %p\n", pProgFile);
	getch();
	#endif
	// 返回系统
	return byRet;

errorFree:
	memFree(pProgFile);
error:
	return -1;
}