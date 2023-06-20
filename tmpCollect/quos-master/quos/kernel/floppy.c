//文件名：floppy.c
//功能：
//	提供对软驱的读写操作
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- floppy.obj floppy.c
//  如斯
//2007-6-11
#include "include/stdc.h"
#include "include/floppy.h"

BOOL floppyRead(BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuffer)
{
	return Int13Floppy(0x2, bCount, bTrack, bSector, bHead, bDriveNO, pBuffer);
}

BOOL floppyWrite(BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuffer)
{
	return Int13Floppy(0x3, bCount, bTrack, bSector, bHead, bDriveNO, pBuffer);
}
BOOL Int13Floppy(BYTE bKey,BYTE bCount, BYTE bTrack, BYTE bSector, BYTE bHead, BYTE bDriveNO, void far *pBuf)
{
	union REGS r;
	struct SREGS sr;
	memset((union REGS far *)&r, 0, sizeof(union REGS));
	memset((struct SREGS far *)&sr, 0, sizeof(struct SREGS));
	#ifdef DEBUG_FLOPPY
	{
		printf("\tInt13Floppy: bKey=0x%x  bCount=0x%x  bTrack=0x%x  bSector=0x%x\n", bKey, bCount, bTrack, bSector);
		printf("\tInt13Floppy: bHead=0x%x  bDriveNO=%d  pBuf=0x%lx\n", bHead, bDriveNO, farToLine(pBuf));
	}
	#endif
	r.h.ah = bKey;
	r.h.al = bCount;
	r.h.ch = bTrack;
	r.h.cl = bSector;
	r.h.dh = bHead;
	r.h.dl = bDriveNO;
	sr.es = FP_SEG(pBuf);
	r.x.bx = FP_OFF(pBuf);
	int86x(0x13, (union REGS far *)&r, (union REGS far *)&r, (struct SREGSfar *)&sr);
	
	#ifdef DEBUG_FLOPPY
	{
		printf("\tInt13Floppy: return:\n\tr.x.ax=0x%x r.x.cflag=%b\n", r.x.ax, r.x.cflag);
		getch();
	}
	#endif
	return !(r.x.cflag & FLAG_CARRY);
}