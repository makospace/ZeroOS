//文件名：memManage.c
//功能：
//	内存管理相关函数
// 编译方法：
//		cl /nologo /c /Gs /G3 /w /Gx- memManage.obj memManage.c
//  如斯
//2007-6-7
#include "include/memManage.h"
#include "include/stdc.h"
#include "../include/quos.h"
#include "include/errorNO.h"
extern	BYTE g_byCurrProcessID;
void memInit(void)
{
	MCB far * pMCB = 0;
	// 手工构造最初的内存使用情况链表
	pMCB = (MCB far *)lineToFar(FRIST_MCB);
	pMCB->front = NULL;
	pMCB->next = NULL;
	pMCB->bSate = FREE;
	// pMCB->byProcessID = 0; 
	pMCB->dwSize = MAX_MEM_SIZE - sizeof(MCB);
	// 安装内存管理中断

}
//申请内存
void far * memMalloc(WORD wSize)
{
	MCB far * pMCB = (MCB far *)lineToFar(FRIST_MCB);
	do
	{
		if(pMCB->bSate==USED)
		{
			pMCB = pMCB->next;
		}
		if((pMCB->bSate==FREE))
		{
			if(pMCB->dwSize < sizeof(MCB))
			{
				continue;
			}
			else if((pMCB->dwSize-wSize)<=sizeof(MCB))// 如果该快内存剩余的内存不可在分了，那就一块分掉它
			{
				//BYTE far * pProcessID = (BYTE far *)lineToFar(CURR_BY_PROCESS_NO);
				pMCB->byProcessID = g_byCurrProcessID;
				pMCB->bSate = USED;
				return (MCB far *)lineToFar(farToLine(pMCB) + sizeof(MCB));
			}
			else// if((pMCB->dwSize-wSize)>sizeof(MCB))
			{
				MCB far * pOldMCB = pMCB->next;
				DWORD dwOldSize = pMCB->dwSize;
				//BYTE far * pProcessID = (BYTE far *)lineToFar(CURR_BY_PROCESS_NO);

				pMCB->byProcessID = g_byCurrProcessID;
				pMCB->bSate = USED;
				pMCB->dwSize = wSize;
				pMCB->next = (MCB far *)lineToFar(farToLine(pMCB) + sizeof(MCB) + wSize);

				pMCB->next->front = pMCB;
				pMCB->next->next = pOldMCB;
				pMCB->next->bSate = FREE;
				//pMCB->next->byProcessID = 0; 
				pMCB->next->dwSize = dwOldSize - wSize - sizeof(MCB);
				return (MCB far *)lineToFar(farToLine(pMCB) + sizeof(MCB));
			}
		}
	}while (pMCB->next != NULL);// 只要没有到链表的尾部就一直往下找
	return NULL; // 没有找到
}

//释放内存
int memFree(const void far * pIn)
{
	MCB far * pMCB = (MCB far *)lineToFar(farToLine(pIn) - sizeof(MCB));
	//BYTE far * pProcessID = (BYTE far *)lineToFar(CURR_BY_PROCESS_NO);

	MCB far * pFront = pMCB->front;
	MCB far * pNext = pMCB->next;
	
	//如果释放者和申请者不是同一个进程，就不允许释放
	if(pMCB->byProcessID != g_byCurrProcessID) return NO_RIGHT;
	//如果被释放的内存块没有被申请使用过，就不允许释放
	if(pMCB->bSate != USED) return INVALIDATION;
	if(pFront)
	{
		if(pFront->bSate == FREE)// 如果前一个内存块存在并且是空闲的，就把要释放的内存块和它的前一个合在一起
		{
			pFront->dwSize += pMCB->dwSize + sizeof(MCB);
			pFront->next = pNext;
			pMCB->bSate = UNKNOW;
			if(pNext)
				pNext->front = pFront;
		}
	}
	if(pNext)
	{
		if(pNext->bSate == FREE)// 如果后一个内存块存在并且是空闲的
		{
			if(pMCB->bSate == UNKNOW)// 如果已经和前面的合在一起了,那么就把前一块和后一块合在一起
			{
				pFront->dwSize += pNext->dwSize + sizeof(MCB);
				pFront->next = pNext->next;
				if(pNext->next)
					pNext->next->front = pFront;
				return OK;
			}
			else// 如果没有跟前一个合在一起,就直接跟后一个合在一起并释放
			{
				pMCB->bSate = FREE;
				//pMCB->byProcessID = 0;
				pMCB->dwSize += pNext->dwSize + sizeof(MCB);
				pMCB->next = pNext->next;
				if(pNext->next)
					pNext->next->front = pFront;
				return OK;
			}
		}
	}
	//如果前面的后面的都不是空闲的内存块,就直接释放它
	pMCB->bSate = FREE;
	//pMCB->byProcessID = 0;
	return OK;
}