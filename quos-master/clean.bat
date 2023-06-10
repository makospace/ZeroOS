@echo off
@echo Cleaning...
echo ------------------------------------------------------------------------
nmake /nologo clean
if errorlevel 1 goto err
:OK
echo ------------------------------------------------------------------------
echo Clean OK!
goto end
:err
echo ------------------------------------------------------------------------
echo Clean Failed!!!!!
cmd /K
exit /b 1
:end
exit /b 0
@echo on 