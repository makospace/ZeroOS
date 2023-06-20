
@echo off
@echo Building...
echo ------------------------------------------------------------------------

set path=build\bin;c:\masm\bin;c:\windows\system32;
set include=c:\vc1.5\include;c:\masm\include;
set lib=c:\vc1.5\lib;c:\masm\lib;
nmake /nologo
if errorlevel 1 goto err
:OK
echo ------------------------------------------------------------------------
echo Build OK!
goto end
:err
echo ------------------------------------------------------------------------
echo Build Failed!!!!!
cmd /K
exit /b 1
:end
exit /b 0
@echo on 
