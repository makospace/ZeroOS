#ifndef PROCESS_H
	#include "include/defs.h"
	#include "include/file.h"
	#include "include/stdc.h"
	#define PROCESS_H
	#define MAX_PROCESS 5
	#define MAX_FILE 64
	#define PATH_SIZE 128
	
	struct fileNO
	{
		BYTE NO;
		File far * fileStruct;
	};
	struct PCB //进程控制块
	{
		BYTE byProcessNO;		// 进程号
		//File far * file[MAX_FILE];	// 该进程打开的文件号
		struct fileNO file[MAX_FILE];
		char strRootPath[PATH_SIZE];	// 该进程的根目录
		char strArg[PATH_SIZE];			// 命令行参数
		union REGS r;					// 该进程的运行环境寄存器的值，用于任务切换，但是现在还不支持任务切换
		struct SREGS sr;
		WORD IP;
		WORD SP;
		WORD BP;
	};
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
	BOOL processManageInit();
	int exec(char far * strProgameName, char far * args);
#endif
