//文件名：mem.c
//功能：
//	内存操作相关函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- mem.obj mem.c
//  如斯
//2007-6-3 
#include "include/stdc.h"
void memcpy(void far *pDest, const void far *pSrc, WORD wCount)
{
    WORD i = 0;
    for(i = 0; i < wCount; i++)
    {
        ((BYTE far *)pDest)[i] = ((BYTE far *)pSrc)[i];
    }
}

WORD memcmp(void far *pDest, const void far *pSrc, WORD wCount)
{
	int i = 0;
	for(i = 0; i < (int)wCount; i++)
    {
        BYTE by1, by2;
        by1	= ((BYTE far*)pDest)[i];
        by2	= ((BYTE far*)pSrc)[i];
        if(by1 != by2)
            break;
	}

	if(i >= (int)wCount) i = -1;

	return (WORD)(i+1);
}

void memset(void far *pDest, BYTE byChar, WORD wCount)
{
    int i = wCount;
    if(i < 0 )
    {
        return;
    }
    _asm
    {
        pushf
        push    di
        push    es
        les     di, pDest
        mov     cx, wCount
        cld
        mov     al, byChar
        mov     ah, al
        shr     cx, 1
        rep     stosw
        adc     cx, cx
        rep     stosb
        pop     es
        pop     di
        popf
    }
}
void far *malloc(WORD wSize)
{
	union REGS r;
	struct SREGS sr;
	DWORD d;
	r.h.ah = 0x48;
	r.x.bx = wSize;
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGS far *)&sr);
	d = (DWORD)sr.es * 0x10 + r.x.bx;

	return lineToFar(d);
}
void free(void far * pMem)
{
	union REGS r;
	struct SREGS sr;
	r.h.ah = 0x49;
	r.x.bx = FP_OFF(pMem);
	sr.es = FP_SEG(pMem);
	int86x(0x22, (union REGS far *)&r, (union REGS far *)&r, (struct SREGS far *)&sr);
}