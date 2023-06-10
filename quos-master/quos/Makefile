everything :	
	@cd kernel
	@nmake /nologo
	@cd ..\boot
	@nmake /nologo 
	@cd ..\shell
	@nmake /nologo 
	@cd ..

all : clean everything

clean :
	@cd kernel
	@nmake /nologo clean
	@cd ..\boot
	@nmake /nologo clean
	@cd ..\shell
	@nmake /nologo clean
	@cd ..