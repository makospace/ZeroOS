#ifndef FILE_H
#define FILE_H

	#include "include/defs.h"
	#include "fat12.h"

	#define FILE_ATTR_RDONLY	1	//只读文件
	#define FILE_ATTR_ISDIR		2	//目录文件
	#define FILE_ATTR_SYSTEM	4	//系统文件，不允许用户直接访问。
	#define FILE_ATTR_HIDDEN	8	//隐藏文件

	#define FILE_NAME_SIZE 8
	#define FILE_EXT_SIZE 3

	#define FILE_READ 0x1
	#define FILE_WRITE 0x2

	// 偏移标志
	#define SEEK_CUR 0
	#define SEEK_SET 1
	#define SEEK_END 2

	typedef unsigned short FILE;

	// FILE结构
	typedef struct File
	{
		char strName[FILE_NAME_SIZE];	// 文件名
		char strExt[FILE_EXT_SIZE];		// 扩展名
		BYTE byMode;		// 文件读写方式
		BYTE byAttribute;	// 文件属性
		DWORD dwSize;		// 文件大小
		DWORD dwPos;		// 读写位置
		BYTE byDevNO;		// 所属设备号
		WORD wTime;				// 最后一次修改时间
		WORD wDate;				// 最后一次修改日期
		WORD wFirstCus;	// 磁盘中的开始簇号
		struct FileOpts far * pOpts;	// 文件系统操作函数
		struct FAT12Parameter far * pFSPar;	// 文件系参数信息
	}File;

	// 文件操作函数
	typedef struct FileOpts{
		File far * (*Open) (char far * strFileName, BYTE byMode);
		void (*Close) (File far * pFile);
		DWORD (*Read) (BYTE far * pDest, DWORD dwSize, DWORD dwCount, File far * pFile);
		DWORD (*Seek) (File far * pFile, DWORD dwOffset, BYTE by0rigin);
		DWORD (*Write) (BYTE far * pSrc, DWORD dwSize, DWORD dwCount, File far * pFile);
		BOOL (*Rename) (char far * strName, File far * pFile);
		BOOL (*Delete) (File far * pFile);
		BOOL (*Attribute) (File far * pFile, BYTE byAttribute);
	}FileOpts;
	BYTE allocFileNo(File far * pFile);// 从当前进程中分配一个文件号
	void freeFileNo(BYTE byFileNO);// 释放当前进程的一个文件号
	File far * getFileStruct(BYTE byFileNO);
	FILE fopen(char far * strFileName, char far * strMode);
	DWORD fread(BYTE far * pDest, DWORD dwSize, DWORD dwCount, FILE fileNo);
	DWORD fseek(FILE fileNo, DWORD dwOffset, BYTE by0rigin);
	void fclose(FILE fileNo);
	DWORD fwrite(BYTE far * pSrc, DWORD dwSize, DWORD dwCount, FILE fileNo);
	void fls(const char far * path);
	BOOL fdelete(FILE fileNo);
	BOOL frename(char far * strName, FILE fileNo);
	BOOL fattribute(BYTE byAttribute, FILE fileNo);
#endif //ifdef  FILE_H