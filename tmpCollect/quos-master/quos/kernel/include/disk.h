#ifndef DISK_H
#define DISK_H
	typedef struct
	{
		BYTE  bySize;			// 数据包尺寸(16字节)	
		BYTE  byReserved;		// ==0
		WORD  wBlockNum;		// 要传输的数据块个数(以扇区为单位)
		DWORD dwBufferAddr;		// 传输缓冲地址(segment:offset)
		DWORD dwBeginBlockLo;	// 磁盘起始绝对块地址
		DWORD dwBeginBlockHi;	
	} DISK_ADDR_PACKET;

WORD diskReadSector(DWORD dwSector, WORD wCount, BYTE bDriveNO, BYTE far *pBuffer);//读硬盘扇区
WORD diskWriteSector(DWORD dwSector, WORD wCount, BYTE bDriveNO, BYTE far *pBuffer);//写硬盘扇区
WORD Int13Disk(BYTE bKey, DWORD dwLBA, WORD wCount, BYTE bDriveNO, void far *pBuf);//调用BIOS中断读写硬盘扇区
#endif //ifdef  DISK_H