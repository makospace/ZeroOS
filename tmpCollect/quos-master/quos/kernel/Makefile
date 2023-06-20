path = ..\..\build\bin;c:\windows\system32;
include=
lib= ..\..\build\LIB;

CC = cl
CFLAGS = /nologo /c /Gs /G3 /w /Gx- /DDEBUG
ASM = ml
AFLAGS = /nologo /c /w
LD = link
LDFLAGS = /TINY /nologo

KERNEL = kernel.exe
IMA = ..\QUOS.IMA
OBJS = kernel.obj kMain.obj string.obj mem.obj int86.obj portio.obj printf.obj scanf.obj char.obj convert.obj keyboard.obj time.obj memManage.obj sysInt.obj math.obj fat12.obj disk.obj floppy.obj screen.obj process.obj file.obj panic.obj fat12_path.obj
LDOBJS = kernel.obj+kMain.obj+string.obj+mem.obj+int86.obj+portio.obj+printf.obj+scanf.obj+char.obj+convert.obj+keyboard.obj+time.obj+memManage.obj+sysInt.obj+math.obj+fat12.obj+disk.obj+floppy.obj+screen.obj+process.obj+file.obj+panic.obj+fat12_path.obj

everything :$(KERNEL)
all : clean everything

clean : 
	@del /Q  *.map
	@del /Q  *.obj
	@del /Q  *.exe

kernel.obj: kernel.asm
	$(ASM) $(AFLAGS) $@ %s
kMain.obj: kMain.c include/kMain.h ../include/quos.h include/stdc.h include/memManage.h include/sysInt.h
	$(CC) $(CFLAGS) $@ %s
$(KERNEL): $(OBJS)
	$(LD) /nologo $(LDOBJS), $@, , , , 
	@..\..\tools\build.exe $(KERNEL) kernel.exe 28672
	@fatx -f:$(IMA) -i:$(KERNEL) -force -silence

string.obj: string.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
mem.obj: mem.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
int86.obj: int86.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
portio.obj: portio.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
printf.obj: printf.c include/stdc.h include/stdarg.h
	$(CC) $(CFLAGS) $@ %s
scanf.obj: scanf.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
char.obj: char.c include/stdc.h include/keyboard.h
	$(CC) $(CFLAGS) $@ %s
convert.obj: convert.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
math.obj: math.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s

keyboard.obj: keyboard.c include/keyboard.h
	$(CC) $(CFLAGS) $@ %s
screen.obj: screen.c include/screen.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s
time.obj: time.c include/time.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s
disk.obj: disk.c include/disk.h include/stdc.h include/errorNO.h
	$(CC) $(CFLAGS) $@ %s
floppy.obj: floppy.c include/floppy.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s

memManage.obj: memManage.c include/memManage.h include/errorNO.h ../include/quos.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s
sysInt.obj: sysInt.c include/sysInt.h include/errorNO.h include/memManage.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s
fat12.obj: fat12.c include/fat12.h include/floppy.h include/disk.h include/file.h include/stdc.h include/fat12_path.h include/time.h include/errorNO.h
	$(CC) $(CFLAGS) $@ %s
fat12_path.obj: fat12_path.c include/fat12.h include/floppy.h include/disk.h include/file.h include/stdc.h include/fat12_path.h
	$(CC) $(CFLAGS) $@ %s
file.obj: file.c include/file.h include/process.h include/globals.h  include/fat12.h
	$(CC) $(CFLAGS) $@ %s
process.obj: process.c include/process.h
	$(CC) $(CFLAGS) $@ %s


panic.obj: panic.c include/panic.h
	$(CC) $(CFLAGS) $@ %s