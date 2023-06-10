                .MODEL  SMALL
                .386
_Shell_main			PROTO	NEAR
			Stack_Seg		equ 06000h
			Stack_Off		equ 0ffffh
	.data
	.CODE
				ORG     00h
				assume cs:@code ,ds:@data
   
Public $begin
$begin:

start:	   
    jmp     main
main:
	;初始化环境
;	mov ax, Stack_Seg
;	mov ss, ax
;	mov sp, Stack_Off
	mov ax, @data
	mov es, ax
	mov ds, ax
	INVOKE _Shell_main
out_e:
;	mov al, 'e'; // 输出一个字母
;	mov ah, 09h
;	int 10h
    mov ah, 0Eh
    mov al, 'h'
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, 'e'
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, 'l'
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, 'l'
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, 'o'
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, ' '
    mov bl, 0Fh
    int 10h
    mov ah, 0Eh
    mov al, ' '
    mov bl, 0Fh
    int 10h
	; 停机
	hlt
	nop
	; 返回系统
    mov ax, 4c00h
    int 22h
	jmp out_e
    end	start