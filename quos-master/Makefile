path = c:\windows\system32;build\bin;

QUOS = quos
TOOLS = tools

everything :
	@cd $(QUOS)
	@nmake /nologo
	@cd ..

all : clean everything

clean : 
	@cd $(QUOS)
	@nmake /nologo clean
	@cd ..
	
