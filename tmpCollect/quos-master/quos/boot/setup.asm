; 文件名：setup.asm
; 功能：
;	在软盘根目录找到kernel.exe并加载运行它。注意这儿的kernel最终会是dos exe格式的，所以需要解析后才能运行。
; 编译方法：
;		ML /c /Fo setup.obj setup.asm
;   如斯
; 2007-5-31
				.MODEL  TINY
                .386
;----------------------------------------------------------------------------------------------------
;===============================================================================
;									一些常量
;===============================================================================
Kernel_Seg			equ  07000h ; kernel.exe 被加载到内存后所在位置的段地址
Kernel_Off			equ  00h    ; kernel.exe 被加载到内存后所在位置的偏移地址
Kernel_PhyAddr		equ Kernel_Seg * 10h   ; kernel.exe 被加载到内存后所在位置的物理地址

; 基于 FAT12 头的一些常量定义，如果头信息改变，下面的常量可能也要做相应改变
FAT_Sz				equ 9   ; BPB_FATSz16
RootDir_Sectors_Count	equ 14  ; 根目录占用空间: RootDir_Sectors_Count = ((BPB_RootEntCnt * 32) + (Byts_Per_Sector – 1)) / Byts_Per_Sector; 但如果按照此公式代码过长
RootDir_SectorNo	equ 19  ; Root Directory 的第一个扇区号 = BPB_RsvdSecCnt + (BPB_NumFATs * FAT_Sz)
FAT1_SectorsNo		equ 1   ; FAT1 的第一个扇区号   = BPB_RsvdSecCnt
Delta_SectorNo		equ 17  ; Delta_SectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FAT_Sz) - 2
                            ; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目 + Delta_SectorNo
Byts_Per_Sector		equ 512 ; 每扇区字节数
Sector_Per_Track	equ 18  ; 每磁道扇区数
Drive_NO			equ 0   ; 中断 13 的驱动器号
;----------------------------------------------------------------------------------------------------

	.data
;----------------------------------------------------------------------------------------------------
	wRootDirSizeForLoop dw  RootDir_Sectors_Count  ; Root Directory 占用的扇区数, 在循环中会递减至零.
	wSectorNo           dw  0               ; 要读取的扇区号
	bOdd                db  0               ; 奇数还是偶数
	strFileName            db  "KERNEL  EXE", 0; 文件名
	strLoading	        db  "Loading kernel    "	; 序号 0，加载内核文件提示信息
	Loading_Len	        equ	$-offset strLoading		; 序号 0字符串长度
	strOKHead			db  "        ["				; 序号 1，加载完成信息
	OKHead_Len			equ	$-offset strOKHead		; 序号 1字符串长度
	strOKTail			db  "]", 0dh, 0ah			; 序号 1，加载完成信息
	OKTail_Len			equ	$-offset strOKTail		; 序号 1字符串长度
	strNoKernel			db  0dh, 0ah, "Fatal[1]: No kernel.exe!"			; 序号 2，没有找到
	NoKernel_Len		equ	$-offset strNoKernel				; 序号 2字符串长度
	strMemTooSamll		db  0dh, 0ah, "Fatal[2]: The memory is too small!"			; 序号 2，没有找到
	MemTooSamll_Len		equ	$-offset strMemTooSamll				; 序号 2字符串长度
;----------------------------------------------------------------------------------------------------
	.code
				ORG     00h
				assume cs:@code ,ds:@data

Public $begin
$begin:
;----------------------------------------------------------------------------------------------------
main:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, ax
; 检验是否有足够的内存
	int 12h
	cmp ax, 254h ; 内核最大是128kb，又会被加载到70000h处，所以内存必须大于95000h(系统堆栈还至少需要5000h)才行，95000h=0x254kb
	ja CanLoadKernel

	mov cx, MemTooSamll_Len
	mov ax, offset strMemTooSamll
	call DispStr
HALT1:
    hlt
    jmp HALT1  ; 死机
 
CanLoadKernel:
	mov cx, Loading_Len
    mov ax, offset strLoading
    call DispStr; 显示字符串"Loading kernel.exe"

	call LoadKernel

	; 加载kernel.exe成功
	mov cx, OKHead_Len
    mov ax, offset strOKHead
    call DispStr

	mov	al, 'O'
	mov	ah, 09h
	mov bh, 0
	mov	bl, 0ah
	mov	cx, 01
	int	 10h

;//获取光标位置
	mov		ah, 03
	mov		bh, 00
	int		10h
;//重新设置光标位置
	inc		dl
	mov		ah, 02
	mov		bh, 00
	int		10h

	mov	al, 'K'
	mov	ah, 09h
	mov bh, 0
	mov	bl, 0Ah
	mov	cx, 01
	int	 10h

;//获取光标位置
	mov		ah, 03
	mov		bh, 00
	int		10h
;//重新设置光标位置
	inc		dl
	mov		ah, 02
	mov		bh, 00
	int		10h

	mov cx, OKTail_Len
    mov ax, offset strOKTail
    call DispStr
	; 跳转到内核去
	push Kernel_Seg
	push Kernel_Off
	retf


;----------------------------------------------------------------------------------------------------
;===============================================================================
; 函数名：LoadKernel
; 参数：无
; 返回：无
; 作用：
; 	找到kernel.exe并加载它
;===============================================================================
LoadKernel proc near
    mov wSectorNo, RootDir_SectorNo
SEARCH_IN_ROOT_DIR_BEGIN:
    cmp wRootDirSizeForLoop, 0   ; ┓
    jz  NO_KERNEL                ; ┣ 判断根目录区是不是已经读完
    dec wRootDirSizeForLoop      ; ┛ 如果读完表示没有找到 setup.bin
    mov ax, Kernel_Seg
    mov es, ax
    mov bx, Kernel_Off   ; 将根目录的FAT表读到Kernel_Seg:Kernel_Off处
    mov ax, wSectorNo     ; 根目录中的某 Sector 号-->ax
    mov cl, 1
    call    ReadSector

    mov si, offset strFileName   ; 让ds:si指向FileName
    mov di, Kernel_Off
    cld
    mov dx, 10h
SEARCH_KERNEL_FILE:
    cmp dx, 0                               ; ┓循环次数控制,
    jz  GOTO_NEXT_SECTOR_IN_ROOT_DIR        ; ┣如果已经读完了一个 Sector,
    dec dx                                  ; ┛就跳到下一个 Sector
    mov cx, 11 ; 我们需要比较11个字符，因为FAT12文件系统中的文件名有一个“8.3”规定，也就是8字符的文件名加3个字符的扩展名
CMP_FILENAME:
    cmp cx, 0
    jz  FILENAME_FOUND    ; 如果比较了11个字符全都相等, 表示找到了。
    dec cx
    lodsb
    cmp al, es:[di]
    jz  CMP_NEXT_CHAR   ; 匹配下一个字符
    jmp DIFFERENT       ; 只要发现一个不匹配的字符就表明本 DirectoryEntry 不是我们要找的
CMP_NEXT_CHAR:
    inc di
    jmp CMP_FILENAME    ; 继续循环，比较下一个字符

DIFFERENT:
    and di, 0FFE0h                  ; else ┓    di &= E0 为了让它指向本条目开头
    add di, 20h                     ;      ┃
    mov si, offset strFileName           ;      ┣ di += 20h  下一个目录条目
    jmp SEARCH_KERNEL_FILE         ;      ┛

GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    inc wSectorNo
    jmp SEARCH_IN_ROOT_DIR_BEGIN

NO_KERNEL:
	mov ax, offset strNoKernel
	mov cx, NoKernel_Len
    call    DispStr
HALT:
    hlt
    jmp HALT            ; 没有找到文件，死循环在这里

FILENAME_FOUND:           ; 找到文件后便来到这里继续
    mov ax, RootDir_Sectors_Count
    and di, 0FFE0h      ; di -> 当前条目的开始
    add di, 01Ah        ; di -> 首 Sector
    mov cx, es:[di]
    push cx             ; 保存此 Sector 在 FAT 中的序号
    add cx, ax
    add cx, Delta_SectorNo   ; 这句完成时 cl 里面变成 kernel.exe 的起始扇区号 (从 0 开始数的序号)
    mov ax, Kernel_Seg
    mov es, ax              ; es <- Kernel_Seg
    mov bx, Kernel_Off   ; bx <- Kernel_Off   于是, es:bx = Kernel_Seg:Kernel_Off
    mov ax, cx              ; ax <- Sector 号

GO_ON_LOADING_FILE:

    mov cl, 1
    call ReadSector
    pop ax              ; 取出此 Sector 在 FAT 中的序号
    call GetFATEntry
    cmp ax, 0FFFh       ; 是否已经加载完成
    jz  FILE_LOADED
    push ax             ; 保存 Sector 在 FAT 中的序号
    mov dx, RootDir_Sectors_Count
    add ax, dx
    add ax, Delta_SectorNo
    add bx, Byts_Per_Sector
    jmp GO_ON_LOADING_FILE
FILE_LOADED:
	ret
LoadKernel		endp
;----------------------------------------------------------------------------------------------------
;===============================================================================
; 函数名：DispStr
; 参数：
;	cx 字符串的长度
;	ax 字符串偏移地址
; 返回：无
; 作用：
; 	显示一个字符串
;===============================================================================
DispStr		proc	near
	push es
	push bp
	; 获取行号和列号
	push cx
	push ax
	mov ah, 03h
	mov bh, 00h
	int 10h
	pop ax
	pop cx
	; 输出字符串
    mov bp, ax          ; ┓
    mov ax, ds          ; ┣ ES:BP = 串地址
    mov es, ax          ; ┛
    mov ax, 1301h
    mov bx, 0007h       ; BH = 页号，BL = 显示属性---黑底白字(07h)
    int 10h
	pop bp
	pop es
	ret
DispStr		endp

;----------------------------------------------------------------------------
; 函数名: ReadSector
;----------------------------------------------------------------------------
; 作用:
;   从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
ReadSector		proc	near
    ; -----------------------------------------------------------------------
    ; 怎样由扇区号求扇区在磁盘中的位置 (扇区号 -> 柱面号, 起始扇区, 磁头号)
    ; -----------------------------------------------------------------------
    ; 设扇区号为 x
    ;                           ┌ 柱面号 = y >> 1
    ;       x           ┌ 商 y ┤
    ; -------------- => ┤      └ 磁头号 = y & 1
    ;  每磁道扇区数     │
    ;                   └ 余 z => 起始扇区号 = z + 1
    push bp
    mov bp, sp
    sub esp, 2          ; 辟出两个字节的堆栈区域保存要读的扇区数: byte [bp-2]

    mov [bp-2], cl
    push bx
    mov bl, Sector_Per_Track ; bl: 除数
    div bl          ; y 在 al 中, z 在 ah 中
    inc ah          ; z ++
    mov cl, ah          ; cl = 起始扇区号
    mov dh, al          ; dh = y
    shr al, 1           ; y >> 1 (其实是 y/BPB_NumHeads, 这里BPB_NumHeads=2)
    mov ch, al          ; ch = 柱面号
    and dh, 1           ; dh & 1 = 磁头号
    pop bx
    ; 至此, "柱面号, 起始扇区, 磁头号" 全部得到了
    mov dl, Drive_NO     ; 驱动器号 (0 表示 A 盘)
GoOnReading:
    mov ah, 2
    mov al, [bp-2]     ; 读al个扇区
    int 13h
    jc  GoOnReading        ; 如果读取错误 CF 会被置为 1, 这时就不停地读, 直到正确为止

    add esp, 2
    pop bp

    ret
ReadSector		endp
;----------------------------------------------------------------------------
; 函数名: GetFATEntry
;----------------------------------------------------------------------------
; 作用:
;   找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;   需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry		proc	near
    push es
    push bx
    push ax
    mov ax, Kernel_Seg ; ┓
    sub ax, 0100h       ; ┣ 在 Kernel_Seg 后面留出 4K 空间用于存放 FAT
    mov es, ax          ; ┛
    pop ax
    mov bOdd, 0
    mov bx, 3
    mul bx          ; dx:ax = ax * 3
    mov bx, 2
    div bx          ; dx:ax / 2  ==>  ax <- 商, dx <- 余数
    cmp dx, 0
    jz  OOOO
    mov bOdd, 1
OOOO:
;偶数
    xor dx, dx          ; 现在 ax 中是 FATEntry 在 FAT 中的偏移量. 下面来计算 FATEntry 在哪个扇区中(FAT占用不止一个扇区)
    mov bx, Byts_Per_Sector
    div bx          ; dx:ax / Byts_Per_Sector  ==>   ax <- 商   (FATEntry 所在的扇区相对于 FAT 来说的扇区号)
                    ;               dx <- 余数 (FATEntry 在扇区内的偏移)。
    push dx
    mov bx, 0           ; bx <- 0   于是, es:bx = (BaseOfSetup - 100):00 = (BaseOfSetup - 100) * 10h
    add ax, FAT1_SectorsNo  ; 此句执行之后的 ax 就是 FATEntry 所在的扇区号
    mov cl, 2
    call    ReadSector      ; 读取 FATEntry 所在的扇区, 一次读两个, 避免在边界发生错误, 因为一个 FATEntry 可能跨越两个扇区
    pop dx
    add bx, dx
    mov ax, es:[bx]
    cmp bOdd, 1
    jnz EVEN_2
    shr ax, 4
EVEN_2:
    and ax, 0FFFh

GET_FAT_ENRY_OK:

    pop bx
    pop es
    ret
GetFATEntry		endp
;----------------------------------------------------------------------------------------------------
end	main