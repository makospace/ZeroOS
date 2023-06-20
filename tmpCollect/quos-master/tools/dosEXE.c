#include <stdio.h>
#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned long
struct exeHead
{	
		WORD wMagic;			// 幻数
		WORD wSizeInLastBlock;	// 最后一个块使用的字节数
		WORD wBlockCount;		// 512字节块的个数
		WORD wReallocCount;		// 重定位项个数
		WORD wHeadSize;			// 文件头部大小（以16字节为单位）
		WORD wMinAlloc;			// 需额外分配的最小内存量
		WORD wMaxAlloc;			// 需额外分配的最大内存量
		WORD wInitSS;			// 初始SS
		WORD wInitSP;			// 初始SP
		WORD wChecksum;			// 文件校验和(保留不用，为0)
		WORD wInitIP;			// 初始IP
		WORD wInitCS;			// 初始CS
		WORD wFirstRealloc;		// 重定位修正表位置(第一个重定位项相对于文件的偏移值)
		WORD wNoverlay;			// 重叠的个数，程序为0
};
int main(int argc, char *argv[])
{
	FILE * exeFile = 0;
	struct exeHead * pHead = 0;
    if(argc<1)
	{
		printf("Error[0]: No enough argvs.\n");
		return 1;
    }	
    exeFile = fopen(argv[1], "rb");
    if(exeFile == NULL)
	{
		printf("Error[1]: Can't open file.\n");
		return 1;
    }
	pHead = (struct exeHead *)malloc(sizeof(struct exeHead));
	fread(pHead, sizeof(struct exeHead), 1, exeFile);
	fclose(exeFile);

// 验证幻数是否有效
	if(pHead->wMagic != 0x5A4D)
	{
		printf("Error[2]: This file isn't a dos-EXE program!");
		return 1;
	}

	printf("                 幻数:\t%#X\n", pHead->wMagic);
	printf("最后一个块使用的字节数:\t%d\n", pHead->wSizeInLastBlock);
	printf("      512字节块的个数:\t%d\n", pHead->wBlockCount);
	printf("         重定位项个数:\t%d\n", pHead->wReallocCount);
	printf("文件头部大小（以16字节为单位）:\t%d\n", pHead->wHeadSize);
	printf("需额外分配的最小内存量:\t%d\n", pHead->wMinAlloc);
	printf("需额外分配的最大内存量:\t%d\n", pHead->wMaxAlloc);
	printf("      wInitSS:wInitSP =\t%04xh:%04xh\n", pHead->wInitSS, pHead->wInitSP);
	printf(" 文件校验和(保留不用):\t%04xh\n", pHead->wChecksum);
	printf("      wInitCS:wInitIP =\t%04xh:%04xh\n", pHead->wInitIP, pHead->wInitCS);
	printf("     重定位修正表位置:\t%d\n", pHead->wFirstRealloc);
	printf("  重叠的个数(程序为0):\t%d\n", pHead->wNoverlay);
	return 0;
}