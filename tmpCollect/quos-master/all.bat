@echo off

set path=%path%;build\bin;
set include=build\include;
set lib=build\lib;
nmake /nologo all
if errorlevel 1 goto err
:OK
echo ------------------------------------------------------------------------
echo All OK!
goto end
:err
echo ------------------------------------------------------------------------
echo All Failed!!!!!
cmd /K
exit /b 1
:end
exit /b 0
@echo on 
