//文件名：kMain.c
//功能：
//	内核主函数文件，进行各种初始化操作
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- kMain.obj kMain.c
//  如斯
//2007-6-5
#include "../include/quos.h"
#include "include/kMain.h"
#include "include/stdc.h"
#include "include/time.h"
#include "include/memManage.h"
#include "include/sysInt.h"
#include "include/floppy.h"
#include "include/process.h"
#include "include/globals.h"
#include "include/file.h"
#include "include/fat12.h"
void init()
{
	WORD far * pCursor = lineToFar(0x450);
	BYTE byTemp;
	union REGS r;

	// 初始化屏幕
	screenInit(0x03);

	// 输出一些信息
	puts("Welcome to QUOS! The author is ");
	byTemp = g_byCurrColor;
	g_byCurrColor = COLOR_GREEN;
	puts("Allan");
	g_byCurrColor = byTemp;
	puts(".\n\n");

	// 进程管理初始化
	processManageInit();

	// 初始化内存管理
	memInit();

	// 初始化文件系统
	fat12_Init(FLOPPY_A);
}
void test()
{
	#define STR "this is kernel\tinfo\n"
//	char string[];
	char far * string = (char far *)malloc(123);
	char far * string1, far * string2, far * string3, far * string4;
	strcpy(string, STR);
	printf("%p  %s\n", string, string);
	free(string);
//	puts("\nsystem time:\t");
//	cmosDate();
//	puts(" ");
//	cmosTime();
//	puts("\n");

	string1 =  (char far *)malloc(200);
	strcpy(string1, "test1");
	printf("%p  %s\n", string1, string1);
	free(string1);

	string2 =  (char far *)malloc(0x200);
	strcpy(string2, "test2");
	printf("%p  %s\n", string2, string2);
	free(string2);

	string3 =  (char far *)malloc(40);
	strcpy(string3, "test3");
	printf("%p  %s\n", string3, string3);
	free(string3);

	string4 =  (char far *)malloc(23);
	strcpy(string4, "test4");
	printf("%p  %s\n", string4, string4);
	free(string4);

	while(1)
	{
		BYTE byTemp;
		char far *buf =  (char far *)malloc(256);
		puts("Now, you can input a string:");
		byTemp = g_byCurrColor;
		g_byCurrColor = COLOR_BLUE;
		gets(buf);
		g_byCurrColor = byTemp;
		printf("The string is :%s\n", buf);
		free(buf);
	}
}
void test2()
{
	BYTE far * p = 0, far *w;
	BYTE far * pAdd =  (BYTE far *)malloc(200);
	while(1)
	{
		int j = 8;
		puts("Input a memory address:0x");
		p = lineToFar(xtol(gets(pAdd)));
		w = p;
		if(strlen(pAdd)==0)break;
		while(j--)
		{
			int i = 16;
			BYTE far * temp = w;
			printf("0x%05lx\t", farToLine(w));
			while(i--)
			{
				printf("%02x ", (BYTE)*w);
				w++;
			}
			puts("; ");
			for(i=16; i>0; i--)
			{
				if(*temp>=21 && *temp<=125)
					putchar(*temp);
				else
					putchar('.');
				temp++;
			}
			puts("\n");
		}
	}
	free(pAdd);
}
void test3(void far * pOut)
{
	BYTE far * p = pOut, far *w;
	BYTE far * pAdd =  (BYTE far *)malloc(200);
	BOOL bInputAdd = FALSE;
	while(1)
	{
		int j = 8;
		if(bInputAdd)
		{
			puts("Input a memory address:0x");
			p = lineToFar(xtol(gets(pAdd)));
			w = p;
			bInputAdd = TRUE;
			if(strlen(pAdd)==0)break;
		}
		while(j--)
		{
			int i = 16;
			BYTE far * temp = w;
			printf("0x%05lx\t", farToLine(w));
			while(i--)
			{
				printf("%02x ", (BYTE)*w);
				w++;
			}
			puts("; ");
			for(i=16; i>0; i--)
			{
				if(*temp>=21 && *temp<=125)
					putchar(*temp);
				else
					putchar('.');
				temp++;
			}
			puts("\n");
		}
		bInputAdd = TRUE;
	}
	free(pAdd);
}
void kMain()
{
	BYTE byTemp;

	// 初始化内核
	init();
	puts("Init kernel               [");
	byTemp = g_byCurrColor;
	g_byCurrColor = COLOR_GREEN;
	puts("OK");
	g_byCurrColor = byTemp;
	puts("]\n");
#ifdef DEBUG_FLOPPY
{
	BYTE far * p;
	BYTE far * pAdd =  (BYTE far *)malloc(200);	
	puts("DEBUG_FLOPPY: Input a memory address what will be write to disk:0x");
	p = lineToFar(xtol(gets(pAdd)));
	free(pAdd);
	if(strlen(pAdd)!=0)
		floppyWrite(1, 0, 15, 0, FLOPPY_A, p);
	p = malloc(0x200);
	floppyRead(1, 0, 15, 0, FLOPPY_A, p);	
	test2();// 查看内存
	free(p);
}
#endif
#ifdef DEBUG_DISK
{
	BYTE far * p;
	BYTE far * pAdd =  (BYTE far *)malloc(200);	
	puts("DEBUG_DISK: Input a memory address what will be write to disk:0x");
	p = lineToFar(xtol(gets(pAdd)));
	free(pAdd);
	if(strlen(pAdd)!=0)
		diskWriteSector(0, 5, DISK_A, p);
	p = malloc(0x200);
	diskWriteSector(0, 5, DISK_A, p);	
	test2();// 查看内存
	free(p);
}
#endif
#ifdef DEBUG_FAT12_READ
{
	char far * pBuffer = malloc(512);
	File far * pFile = fat12_fOpen("TEST.TXT", FILE_READ);
	fat12_fRead(pBuffer, 510, 1, pFile);
	fat12_fClose(pFile);
	printf("DEBUG_FAT12: fileContent:\n%s\n", pBuffer);
	free(pBuffer);
}
#endif
#ifdef DEBUG_FAT12_WRITE
{
	char far * pBuffer = malloc(1024), far * pWork;
	File far * pFile = fat12_fOpen("TEST.TXT", FILE_READ);
	WORD i, j;
	memset(pBuffer, 0, 512);
	pWork = pBuffer;
	printf("begin test write");
	for(j=0; j<600; j++)
	{
		*pWork = j+1;
		pWork = lineToFar(farToLine(pWork) + 1);
	}
	fat12_fSeek(pFile, 2, SEEK_SET);
	fat12_fWrite(pBuffer, 600, 1, pFile);
	memset(pBuffer, 0, 512);
	fat12_fSeek(pFile, 0, SEEK_SET);
	fat12_fRead(pBuffer, 600, 1, pFile);
	//printf("DEBUG_FAT12: fileContent:\n%s\n", pBuffer);
	test3(pBuffer);
	fat12_fClose(pFile);
	
	memset(pBuffer, 0, 512);
	free(pBuffer);
}
#endif
#ifdef DEBUG_FILE
{
	char far * pBuffer = malloc(1024);
	FILE FileNO = fopen("TEST.TXT", "r");
	fread(pBuffer, 1023, 1, FileNO);
	fclose(FileNO);
	printf("fileContent:\n%s\n", pBuffer);
	free(pBuffer);
}
#endif
#ifdef DEBUG_PROCESS
{
	puts("DEBUG_PROCESS: run shell.exe\n");
	getch();
	exec("SHELL.EXE", "");
	test2();// 查看内存
}
#endif
// 运行shell
while(1)	
	exec("SHELL.EXE", "");

}