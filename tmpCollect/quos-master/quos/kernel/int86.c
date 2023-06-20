//文件名：int86.c
//功能：
//	调用BIOS中断函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- int86.obj int86.c
//  如斯
//2007-6-4
#include "include/stdc.h"
int int86(int iIntNO, const union REGS far * pRegsIn, union REGS far* pRegsOut)
{
	union REGS r;
	memcpy((void far *)&r, pRegsIn, sizeof(union REGS));
	_asm
	{
		pusha
		mov	ax, iIntNO
		mov	byte ptr cs:[selfx + 1],al
		mov ax, r.x.ax
		mov bx, r.x.bx
		mov cx, r.x.cx
		mov dx, r.x.dx
		mov si, r.x.si
		mov di, r.x.di
		push r.x.cflag
		popf
selfx:		int	19h			; Bootstrap loader
		mov r.x.ax, ax
		mov r.x.bx, bx
		mov r.x.cx, cx
		mov r.x.dx, dx
		mov r.x.si, si
		mov r.x.di, di
		pushf
		pop r.x.cflag
		popa
	}
	memcpy(pRegsOut, (void far *)&r, sizeof(union REGS));
	return r.x.ax;
}
int int86x(int iIntNO, const union REGS far* pRegsIn, union REGS far* pRegsOut, struct SREGS far* pSregs)
{
	union REGS r;
	struct SREGS sr;
	memcpy((void far *)&r, pRegsIn, sizeof(union REGS));
	memcpy((void far *)&sr, pSregs, sizeof(struct SREGS));
	_asm
	{
		pusha
		push es
		push ds
		push ss

		push sr.es
		pop es
		push sr.ds
		pop ds

		mov	ax, iIntNO
		mov	byte ptr cs:[selfx + 1],al
		mov ax, r.x.ax
		mov bx, r.x.bx
		mov cx, r.x.cx
		mov dx, r.x.dx
		mov si, r.x.si
		mov di, r.x.di
		push r.x.cflag
		popf
selfx:		int	19h			; Bootstrap loader
		mov r.x.ax, ax
		mov r.x.bx, bx
		mov r.x.cx, cx
		mov r.x.dx, dx
		mov r.x.si, si
		mov r.x.di, di
		pushf
		pop r.x.cflag
		
		push es
		pop sr.es
		push ds
		pop sr.ds

		pop ss
		pop ds
		pop es
		popa
	}
	memcpy(pRegsOut, (void far *)&r, sizeof(union REGS));
	memcpy(pSregs, (void far *)&sr, sizeof(struct SREGS));
	return r.x.ax;
}
