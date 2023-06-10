path = C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\WINDOWS\system32\WindowsPowerShell\v1.0;D:\Microsoft Visual Studio\Common\Tools\WinNT;D:\Microsoft Visual Studio\Common\MSDev98\Bin;D:\Microsoft Visual Studio\Common\Tools;D:\Microsoft Visual Studio\VC98\bin;D:\Microsoft Visual Studio\Common\Tools\WinNT;D:\Microsoft Visual Studio\Common\MSDev98\Bin;D:\Microsoft Visual Studio\Common\Tools;D:\Microsoft Visual Studio\VC98\bin
include = D:\Microsoft Visual Studio\VC98\atl\include;D:\Microsoft Visual Studio\VC98\mfc\include;D:\Microsoft Visual Studio\VC98\include
lib = D:\Microsoft Visual Studio\VC98\mfc\lib;D:\Microsoft Visual Studio\VC98\lib
CC = @cl
CFLAGS = /nologo
#  /D__DEBUG__
everything : put.exe dosEXE.exe build.exe reworkBoot.exe
all : clean everything

clean :
	@del /Q  *.obj
	@del /Q  put.exe
	@del /Q  dosEXE.exe
	@del /Q  build.exe
	@del /Q  reworkBoot.exe

reworkBoot.exe : reworkBoot.c
	@$(CC) $(CFLAGS) %s
put.exe : put.c
	@$(CC) $(CFLAGS) %s
dosEXE.exe : dosEXE.c
	@$(CC) $(CFLAGS) %s
build.exe : build.c
	@$(CC) $(CFLAGS) %s