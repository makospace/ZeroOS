path = ..\..\build\bin;c:\windows\system32;C:\Program Files\WinImage;
include=
lib=..\..\build\lib;

ASM = @ml
AFLAGS = /nologo /c /w /D __DEBUG__
LD = @link
LDFLAGS = /TINY /nologo

BOOT = boot.bin
SETUP = setup.bin
IMA = ..\QUOS.IMA

everything: $(SETUP) $(BOOT)
all : clean everything copy

clean : 
	@del /Q  *.map
	@del /Q  *.obj
	@del /Q  *.bin

$(BOOT): boot.asm
	@nasm -o $@ %s
	@..\..\tools\reworkBoot $(BOOT) $(SETUP)
	@..\..\tools\put $(BOOT) $(IMA) 0

setup.obj:	setup.asm
	$(ASM) $(AFLAGS) $@ %s

$(SETUP): setup.obj
	$(LD) $(LDFLAGS) %s, $@, , , ,
	@nasm -o $(BOOT) boot.asm
	@..\..\tools\reworkBoot $(BOOT) $(SETUP)
	@..\..\tools\put $(BOOT) $(IMA) 0
	@Fatx -f:$(IMA) -i:$(SETUP) -force -silence
