//文件名：file.c
//功能：
//	文件操作相关的函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- file.obj file.c
//  如斯
//2007-6-16
#include "include/stdc.h"
FILE fopen(char far * strFileName, char far * strMode)
{	
	union REGS r;
	struct SREGS sr;
	sr.es = FP_SEG(strFileName);
	r.x.dx = FP_OFF(strFileName);
	r.h.ah = 0x20;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
	//printf("fileName=%s, fileNO=%d\n", strFileName, r.x.bx);
	//getch();
	return r.x.bx;
error:
	return 0;
}
DWORD fread(BYTE far * pDest, WORD wSize, BYTE byCount, FILE fileNo)
{
	union REGS r;
	struct SREGS sr;

	//printf("fileNO=%d\n", fileNo);
	//getch();
	sr.es = FP_SEG(pDest);
	r.x.dx = FP_OFF(pDest);
	r.x.cx = wSize;
	r.x.bx = fileNo;
	r.h.al = byCount;
	r.h.ah = 0x21;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
	return r.x.ax;
}
void fclose(FILE fileNo)
{
	union REGS r;
	r.x.bx = fileNo;
	r.h.ah = 0x22;
	int86(0x22, (union REGS far *)&r, (union REGS far *)&r);
}
DWORD fseek(FILE fileNo, DWORD dwOffset, BYTE by0rigin)
{
	union REGS r;
	r.x.bx = fileNo;
	r.x.dx = dwOffset>>16;
	r.x.cx = dwOffset;
	r.h.ah = 0x23;
	r.h.al = by0rigin;
	int86(0x22, (union REGS far *)&r, (union REGS far *)&r);
	return r.x.ax;
}
void fls(char far * strPathName)
{	
	union REGS r;
	struct SREGS sr;
	sr.es = FP_SEG(strPathName);
	r.x.dx = FP_OFF(strPathName);
	r.h.ah = 0x24;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
}
BOOL frename(FILE fileNo, char far * strFileName)
{
	union REGS r;
	struct SREGS sr;
	r.x.bx = fileNo;
	sr.es = FP_SEG(strFileName);
	r.x.dx = FP_OFF(strFileName);
	r.h.ah = 0x25;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
	return r.x.ax;
}
BOOL fdelect(FILE fileNo)
{
	union REGS r;
	struct SREGS sr;
	r.x.bx = fileNo;
	r.h.ah = 0x26;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
	return r.x.ax;
}