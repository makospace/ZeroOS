path = ..\..\build\bin;C:\windows\system32;
include=
lib=..\..\build\lib;

CC = @cl
CFLAGS = /nologo /c /Gs /G3 /w /Gx- /DDEBUG
ASM = @ml
AFLAGS = /nologo /c /w
LD = @link
LDFLAGS = /nologo
OBJS = shellHead.obj shell.obj command.obj char.obj string.obj mem.obj int86.obj portio.obj printf.obj convert.obj math.obj file.obj
LDOBJS = shellHead.obj+shell.obj+command.obj+char.obj+string.obj+mem.obj+int86.obj+portio.obj+printf.obj+convert.obj+math.obj+file.obj

SHELL = shell.exe
IMA = ..\QUOS.IMA

everything : $(OBJS) $(SHELL)
all : clean everything copy

clean : 
	@del /Q  *.map
	@del /Q  *.obj
	@del /Q  *.exe

shellHead.obj : shellHead.asm
	$(ASM) $(AFLAGS) $@ %s
shell.obj : shell.c include/shell.h include/command.h include/stdc.h
	$(CC) $(CFLAGS) $@ %s
command.obj : command.c include/shell.h include/stdc.h include/command.h ../include/quos.h
	$(CC) $(CFLAGS) $@ %s

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
char.obj: char.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
convert.obj: convert.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
math.obj: math.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s
file.obj: file.c include/stdc.h
	$(CC) $(CFLAGS) $@ %s

$(SHELL) : $(OBJS)
	$(LD) $(LDFLAGS) $(LDOBJS), $@, , , ,
	@fatx -f:$(IMA) -i:$(SHELL) -force -silence