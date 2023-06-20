#include <stdio.h>
#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long
void outArgv(char * strProgrameName)
{
	printf("argv:\t%s bootFile setupFile\n", strProgrameName);
}
int main(int argc, char *argv[])
{
	DWORD dwFileSize = 0;
    BYTE *pFile = 0, *p = 0;
    WORD wSize = 0;
	FILE *fileBoot = 0, *fileSetup = 0;
	if(argc <= 2)
	{
		printf("Error[0]: Not enough argvs!\n");
		outArgv(argv[0]);
		return 1;
    }
// 计算setup.bin文件的大小
    fileSetup = fopen(argv[2], "rb");
    if(fileSetup==NULL)
	{
		printf("Error[1]: Can't open setup file!\n");
		outArgv(argv[0]);
		return 1;
    }
    fseek(fileSetup, 0L, SEEK_END);
	dwFileSize = ftell(fileSetup);
    fseek(fileSetup, 0L, SEEK_SET);
	wSize = dwFileSize/1024 + 1;
	fclose(fileSetup);

// 读入boot.bin
    fileBoot = fopen(argv[1], "rb");
    if(fileBoot == NULL)
	{
		printf("Error[2]: Can't open boot file!\n");
		outArgv(argv[0]);
		return 1;
    }
    fseek(fileBoot, 0L, SEEK_END);
	dwFileSize = ftell(fileBoot);
    fseek(fileBoot, 0L, SEEK_SET);
	p = pFile = (BYTE *)malloc(dwFileSize);
	if(pFile == NULL)
	{
		printf("Error[3]: Not enough memory!\n");
		return 1;
	}
	fread(pFile, dwFileSize, 1, fileBoot);
	fclose(fileBoot);
// 修改boot.bin
	p[0x3e] = wSize;

// 保存修改后的boot.bin
    fileBoot = fopen(argv[1], "wb+");
    if(fileBoot == NULL)
	{
		printf("Error[4]: Can't open boot file!\n");
		outArgv(argv[0]);
		return 1;
    }
	printf("rework boot.bin[0x3e]:%x\n", wSize);
    fseek(fileSetup, 0L, SEEK_SET);
	fwrite(pFile, dwFileSize, 1, fileBoot);
	fclose(fileBoot);
	return 0;
}