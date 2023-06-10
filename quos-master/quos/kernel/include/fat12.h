#ifndef FAT12_H
#define FAT12_H

	#define ATTR_READ_ONLY 0x01
	#define ATTR_HIDDEN 0x02
	#define ATTR_SYSTEM 0x04
	#define ATTR_VOLUME_ID 0x08
	#define ATTR_DIRECTORY 0x10
	#define ATTR_ARCHIVE 0x20

	#define FAT_BUF_LEN fat12Par.wBytPerSec*2
	// FAT12 目录项
	struct FAT12DirEntry
	{
		char strFileName[8];	// 文件名
		char strFileExt[3];		// 文件扩展名
		BYTE byFileAttrib;		// 文件属性
		BYTE byReserved[10];	// 保留
		WORD wTime;				// 最后一次修改时间
		WORD wDate;				// 最后一次修改日期
		WORD wStartClus;		// 开始簇号
		DWORD dwFileSize;		// 文件大小，以字节为单位
	};

	//FAT12 文件系统参数
	struct FAT12Parameter{
		char strOEM[8];		//OEM信息	0x0003
		WORD wBytPerSec;	//每扇区字节数	0x000B
		BYTE bySecPerClu;	//每cluster的扇区数	0x000D
		WORD wSecReserved;	//保留扇区	0x000E
		BYTE byNumFats;		//FAT表数目	0x0010
		WORD wRootUnits;	//根目录项数	0x0011
		WORD wSecSmall;		//扇区总数（少于256*256）0x0013
		BYTE byMedia;		//媒体描述符 0xF8表示硬盘	0x0015
		WORD wSecPerFat;	//每FAT扇区数	0x0016
		WORD wSecPerTra;	//每磁道扇区数	0x0018
		WORD wHeads;		//磁头数(柱面数)	0x000A
		DWORD dwSecHidden;	//隐藏扇区	0x001C
		DWORD dwSecBig;		//大扇区数（大于等于256*256）	0x0020
		BYTE byDrive;		//物理驱动号， 00软驱 0x80为硬盘 0x0024
		BYTE byReserved;	//保留
		BYTE byExBootSig;	//扩展引导标志位
		DWORD dwVolume;		//序列号
		char strLabel[11];	//标签
		char strFSType[8];	//文件系统类型
	};

	BOOL fat12_ReadSuper(BYTE byDriveNO);
	BOOL fat12_Init(BYTE byDriveNO);
	BOOL fat12_Path(char far * strPath,struct File far * pFile);
	struct File far * fat12_fOpen(char far * strFileName, BYTE byMode);
	void fat12_fClose(struct File far * pFile);
	DWORD fat12_fRead(BYTE far * pDest, DWORD dwSize, DWORD dwCount, struct File far * pFile);
	DWORD fat12_fSeek(struct File far * pFile, DWORD dwOffset, BYTE by0rigin);
	DWORD fat12_fWrite(BYTE far * pSrc, DWORD dwSize, DWORD dwCount, struct File far * pFile);
	BOOL fat12_fRename(char far * strName, struct File far * pFile);
	BOOL fat12_fDelect(struct File far * pFile);
	BOOL fat12_fAttribute(struct File far * pFile, BYTE attribute);
	BOOL fat12_ReWorkDir(struct File far * pFile);
	struct FAT12Parameter far * getFat12Par();
#endif //ifdef  FAT12_H