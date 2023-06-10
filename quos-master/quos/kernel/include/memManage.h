#ifndef MEMMANAGE_H
	#define MEMMANAGE_H
	#include "defs.h"
	#define FREE 0
	#define USED 1
	#define UNKNOW 2
// 第一个MCB的地址
	#define FRIST_MCB 0x500
	#define MAX_MEM_SIZE 0x70000 - FRIST_MCB
	typedef struct memControlBlock
	{
		struct memControlBlock far * front;
		struct memControlBlock far * next;
		BOOL bSate;		// 内存使用状态，
		BYTE byProcessID;	// 内存所属进程号
		DWORD dwSize;	// 实际可以内存的大小，不包括MCB的大小
	}MCB;
	void memInit(void);
	void far * memMalloc(WORD wSize);
	int memFree(const void far * pIn);
#endif //ifdef  MEMMANAGE_H