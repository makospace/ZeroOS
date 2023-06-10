//文件名：file.c
//功能：
//	文件操作相关的函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- file.obj file.c
//  如斯
//2007-6-16
#include "include/file.h"
#include "include/process.h"
#include "include/globals.h"
#include "include/panic.h"
#include "include/stdc.h"
#include "include/fat12.h"
#include "include/errorNO.h"
#include "include/fat12_path.h"
#include "include/screen.h"
// 从当前进程中分配一个文件号
BYTE allocFileNo(File far * pFile)
{
	struct fileNO far * pFileNo = (struct fileNO far *)g_Process[g_byCurrProcessID].file;
	int i=1;
	for(i=1; i<MAX_FILE; i++)
	{
		if(pFileNo[i].NO==0)
		{
			pFileNo[i].NO = 1;
			pFileNo[i].fileStruct = pFile;
			return i;
		}
	}
	return 0;
}
void freeFileNo(BYTE byFileNO)
{
	struct fileNO far * pFileNo = (struct fileNO far *)g_Process[g_byCurrProcessID].file;
	/*
	printf("byFileNO=%d\n", byFileNO);
	getch();
	*/
	if(pFileNo[byFileNO].NO)
	{
		pFileNo[byFileNO].NO = 0;
		pFileNo[byFileNO].fileStruct = 0;
	}
	else// BUG,释放空闲的文件号
	{
		bug(BUG_file_freeFileNo_1);
	}
}
File far * getFileStruct(BYTE byFileNO)
{
	struct fileNO far * pFileNo = (struct fileNO far *)g_Process[g_byCurrProcessID].file;
//	printf("getFileStruct:byFileNO=%d\n", byFileNO);
	if(pFileNo[byFileNO].NO)
		return pFileNo[byFileNO].fileStruct;
	else// BUG
	{
		bug(BUG_file_getFileStruct_1);
	}
	return 0;
}
FILE fopen(char far * strFileName, char far * strMode)
{
	// 解析strMode
	File far * pFile;// 现在仅仅支持读，所以没有解析用户传入的读写类型
	FILE fileNo = 0;
	pFile = fat12_fOpen(strTOSuper(strFileName), FILE_READ);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfopen: Error when open file %s  %p\n", strFileName, pFile);
		#endif
		goto error;
	}
	fileNo = allocFileNo(pFile);
	if(!fileNo)
	{
		#ifdef DEBUG_FILE
		puts("\tfopen: Error when alloc file NO\n");
		#endif
		goto error;
	}
//	printf("fopen:strFileName=%s, file NO=%d\n", strFileName, fileNo);	
//	getch();
	return fileNo;
error:
	return 0;
}
DWORD fread(BYTE far * pDest, DWORD dwSize, DWORD dwCount, FILE fileNo)
{
	File far * pFile = getFileStruct(fileNo);
//	printf("fread:fileNo=%d\n", fileNo);
//	getch();
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfread: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	return pFile->pOpts->Read(pDest, dwSize, dwCount, pFile);
error:
	return 0;
}
void fclose(FILE fileNo)
{
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfclose: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	freeFileNo(fileNo);
	pFile->pOpts->Close(pFile);
error:
	return;
}
DWORD fseek(FILE fileNo, DWORD dwOffset, BYTE by0rigin)
{
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfseek: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		return 0;
	}
	return pFile->pOpts->Seek(pFile, dwOffset, by0rigin);
}

DWORD fwrite(const BYTE far * pSrc, DWORD dwSize, DWORD dwCount, FILE fileNo)
{	
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfread: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	return pFile->pOpts->Write(pSrc, dwSize, dwCount, pFile);
error:
	return 0;
}

BOOL fattribute(BYTE byAttribute, FILE fileNo)
{
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfread: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	return pFile->pOpts->Attribute(pFile, byAttribute);
error:
	return 0;
}
BOOL frename(char far * strName, FILE fileNo)
{
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfread: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	return pFile->pOpts->Rename(strName, pFile);
error:
	return 0;
}
BOOL fdelete(FILE fileNo)
{
	File far * pFile = getFileStruct(fileNo);
	if(!pFile)
	{
		#ifdef DEBUG_FILE
		printf("\tfread: Error when get file struct which file NO is %d!\n", fileNo);
		#endif
		goto error;
	}
	return pFile->pOpts->Delete(pFile);
error:
	return 0;
}
void fls(const char far * path)
{
	BYTE Buf[512];
	int i;
	struct FAT12DirEntry far * ret;
	setScreenColor(COLOR_GREEN);
	puts("\tname\text\t\tsize\n");
	getComebackScreenColor();
	puts("----------------------------------\n");
#define MAX_COUNT 64
	for(i=0; i<MAX_COUNT; i++)
	{
		int j;
		ret = fat12_GetDirSectorNO(path, i, (BYTE far *)Buf);
		if(!ret)break;
		
		for(j=0; j<8; j++)
		{
			if(*(char far *)ret != 0)
			{
				DWORD dwSize = ret->dwFileSize;
				char strFileName[FILE_NAME_SIZE+FILE_EXT_SIZE+1];
				if(*(BYTE far *)ret == 0xe5)
				{
					ret++;
					continue;
				}
				memcpy((char far *)strFileName, (char far *)ret->strFileName, FILE_NAME_SIZE+FILE_EXT_SIZE);
				strFileName[FILE_NAME_SIZE+FILE_EXT_SIZE] = 0;
				printf("\t%s\t\t", strTOLower((char far *)strFileName));
				if(dwSize/1024 <= 0)
				{
					printf("%ld byte\n" , dwSize);
				}
				else if(dwSize/1024/1024 <= 0)
				{
					BYTE i = (dwSize%1024 * 10)/1024;// 精确到一位小数
					printf("%ld.%d KB\n" , dwSize/1024, i);
				}
				else
				{
					BYTE i = (dwSize%1024 * 10)/1024;
					printf("%ld.%d MB\n" , dwSize/1024/1024, i);
				}
				ret++;
			}
			else
			{
				break;
			}
		}
	}
}