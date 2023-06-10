; 文件名：kernel.asm
; 功能：	 一个测试程序,仅仅用于测试，没有什么实际功能。虽然这儿是TINY类型的COM格式，但是我们最终会编译一个真正的DOS exe格式的程序出来。
; 编译方法：
;		ML /c /Fo kernel.obj kernel.asm
;		link /TINY kernel.obj, kernel.exe, kernel.map, , ,
;   如斯
; 2007-5-31
				.MODEL  small
                .386
kMain PROTO near	C
sysInt_int22 PROTO near 	C , addSeg:WORD, addOff:WORD
			Stack_Seg		equ 09000h
			;Stack_Off		equ 0ffffh
	.data
	oldip dw ?
	oldcs dw ?
	oldcflag dw ?
	.code
				ORG     00h
				assume cs:@code ,ds:@data

Public $begin 
$begin:
main:
;----------------------------------------------------------------------------------
;=======================================================================
;			安装新的时钟中断来替换掉BIOS原来的时钟中断
;=======================================================================

		mov ax, @data
		mov ds, ax
		mov es, ax
;		mov ax, Stack_Seg
;		mov ss, ax
; 计算堆栈地址
		mov eax, 0
		int 12h
		inc ax
		mov ebx, 400h
		mul ebx ; ax里面就是堆栈的偏移量
;		mov ebx, eax ; ebx里面就是堆栈的线性地址
;		mov eax, 0
;		mov ax, bx
		dec ax ;如果是0的话就变成0xffff
		mov sp, ax
		mov bx, 10h
		div bx ; ax里面就是堆栈的段地址
		mov ss, ax
		;mov sp, Stack_Off
		call _sysInt_init
; 调用C语言的函数
		call kMain

_sysInt_init proc near
		mov ax, 0
		mov es, ax
		mov ax, offset _sysInt
		mov es:[22h*4], ax		;// 偏移量
		mov ax, cs
		mov es:[22h*4+2], cs	;// 段地址
		ret

_sysInt_init	endp
_sysInt	proc	far
				; 保存用户传入的参数
				pushf
				pop cs:INT_FLAG
                mov     cs:INT_CS, cs
                mov     cs:INT_DS, ds
                mov     cs:INT_ES, es
                mov     cs:INT_SS, ss
                mov     cs:INT_SP, sp
                mov     cs:INT_BP, bp
                mov     cs:INT_DI, di
                mov     cs:INT_SI, si
                mov     cs:INT_AX, ax
                mov     cs:INT_BX, bx
                mov     cs:INT_CX, cx
                mov     cs:INT_DX, dx

				; 设置新的运行环境
;				mov eax, 0
;				int 12h
;				inc ax
;				mov ebx, 400h
;				mul ebx ; ax里面就是堆栈的偏移量
;				dec ax ;如果是0的话就变成0xffff
;				mov sp, ax
;				mov bx, 10h
;				div bx ; ax里面就是堆栈的段地址
;				mov ss, ax
				
				mov ax, @data
				mov ds, ax
				mov es, ax
				mov ax, offset userArg
				INVOKE sysInt_int22, cs, ax

				; 恢复用户的运行环境
				push cs:INT_FLAG
				popf				
                mov     ax, cs:INT_AX
                mov     bx, cs:INT_BX
                mov     cx, cs:INT_CX
                mov     dx, cs:INT_DX
                mov     di, cs:INT_DI
                mov     si, cs:INT_SI
                mov     sp, cs:INT_SP
                mov     bp, cs:INT_BP
                mov     es, cs:INT_ES
                mov     ds, cs:INT_DS
                mov     ss, cs:INT_SS

; 修改标志寄存器
;		pop oldip
;		pop oldcs
;		pop oldcflag

;		pushf
;		push oldcs
;		push oldip
		iret

userArg:
	            INT_FLAG            dw ? ;
	            INT_AX              dw ? ;
	            INT_BX              dw ? ;
	            INT_CX              dw ? ;
	            INT_DX              dw ? ;
	            INT_DI              dw ? ;
	            INT_SI              dw ? ;
	            INT_BP              dw ? ;
	            INT_SP              dw ? ; 
	            INT_CS              dw ? ;
	            INT_DS              dw ? ;
	            INT_ES              dw ? ;
	            INT_SS              dw ? ; 


_sysInt	endp
end	main