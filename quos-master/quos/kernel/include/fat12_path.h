#ifndef FAT12_PATH_H
#define FAT12_PATH_H
	#include "include/File.h"
	WORD fat12_GetDirSector(const File far * pFile, BYTE far *pBuf, struct DWORD far * pDirEntry);
	struct FAT12DirEntry far * fat12_GetDirSectorNO(char far * path, WORD NO, BYTE far *pBuf);
#endif //ifdef  FAT12_PATH_H