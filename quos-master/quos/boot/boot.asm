; 文件名：boot.asm
; 功能：
;	从软盘根目录找到setp.bin文件，加载并运行它
; 编译：nasmw boot.asm -o boot.bin
; 参考《自己动手写操作系统》完成
;   如斯 
; 2007-5-28

                    org  07c00h ; Bios会在开机的时候把booter加载到0x7C00处并运行它
 
;================================================================================================
; 一些常量
; ----------------------------------------------------------------------
BaseOfStack         equ 07c00h  ; 堆栈基地址。这个地址是栈底,堆栈从这个位置向低地址生长，因为我们的booter在0x7c00的高地址处512个字节，所以不会有问题

;segOfSetup         equ  09000h ; setup.bin 被加载到内存后所在位置的段地址
OffsetOfSetup       equ  00h    ; setup.bin 被加载到内存后所在位置的偏移地址

; -------------------------------------------------------------------------
; 基于 FAT12 头的一些常量定义，如果头信息改变，下面的常量可能也要做相应改变
; -------------------------------------------------------------------------
FATSz               equ 9   ; BPB_FATSz16
RootDirSectors      equ 14  ; 根目录占用空间: RootDirSectors = ((BPB_RootEntCnt * 32) + (BPB_BytsPerSec – 1)) / BPB_BytsPerSec; 但如果按照此公式代码过长
SectorNoOfRootDirectory equ 19  ; Root Directory 的第一个扇区号 = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz)
SectorNoOfFAT1      equ 1   ; FAT1 的第一个扇区号   = BPB_RsvdSecCnt
DeltaSectorNo       equ 17  ; DeltaSectorNo = BPB_RsvdSecCnt + (BPB_NumFATs * FATSz) - 2
                            ; 文件的开始Sector号 = DirEntry中的开始Sector号 + 根目录占用Sector数目 + DeltaSectorNo
;================================================================================================
    jmp short START
    nop             ; 为了满足fat文件系统的偏移要求，这个nop必不可少
 
; FAT12文件系统标识头
; ----------------------------------------------------------------------
    BS_OEMName      DB 'Allan   '   ; OEM String, 必须 8 个字节

    BPB_BytsPerSec  DW 512      ; 每扇区字节数
    BPB_SecPerClus  DB 1        ; 每簇多少扇区
    BPB_RsvdSecCnt  DW 1        ; Boot 记录占用多少扇区
    BPB_NumFATs     DB 2        ; 共有多少 FAT 表
    BPB_RootEntCnt  DW 224      ; 根目录文件数最大值
    BPB_TotSec16    DW 2880     ; 逻辑扇区总数
    BPB_Media       DB 0xF0     ; 媒体描述符
    BPB_FATSz16     DW 9        ; 每FAT扇区数
    BPB_SecPerTrk   DW 18       ; 每磁道扇区数
    BPB_NumHeads    DW 2        ; 磁头数(面数)
    BPB_HiddSec     DD 0        ; 隐藏扇区数
    BPB_TotSec32    DD 0        ; 大扇区数（大于等于256*256）

    BS_DrvNum       DB 0        ; 中断 13 的驱动器号
    BS_Reserved1    DB 0        ; 未使用
    BS_BootSig      DB 29h      ; 扩展引导标记 (29h)
    BS_VolID        DD 0        ; 卷序列号
    BS_VolLab       DB 'QUOS1.00   '; 卷标, 必须 11 个字节
    BS_FileSysType  DB 'FAT12   '   ; 文件系统类型, 必须 8个字节  
;------------------------------------------------------------------------
sizeOfSetup         DB  01h
segOfSetup			DW  00
START:
    mov ax, cs
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, BaseOfStack
	; 计算setup即将被加载的位置
	int 12h
	inc ax
	sub ax, [sizeOfSetup]
	sub ax, 30h ; 为了加载根目录至少需要这些内存
	mov bx, 40h
	mul bx
	mov word [segOfSetup], ax
    ; 初始化屏幕为：80 x 25  16色文本
    mov ax, 03h
    int 10h

    mov dh, 0
    call DispStr; 显示字符串"Booting  "
    
    xor ah, ah  ; ┓
    xor dl, dl  ; ┣ 软驱复位
    int 13h     ; ┛
    
; 在根目录寻找 setup.bin，找到后加载它并跳转过去运行，如果找不到就会给出提示信息然后执行一个死循环
    mov word [wSectorNo], SectorNoOfRootDirectory
SEARCH_IN_ROOT_DIR_BEGIN:
    cmp word [wRootDirSizeForLoop], 0   ; ┓
    jz  NO_setupBIN                     ; ┣ 判断根目录区是不是已经读完
    dec word [wRootDirSizeForLoop]      ; ┛ 如果读完表示没有找到 setup.bin
    mov ax, [segOfSetup]
    mov es, ax
    mov bx, OffsetOfSetup   ; es:bx = segOfSetup:OffsetOfSetup
    mov ax, [wSectorNo]     ; 根目录中的某 Sector 号-->ax
    mov cl, 1
    call    ReadSector

    mov si, SetupFileName   ; 让ds:si指向SetupFileName
    mov di, OffsetOfSetup   ; 让es:di指向segOfSetup:0100 = segOfSetup*10h+100
    cld
    mov dx, 10h
SEARCH_FOR_setupBIN:
    cmp dx, 0                               ; ┓循环次数控制,
    jz  GOTO_NEXT_SECTOR_IN_ROOT_DIR        ; ┣如果已经读完了一个 Sector,
    dec dx                                  ; ┛就跳到下一个 Sector
    mov cx, 11 ; 我们需要比较11个字符，因为FAT12文件系统中的文件名有一个“8.3”规定，也就是8字符的文件名加3个字符的扩展名
CMP_FILENAME:
    cmp cx, 0
    jz  FILENAME_FOUND    ; 如果比较了11个字符全都相等, 表示找到了。
    dec cx
    lodsb
    cmp al, byte [es:di]
    jz  CMP_NEXT_CHAR   ; 匹配下一个字符
    jmp DIFFERENT       ; 只要发现一个不匹配的字符就表明本 DirectoryEntry 不是我们要找的
CMP_NEXT_CHAR:
    inc di
    jmp CMP_FILENAME    ; 继续循环，比较下一个字符

DIFFERENT:
    and di, 0FFE0h                  ; else ┓    di &= E0 为了让它指向本条目开头
    add di, 20h                     ;      ┃
    mov si, SetupFileName           ;      ┣ di += 20h  下一个目录条目
    jmp SEARCH_FOR_setupBIN         ;      ┛

GOTO_NEXT_SECTOR_IN_ROOT_DIR:
    add word [wSectorNo], 1
    jmp SEARCH_IN_ROOT_DIR_BEGIN

NO_setupBIN:
    mov dh, 2
    call    DispStr     ; 显示字符串"No SETUP."
HALT:
    hlt
    jmp HALT            ; 没有找到setup.bin，死循环在这里

FILENAME_FOUND:           ; 找到setup.bin后便来到这里继续
    mov ax, RootDirSectors
    and di, 0FFE0h      ; di -> 当前条目的开始
    add di, 01Ah        ; di -> 首 Sector
    mov cx, word [es:di]
    push cx             ; 保存此 Sector 在 FAT 中的序号
    add cx, ax
    add cx, DeltaSectorNo   ; 这句完成时 cl 里面变成 setup.bin 的起始扇区号 (从 0 开始数的序号)
    mov ax, [segOfSetup]
    mov es, ax              ; es <- segOfSetup
    mov bx, OffsetOfSetup   ; bx <- OffsetOfSetup   于是, es:bx = segOfSetup:OffsetOfSetup = segOfSetup * 10h + OffsetOfSetup
    mov ax, cx              ; ax <- Sector 号

GO_ON_LOADING_FILE:
    ;push    ax          ; 每次读一个扇区前输出一个“.”
    ;push    bx 
    ;mov ah, 0Eh
    ;mov al, '.'
    ;mov bl, 0Fh
    ;int 10h
    ;pop bx
    ;pop ax

    mov cl, 1
    call ReadSector
    pop ax              ; 取出此 Sector 在 FAT 中的序号
    call GetFATEntry
    cmp ax, 0FFFh       ; 是否已经加载完成
    jz  FILE_LOADED
    push ax             ; 保存 Sector 在 FAT 中的序号
    mov dx, RootDirSectors
    add ax, dx
    add ax, DeltaSectorNo
    add bx, [BPB_BytsPerSec]
    jmp GO_ON_LOADING_FILE

FILE_LOADED:
    mov dh, 1
    call DispStr     ; 显示字符串"Ready."

; 引导工作完成，开始跳转到setup.bin执行
	; 
    ;jmp es:OffsetOfSetup
mov es,[segOfSetup]
	push es
	push 0
	retf
;============================================================================
; 变量
;----------------------------------------------------------------------------
wRootDirSizeForLoop dw  RootDirSectors  ; Root Directory 占用的扇区数, 在循环中会递减至零.
wSectorNo           dw  0               ; 要读取的扇区号
bOdd                db  0               ; 奇数还是偶数
SetupFileName       db  "SETUP   BIN", 0; setup.bin 之文件名
MessageLength       equ 9               ; 每条信息9字节, 不够则用空格补齐
BootMessage:        db  "Booting  "     ; 序号 0，引导提示信息
Message1            db  "Ready. ", 0dh , 0ah; 序号 1，加载setup.bin完成信息
Message2            db  "No SETUP."         ; 序号 2，没有找到setup.bin提示信息
;============================================================================


;----------------------------------------------------------------------------
; 函数名: DispStr
;----------------------------------------------------------------------------
; 作用:
;   显示一个字符串, 函数开始时 dh 中应该是字符串序号(0-based)
DispStr:
    mov ax, MessageLength
    mul dh
    add ax, BootMessage
    mov bp, ax          ; ┓
    mov ax, ds          ; ┣ ES:BP = 串地址
    mov es, ax          ; ┛
    mov cx, MessageLength; CX = 字符串的长度
    mov ax, 01301h
    mov bx, 0007h       ; BH = 页号，BL = 显示属性---黑底白字(07h)
    mov dl, 0
    int 10h
    ret


;----------------------------------------------------------------------------
; 函数名: ReadSector
;----------------------------------------------------------------------------
; 作用:
;   从第 ax 个 Sector 开始, 将 cl 个 Sector 读入 es:bx 中
ReadSector:
    ; -----------------------------------------------------------------------
    ; 怎样由扇区号求扇区在磁盘中的位置 (扇区号 -> 柱面号, 起始扇区, 磁头号)
    ; -----------------------------------------------------------------------
    ; 设扇区号为 x
    ;                           ┌ 柱面号 = y >> 1
    ;       x           ┌ 商 y ┤
    ; -------------- => ┤      └ 磁头号 = y & 1
    ;  每磁道扇区数     │
    ;                   └ 余 z => 起始扇区号 = z + 1
    push    bp
    mov bp, sp
    sub esp, 2          ; 辟出两个字节的堆栈区域保存要读的扇区数: byte [bp-2]

    mov byte [bp-2], cl
    push bx
    mov bl, [BPB_SecPerTrk] ; bl: 除数
    div bl          ; y 在 al 中, z 在 ah 中
    inc ah          ; z ++
    mov cl, ah          ; cl = 起始扇区号
    mov dh, al          ; dh = y
    shr al, 1           ; y >> 1 (其实是 y/BPB_NumHeads, 这里BPB_NumHeads=2)
    mov ch, al          ; ch = 柱面号
    and dh, 1           ; dh & 1 = 磁头号
    pop bx
    ; 至此, "柱面号, 起始扇区, 磁头号" 全部得到了
    mov dl, [BS_DrvNum]     ; 驱动器号 (0 表示 A 盘)
.GoOnReading:
    mov ah, 2
    mov al, byte [bp-2]     ; 读al个扇区
    int 13h
    jc  .GoOnReading        ; 如果读取错误 CF 会被置为 1, 这时就不停地读, 直到正确为止

    add esp, 2
    pop bp

    ret

;----------------------------------------------------------------------------
; 函数名: GetFATEntry
;----------------------------------------------------------------------------
; 作用:
;   找到序号为 ax 的 Sector 在 FAT 中的条目, 结果放在 ax 中
;   需要注意的是, 中间需要读 FAT 的扇区到 es:bx 处, 所以函数一开始保存了 es 和 bx
GetFATEntry:
    push    es
    push    bx
    push    ax
    mov ax, [segOfSetup] ; ┓
    sub ax, 0100h       ; ┣ 在 segOfSetup 后面留出 4K 空间用于存放 FAT
    mov es, ax          ; ┛
    pop ax
    mov byte [bOdd], 0
    mov bx, 3
    mul bx          ; dx:ax = ax * 3
    mov bx, 2
    div bx          ; dx:ax / 2  ==>  ax <- 商, dx <- 余数
    cmp dx, 0
    jz  EVEN
    mov byte [bOdd], 1
EVEN:;偶数
    xor dx, dx          ; 现在 ax 中是 FATEntry 在 FAT 中的偏移量. 下面来计算 FATEntry 在哪个扇区中(FAT占用不止一个扇区)
    mov bx, [BPB_BytsPerSec]
    div bx          ; dx:ax / BPB_BytsPerSec  ==>   ax <- 商   (FATEntry 所在的扇区相对于 FAT 来说的扇区号)
                    ;               dx <- 余数 (FATEntry 在扇区内的偏移)。
    push    dx
    mov bx, 0           ; bx <- 0   于是, es:bx = (segOfSetup - 100):00 = (segOfSetup - 100) * 10h
    add ax, SectorNoOfFAT1  ; 此句执行之后的 ax 就是 FATEntry 所在的扇区号
    mov cl, 2
    call    ReadSector      ; 读取 FATEntry 所在的扇区, 一次读两个, 避免在边界发生错误, 因为一个 FATEntry 可能跨越两个扇区
    pop dx
    add bx, dx
    mov ax, [es:bx]
    cmp byte [bOdd], 1
    jnz EVEN_2
    shr ax, 4
EVEN_2:
    and ax, 0FFFh

GET_FAT_ENRY_OK:

    pop bx
    pop es
    ret
;----------------------------------------------------------------------------

times   510-($-$$)  db  0   ; 填充剩下的空间，使生成的二进制代码恰好为512字节
dw  0xaa55              ; 结束标志
