//文件名：fat12.c
//功能：
//  FAT12文件系统相关函数
// 编译方法：
//      cl /nologo /c /Gs /G3 /w /Gx- fat12.obj fat12.c
//  如斯
//2007-6-16

#include "include/stdc.h"
#include "include/fat12.h"
#include "include/disk.h"
#include "include/floppy.h"
#include "include/File.h"
#include "include/memManage.h"
#include "include/panic.h"
#include "include/errorNO.h"
#include "include/time.h"
static FileOpts fat12Opts = {
    fat12_fOpen,
    fat12_fClose,
    fat12_fRead,
    fat12_fSeek,
    fat12_fWrite,
    fat12_fRename,
    fat12_fDelect,
    fat12_fAttribute
};
static struct FAT12Parameter fat12Par;
static WORD wRootSectorNO;
static DWORD dwLastFatEntry;// 记录最后一个FAT项在FAT表中的偏移位置
struct FAT12Parameter far * getFat12Par()
{
    return (struct FAT12Parameter far *)&fat12Par;
}
BOOL fat12_ReadSuper(BYTE byDriveNO)
{
    BYTE byBootSec[512];
    BYTE far * pPar = 0;
    BOOL bRet;
    bRet = diskReadSector(0, 1, byDriveNO, (BYTE far *)byBootSec);
    #ifdef DEBUG_FAT12
    #endif
    if(!bRet)
    {
        #ifdef DEBUG_FAT12
        printf("\tfat12_ReadSuper: Error when read boot sector!\tbyDriveNO: %x\n", byDriveNO);
        #endif
        return FALSE;
    }
    pPar = lineToFar(farToLine((BYTE far *)byBootSec) + 3);
    
    //memcpy((void far *)&fat12Par, pPar, sizeof(struct FAT12Parameter));
    memcpy((char far *)&fat12Par, pPar, 10);
    pPar += 10;
    fat12Par.bySecPerClu = (BYTE)*pPar;
    pPar += 1;
    fat12Par.wSecReserved = (WORD)*pPar;
    pPar += 2;
    fat12Par.byNumFats = (BYTE)*pPar;
    pPar += 1;
    fat12Par.wRootUnits = (WORD)*pPar;
    pPar += 2;
    fat12Par.wSecSmall = (WORD)*pPar;
    pPar += 2;
    fat12Par.byMedia = (BYTE)*pPar;
    pPar += 1;
    fat12Par.wSecPerFat = (WORD)*pPar;
    pPar += 2;
    fat12Par.wSecPerTra = (WORD)*pPar;
    pPar += 2;
    fat12Par.wHeads = (WORD)*pPar;
    pPar += 2;
    fat12Par.dwSecHidden = (WORD)*pPar;
    pPar += 4;
    fat12Par.dwSecBig = (WORD)*pPar;
    pPar += 4;
    fat12Par.byDrive = (WORD)*pPar;
    pPar += 1;
    fat12Par.byReserved = (WORD)*pPar;
    pPar += 1;
    fat12Par.byExBootSig = (WORD)*pPar;
    pPar += 1;
    fat12Par.dwVolume = (WORD)*pPar;
    pPar += 4;
    memcpy((char far *)fat12Par.strLabel, pPar, 11);
    pPar += 11;
    memcpy((char far *)fat12Par.strFSType, pPar, 8);

    #ifdef DEBUG_FAT12
    printf("\tfat12_ReadSuper: fat12Par.strOME: %s\n", (char far *)fat12Par.strOEM);
    #endif
    wRootSectorNO = fat12Par.byNumFats * fat12Par.wSecPerFat + fat12Par.wSecReserved;
    dwLastFatEntry = 0;
    return TRUE;
}

BOOL fat12_Init(BYTE byDriveNO)
{
    if(fat12_ReadSuper(byDriveNO))
    {
        // 修改系统文件属性
        BYTE byAttribute = ATTR_SYSTEM|ATTR_HIDDEN;
        FILE far * fi = fat12_fOpen("KERNEL.EXE", "");
        fat12_fAttribute(fi, byAttribute);
        fat12_fClose(fi);
        fi = fat12_fOpen("SHELL.EXE", "");
        fat12_fAttribute(fi, byAttribute);
        fat12_fClose(fi);
        fi = fat12_fOpen("SETUP.BIN", "");
        fat12_fAttribute(fi, byAttribute);
        fi = fat12_fOpen("TEST.TXT", "");
        fat12_fAttribute(fi, byAttribute);
        fat12_fClose(fi);
        return TRUE;
    }

    return FALSE;
}
// 获取文件名和扩展名
void getFileName(const char far * strPath, char far *strName, char far *strExt)
{
    DWORD dwIndex = 0;
    if(!strPath)return;
    // 因为在FAT12中规定不足的空间用空格填，所以我们这儿使用0x20，即空格
    memset(strName, 0x20, sizeof(char)*FILE_NAME_SIZE);
    memset(strExt, 0x20, sizeof(char)*FILE_EXT_SIZE);
    // 注意：这儿并不支持目录
    dwIndex = strnchr(strPath, '.', FILE_NAME_SIZE+FILE_EXT_SIZE);
    if(dwIndex == 0)
        dwIndex = FILE_NAME_SIZE+FILE_EXT_SIZE;
    memcpy(strName, strPath, sizeof(char) * (dwIndex-1));
    memcpy(strExt, strPath+sizeof(char) * dwIndex, sizeof(char) * (strlen(strPath)-dwIndex));
//  {
//  char ch[64];
//  memset((char far *)ch, 0, 64);
//
//  memcpy((char far *)ch, strPath+sizeof(char) * dwIndex, sizeof(char) * (strlen(strPath)-dwIndex));
//  printf("strPath=[%s]\tdwIndex=%ld\tch=[%s]\t%d\n", strPath, dwIndex, (char far *)ch, strlen(strPath)-dwIndex);
//  getch();
//  }
}
// 从对应的目录中找到指定的文件
BOOL fat12_Path(char far * strPath, File far * pFile)
{
    BYTE buffer[512];
    BOOL bRet;
    WORD wSectorNO = wRootSectorNO;
    struct FAT12DirEntry far * pDirEntry = NULL;

    // 设置驱动器号，本来应该是从文件路径获取的，但是现在不支持路径，所以...
    pFile->byDevNO = FLOPPY_A;  // 注意：这儿仅仅支持软盘
    if(!fat12_GetDirSector(pFile, (BYTE far *)buffer, (void far *)&pDirEntry))return FALSE;
    //找到该文件了
    memcpy(pFile, pDirEntry, FILE_NAME_SIZE+FILE_EXT_SIZE);
    pDirEntry = lineToFar(farToLine((BYTE far *)pDirEntry)+FILE_NAME_SIZE+FILE_EXT_SIZE);
    pFile->byAttribute = *(BYTE far *)pDirEntry;
    pDirEntry = (BYTE far *)pDirEntry+11;
    pFile->wTime = *(WORD far *)pDirEntry;
    pDirEntry = (BYTE far *)pDirEntry+2;
    pFile->wDate = *(WORD far *)pDirEntry;
    pDirEntry = (BYTE far *)pDirEntry+2;
    pFile->wFirstCus = *(WORD far *)pDirEntry;
    pDirEntry = (BYTE far *)pDirEntry+2;
    pFile->dwSize = *(DWORD far *)pDirEntry;
    return TRUE;
    #ifdef DEBUG_FAT12
    printf("\tfat12_Path: Can't find out the File named %s in the drive[%d]!\n", strPath, pFile->byDevNO);
    #endif
    return FALSE;
}

File far * fat12_fOpen(char far * strFileName, BYTE byMode)
{
    // BYTE fileNO = allocFileNo();
    File far * pFile = memMalloc(sizeof(File));
    if(!pFile)
    {
        #ifdef DEBUG_FAT12
        puts("\tfat12_fOpen: Error when malloc File space!\n");
        #endif
        goto error;
    }
    // 初始化file结构
    memset(pFile, 0, sizeof(File));
    pFile->byMode = byMode;
    pFile->pOpts = (FileOpts far *)&fat12Opts;
    pFile->pFSPar = (struct FAT12Parameter far *)&fat12Par;
    getFileName(strFileName, (char far *)pFile->strName, (char far *)pFile->strExt);
    // 获取该文件的其他属性
    if(!fat12_Path(strFileName, pFile))
    {
        goto errorFree;
    }
    return pFile;

errorFree:
    memFree(pFile);
    #ifdef DEBUG_FAT12
    puts("\tfat12_fOpen: errorFree\n");
    #endif
error:
    #ifdef DEBUG_FAT12
    printf("\tfat12_fOpen: error:strFileName: %s  byMode: %d\n", strFileName, byMode);
    #endif
    return 0;
}
void fat12_fClose(File far * pFile)
{
    memFree(pFile); 
}
// 读取指定簇号的值
DWORD getClusterNO(WORD wFirstCus, WORD wCount, BYTE byDriveNO, BYTE far * pFATBuffer, WORD far * pFATBufSecNO)
{
    WORD wDescFATSecNO;
    BOOL bRet;
    DWORD dwFatValue, dwCurrCus = wFirstCus;
    while(wCount)
    {
        BYTE byOffInBuffer = (dwCurrCus * 12)/8 % (fat12Par.wBytPerSec);
        wDescFATSecNO = (dwCurrCus * 12 + 7)/8 / (fat12Par.wBytPerSec) + 1 + fat12Par.wSecReserved;
        #ifdef DEBUG_FAT12
        printf("\tgetClusterNO: dwCurrCus=0x%lx  wDescFATSecNO:0x%x   wCount:%d    byOffInBuffer:%d\n", 
            dwCurrCus, wDescFATSecNO, wCount, byOffInBuffer);
        #endif
        if(*pFATBufSecNO != wDescFATSecNO)// 如果目标簇的FAT不在缓存中，就必须刷新缓存
        {
            bRet = diskReadSector(wDescFATSecNO - 1, 2, byDriveNO, pFATBuffer);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12
                printf("\tgetClusterNO: Error when read sector[%d] in drive[%x]!\n", wDescFATSecNO, byDriveNO);
                #endif
                goto error;
            }
            *pFATBufSecNO = wDescFATSecNO;
        }
        // 找到wFirstCus的FAT值
        if((dwCurrCus * 12)%8)
        {
            dwFatValue = ((DWORD)pFATBuffer[byOffInBuffer])>>4;
            dwFatValue += (DWORD)pFATBuffer[byOffInBuffer+1] << 4;
//          #ifdef DEBUG_FAT12
//          printf("yu:\tdwFatValue:0x%lx\n", dwFatValue);
//          getch();
//          #endif
        }
        else
        {
            dwFatValue = pFATBuffer[byOffInBuffer];
            dwFatValue += (DWORD)(pFATBuffer[byOffInBuffer+1]&0xf)<<8;
//          #ifdef DEBUG_FAT12
//          printf("\tdwFatValue:0x%lx\n", dwFatValue);
//          getch();
//          #endif
        }
        // 找下一个，直到wCount为0
        if(dwFatValue<0xFF0 && dwFatValue>0)
        {
            wCount--;
            dwCurrCus = dwFatValue;
        }
        else if(dwFatValue>=0xFF8 && dwFatValue<0xFFF)//到了文件的结尾
        {
            #ifdef DEBUG_FAT12
            printf("\tgetClusterNO: dwFatValue:0x%lx   wCount:%d\n", dwFatValue, wCount);
            getch();
            #endif
            if(wCount>0)return 0xFFF;
        }
        else
        {
            #ifdef DEBUG_FAT12
            printf("\tgetClusterNO: dwFatValue:0x%lx   wCount:%d\n", dwFatValue, wCount);
            getch();
            #endif
            // 出错了，找到了坏的簇或者找到文件的结尾还没有找到用户需要的部分。
            if(wCount>0)goto error;
        }
    }
    return dwCurrCus;


error:
    return 0;
}
DWORD fat12_fRead(BYTE far * pDest, DWORD dwSize, DWORD dwCount, File far * pFile)
{
    WORD wStartSectorInDisk, wFATBufSecNO = 0;// wFATBufSecNO一定要赋值为0，因为FAT肯定不会在第0个扇区里面
    BYTE far * pBuffer = memMalloc(fat12Par.wBytPerSec), far * pWork = pDest, far * pFATBuf = memMalloc(FAT_BUF_LEN);
    BOOL bRet;
    // 如果请求的大于文件本事大小，就只能获取到文件大小的数据
    DWORD dwRemanent = pFile->dwSize < dwSize?pFile->dwSize:dwSize;// 剩余未读取字节数
    
    if(!pBuffer)
    {
        #ifdef DEBUG_FAT12
        puts("\tfat12_fRead: Error when malloc buffer space!\n");
        #endif
        goto error;
    }
    if(!pFATBuf)
    {
        #ifdef DEBUG_FAT12
        puts("\tfat12_fRead: Error when malloc FAT buffer space!\n");
        #endif
        goto errorFreeBuffer;
    }
    // 验证是否可读
    if(!(pFile->byMode&FILE_READ))goto errorFree;
    while(dwRemanent)
    {
        // 计算即将读取的扇区在磁盘中的位置
        DWORD dwClusterNO = getClusterNO(pFile->wFirstCus, (pFile->dwPos+511)/512, pFile->byDevNO, pFATBuf, (WORD far *)&wFATBufSecNO);
        if(dwClusterNO==0 || dwClusterNO==0xFFF)
        {
            dwSize = dwSize - dwRemanent;//出错或者读到了文件的结尾
            goto REOK;
        }
        wStartSectorInDisk = dwClusterNO + wRootSectorNO - 2 + 
            (fat12Par.wRootUnits * 32 + fat12Par.wBytPerSec - 1) / fat12Par.wBytPerSec;
        #ifdef DEBUG_FAT12
        printf("\tfat12_fRead: wStartSectorInDisk:%d Pos:%ld\n", wStartSectorInDisk, (pFile->dwPos+511)/512);
        getch();
        #endif
        // 讲该扇区读入到缓存，
        bRet = diskReadSector(wStartSectorInDisk - 1, 1, pFile->byDevNO, pBuffer);
        if(!bRet)
        {
            #ifdef DEBUG_FAT12
            printf("\tfat12_fRead: Error when read sector[%d] in drive[%x]!\n", wStartSectorInDisk, pFile->byDevNO);
            #endif
            goto errorFree;
        }
        // 然后将用户所需要的部分拷贝到用户空间
        {
            BYTE byFirstStart = pFile->dwPos % 512;
            BYTE far * p = lineToFar(farToLine(pBuffer)+byFirstStart);
            if(dwRemanent + byFirstStart <= 512)
            {
                memcpy(pWork, p, dwRemanent);
                pFile->dwPos += dwRemanent;
                dwSize = dwRemanent;
                goto REOK;
            }
            else
            {
                memcpy(pWork, p, 512 - byFirstStart);
                dwRemanent -= (512 - byFirstStart);
                pFile->dwPos += 512 - byFirstStart;
                pWork = lineToFar(farToLine(pWork) + 512 - byFirstStart);
            }
        }
    }
REOK:
    memFree(pFATBuf);
    memFree(pBuffer);
    return dwSize;

errorFree:
    memFree(pFATBuf);
errorFreeBuffer:
    memFree(pBuffer);
error:
    return 0;
}

DWORD fat12_fSeek(File far * pFile, DWORD dwOffset, BYTE by0rigin)
{
    if(by0rigin == SEEK_CUR)
        pFile->dwPos += dwOffset;
    else if(by0rigin == SEEK_SET)
        pFile->dwPos = dwOffset;
    else if(by0rigin == SEEK_END)
        pFile->dwPos = pFile->dwSize - dwOffset;
    else
    {
        bug(BUG_fat12_fSeek_1);
        return 0;
    }
    // 检查是否越界
    if(pFile->dwPos > pFile->dwSize)
        pFile->dwPos = pFile->dwSize;
    else if(pFile->dwPos < 0)
        pFile->dwPos = 0;
    return pFile->dwPos;
}
// 获取指定文件的最后一个簇号
BOOL setLastCusNO(WORD wFirstCus, BYTE byDriveNO,
    BYTE far * pFATBuffer, WORD far * pFATBufSecNO, WORD wNewVal)
{   
    WORD wDescFATSecNO;
    BOOL bRet;
    DWORD dwFatValue, dwCurrCus = wFirstCus;
	#ifdef DEBUG_FAT12_WRITE
	printf("setLastCusNO(wFirstCus=0x%x, wNewVal=0x%x)\n", wFirstCus, wNewVal);
	getch();
	#endif
    while(1)
    {
        BYTE byOffInBuffer = (dwCurrCus * 12)/8 % (fat12Par.wBytPerSec);
        wDescFATSecNO = (dwCurrCus * 12 + 7)/8 / (fat12Par.wBytPerSec) + 1 + fat12Par.wSecReserved;
        #ifdef DEBUG_FAT12_WRITE
        printf("\tsetLastCusNO: dwCurrCus=0x%lx  wDescFATSecNO:0x%x  byOffInBuffer:%d\n", 
            dwCurrCus, wDescFATSecNO, byOffInBuffer);
        #endif
        if(wDescFATSecNO>19)
            return FALSE;
        if(*pFATBufSecNO != wDescFATSecNO)// 如果目标簇的FAT不在缓存中，就必须刷新缓存
        {
            bRet = diskReadSector(wDescFATSecNO - 1, 2, byDriveNO, pFATBuffer);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12_WRITE
                printf("\tsetLastCusNO: Error when read sector[%d] in drive[%x]!\n", wDescFATSecNO, byDriveNO);
                #endif
                return FALSE;
            }
            *pFATBufSecNO = wDescFATSecNO;
        }
        // 找到wFirstCus的FAT值
        if((dwCurrCus * 12)%8)
        {
            dwFatValue = ((DWORD)pFATBuffer[byOffInBuffer]) >> 4;
            dwFatValue += (DWORD)pFATBuffer[byOffInBuffer+1] << 4;
            #ifdef DEBUG_FAT12_WRITE
            printf("\t\tNotIsByte:\tdwFatValue:0x%lx\n", dwFatValue);
            //getch();
            #endif
        }
        else//整字节处
        {
            dwFatValue = pFATBuffer[byOffInBuffer];
            dwFatValue += (DWORD)(pFATBuffer[byOffInBuffer+1] & 0xf) << 8;
            #ifdef DEBUG_FAT12_WRITE
            printf("\t\tIsByte:\tdwFatValue:0x%lx\n", dwFatValue);
            //getch();
            #endif
        }
        // 找下一个
        if(dwFatValue<0xFF0 && dwFatValue>0)
        {
            dwCurrCus = dwFatValue;
        }
        else if(dwFatValue>=0xFF8 && dwFatValue<=0xFFF)//到了文件的结尾
        {
			#ifdef DEBUG_FAT12_WRITE
			printf("\tfind the last cusNO is <0x%x>\n", dwCurrCus, byDriveNO);
			#endif
            // 写入新值
            if((dwCurrCus * 12)%8)
            {
				printf("notIsByte:wNewVal=0x%x  0x%x\n", wNewVal, (wNewVal & 0xf) << 4);
				getch();
                pFATBuffer[byOffInBuffer] =
                    (pFATBuffer[byOffInBuffer] & 0xf) + ((wNewVal & 0xf) << 4);
                pFATBuffer[byOffInBuffer+1] = wNewVal >> 4; 
            }
            else//整字节处
            {
				printf("IsByte:wNewVal=0x%x\n", wNewVal);
				getch();
                pFATBuffer[byOffInBuffer] = (BYTE)wNewVal;
                pFATBuffer[byOffInBuffer+1] = 
                    ((wNewVal >> 4) & 0xf0)
                    + (pFATBuffer[byOffInBuffer+1] & 0xf);
            }
            //刷新簇缓冲到磁盘
            bRet = diskWriteSector(*pFATBufSecNO-1, 2, byDriveNO, pFATBuffer);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12_WRITE
                printf("\tsetLastCusNO: Error when wirte sector[%d] in drive[%x]!\n", *pFATBufSecNO, byDriveNO);
                #endif
                return FALSE;
            }
            return TRUE;
        }
        else
        {
            #ifdef DEBUG_FAT12_WRITE
            printf("\tsetLastCusNO: error:bad cus:dwFatValue:0x%lx\n", dwFatValue);
            getch();
            #endif
            // 出错了，找到了坏的簇
            return FALSE;
        }
    }
    return FALSE;
}
// 生成一个新的簇号
DWORD getNewClusterNO(BYTE far * pFATBuffer, BYTE byDriveNO, WORD far * pFATBufSecNO)
{
    BYTE bRet;
    BYTE far * pWork = pFATBuffer;
    WORD wCusVal = 0, wCusNO;
    int i;
    BOOL bFind = FALSE,
    bIsByte = TRUE;//现在正在处理的FAT表项是否为整字节处
	#ifdef DEBUG_FAT12_WRITE
	printf("getNewClusterNO\n");
	#endif
    if(*pFATBufSecNO != 1)//必须从第一个FAT表扇区处理，如果不是就读入
    {
    bRet = diskReadSector(1, 2, byDriveNO, pFATBuffer);
    if(!bRet)
    {
        #ifdef DEBUG_FAT12_WRITE
        printf("\tgetNewClusterNO: Error when read sector[%d] in drive[%x]!\n", 1, byDriveNO);
        #endif
        goto error;
        }
        *pFATBufSecNO = 1;
    }
    pWork += 3;// 因为前三个字节有特殊用途，不属于处理范围
    wCusNO = 2;// 第一个可使用的簇号为2，前两个(即前三个字节)作为特殊用途
    do
    {
        if(bIsByte)
        {
            wCusVal = *pWork;
            pWork++;
            wCusVal += ((DWORD)(*pWork & 0xf) << 8);
            pWork--;// 指向该簇号起始字节
        }
        else
        {
            wCusVal = *pWork >> 4;
            pWork++;
            wCusVal += (*pWork << 4);
            pWork--;// 指向该簇号起始字节
        }
        if(wCusVal==0)// 找到了可以使用的簇
        {
            // 写入值0xfff，以标识其为文件结束簇号
            if(bIsByte)
            {
                *pWork = 0xff;
                pWork++;
                *pWork = 0xf + (*pWork & 0xf0);
            }
            else
            {
                *pWork = 0xf0 + (*pWork & 0xf);
                pWork++;
                *pWork = 0xff;
            }
            //刷新簇缓冲到磁盘
            bRet = diskWriteSector(*pFATBufSecNO, 2, byDriveNO, pFATBuffer);
            if(!bRet)
            {
				#ifdef DEBUG_FAT12_WRITE
				printf("\tgetNewClusterNO: Error when wirte sector[%d] in drive[%x]!\n", *pFATBufSecNO, byDriveNO);
				#endif
				goto error;
            }
			#ifdef DEBUG_FAT12_WRITE
			printf("\tgetNewClusterNO: return 0x%x\n", wCusNO);
			getch();
			#endif
            return wCusNO;
        }
        // 没有找到，下一个FAT表项是否完全在下一扇区了？
        if((pWork - pFATBuffer)==513)
        {
            *pFATBufSecNO += 1;
            bRet = diskReadSector(*pFATBufSecNO+1, 2, byDriveNO, pFATBuffer);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12_WRITE
                printf("\tgetNewClusterNO: Error when read sector[%d] in drive[%x]!\n", *pFATBufSecNO, byDriveNO);
                #endif
                goto error;
            }
            pWork = pFATBuffer + 1;
        }
        if(bIsByte)
        {
            pWork++;
        }
        else
        {
            pWork += 2;
        }
        wCusNO++;
		wCusVal = 0;
        bIsByte = !bIsByte;
    }while(*pFATBufSecNO <= fat12Par.byNumFats+1);
error:
    return 0;// 失败，没有找到可用的簇号        
}
DWORD fat12_fWrite(const BYTE far * pSrc, DWORD dwSize, DWORD dwCount, File far * pFile)
{
    BYTE far * pFATBuf = memMalloc(FAT_BUF_LEN), //用于缓存FAT表
         bRet = 0, 
         far * pBuf = memMalloc(fat12Par.wBytPerSec);//用于缓存即将写入文件的数据
    WORD wCusNO = 0, wSecNO = 0, 
         wFATBufSecNO = 0;  // wFATBufSecNO一定要赋值为0
                //因为FAT肯定不会在第0个扇区里面
    DWORD dwRemanent = dwSize;// 剩余还未写入文件的长度
    BYTE far * pWork = pSrc, attribute;
    BYTE byFirst = pFile->dwPos - (pFile->dwPos / 512) * 512;
    DWORD dwNewSize = pFile->dwPos + dwSize * dwCount;
	
	#ifdef DEBUG_FAT12_WRITE
	printf("fat12_fWrite\n");
	#endif
    if(!pFATBuf)
    {
        #ifdef DEBUG_FAT12_WRITE
        puts("\tfat12_fWrite: Error when malloc FAT buffer space!\n");
        #endif
        goto error;
    }
    if(!pBuf)
    {
        #ifdef DEBUG_FAT12_WRITE
        puts("\tfat12_fWrite: Error when malloc Data buffer space!\n");
        #endif
        goto errorFreeFATBuf;
    }
    // 这儿需要检验文件的打开模式已经文件是否可写
    #ifdef DEBUG_FAT12_WRITE
    printf("byFirst=%d\n",byFirst);
    getch();
    #endif
    while(dwRemanent)
    {
        // 获取写入位置的簇号，该簇号是新的未使用的？还是已经存在的
        wCusNO = getClusterNO(pFile->wFirstCus, (pFile->dwPos)/512,
                pFile->byDevNO, pFATBuf, (WORD far *)&wFATBufSecNO);
        
        #ifdef DEBUG_FAT12_WRITE
        printf("wCusNO=%d\n", wCusNO);
        getch();
        #endif
        if(!wCusNO)// 如果获取到的簇号等于0，即该簇号没有找不到
        {
            // 分配一个新的簇号，并将该簇号作为该文件的最后一个簇号
            wCusNO = getNewClusterNO(pFATBuf, pFile->byDevNO, 
                (WORD far *)&wFATBufSecNO);
            #ifdef DEBUG_FAT12_WRITE
            printf("new wCusNO=%d\n", wCusNO);
            getch();
            #endif
            // 更新该文件原有的最后一个簇号的值
            setLastCusNO(pFile->wFirstCus, pFile->byDevNO, pFATBuf, 
                (WORD far *)&wFATBufSecNO, wCusNO);
        }

        wSecNO = wCusNO + wRootSectorNO - 2 + 
            (fat12Par.wRootUnits * 32 + fat12Par.wBytPerSec - 1) /
            fat12Par.wBytPerSec;
        
        #ifdef DEBUG_FAT12_WRITE
        printf("wSecNO=%d\n", wSecNO);
        getch();
        #endif
        // 写入磁盘
        //      如果写的数据不是整个的扇区
        //      先读到缓存中，在缓存中更新，然后再将缓存写入
        if((byFirst+dwRemanent) < fat12Par.wBytPerSec)
        {
            bRet = diskReadSector(wSecNO - 1, 1, pFile->byDevNO, pBuf);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12_WRITE
                printf("\tgetNewClusterNO: Error when read sector[%d] in drive[%x]!\n", wSecNO, pFile->byDevNO);
                #endif
                goto errorFree;
            }
            memcpy(pBuf+byFirst, pWork, dwRemanent);
            pWork += dwRemanent;
            dwRemanent = 0;
        }
        else// if((byFirst+dwRemanent) > fat12Par.wBytPerSec)
        {
            bRet = diskReadSector(wSecNO - 1, 1, pFile->byDevNO, pBuf);
            if(!bRet)
            {
                #ifdef DEBUG_FAT12_WRITE
                printf("\tgetNewClusterNO: Error when read sector[%d] in drive[%x]!\n", wSecNO, pFile->byDevNO);
                #endif
                goto errorFree;
            }
            memcpy(pBuf+byFirst, pWork, fat12Par.wBytPerSec - byFirst);
            pWork += (fat12Par.wBytPerSec - byFirst);
            dwRemanent -= (fat12Par.wBytPerSec - byFirst);
            byFirst=0;
        }
        /*else
        {
            memcpy(pBuf, pWork, fat12Par.wBytPerSec);
            dwRemanent -= fat12Par.wBytPerSec;          
            pWork += fat12Par.wBytPerSec;
        }*/
        bRet = diskWriteSector(wSecNO - 1, 1, pFile->byDevNO, pBuf);
        if(!bRet)
        {
            #ifdef DEBUG_FAT12_WRITE
            printf("\tgetNewClusterNO: Error when wirte sector[%d] in drive[%x]!\n", wSecNO, pFile->byDevNO);
            #endif
            goto errorFree;
        }
        // 更新文件属性
        pFile->wDate = (((getYear()-1980)<<9)&0xfe00) 
            | ((getMonth()<<5)&0x1e0) | (getDay()&0x1f);
        pFile->wTime = ((getHour()<<11)&0xf800) 
            | ((getMinute()<<5)&0x7e0) | ((getSecond()/2)&0x1f);
        pFile->dwSize = dwNewSize;
        pFile->dwPos = dwNewSize;
        fat12_ReWorkDir(pFile);
    }
errorFree:
    memFree(pBuf);
errorFreeFATBuf:
    memFree(pFATBuf);
error:
    return 0;
}
// 从对应的目录中找到指定的文件
BOOL fat12_ReWorkDir(File far * pFile)
{
    BYTE buffer[512];
    BYTE far *pDirEntry = NULL;
    BOOL bRet;
    // 找到文件目录项所在扇区
    WORD wSectorNO = fat12_GetDirSector(pFile, (BYTE far *)buffer, (void far *)&pDirEntry);
    if(!wSectorNO)return FALSE;
    // 将新的属性写入缓冲
    pDirEntry = lineToFar(farToLine((BYTE far *)pDirEntry)+FILE_NAME_SIZE+FILE_EXT_SIZE);
    *(BYTE far *)pDirEntry = pFile->byAttribute;
    pDirEntry = (BYTE far *)pDirEntry+11;
    *(WORD far *)pDirEntry = pFile->wTime;
    pDirEntry = (BYTE far *)pDirEntry+2;
    *(WORD far *)pDirEntry = pFile->wDate;
    pDirEntry = (BYTE far *)pDirEntry+2;
    *(WORD far *)pDirEntry = pFile->wFirstCus;
    pDirEntry = (BYTE far *)pDirEntry+2;
    *(DWORD far *)pDirEntry = pFile->dwSize;
    // 将缓冲刷新到磁盘
    bRet = diskWriteSector(wSectorNO - 1, 1, pFile->byDevNO, (BYTE far *)buffer);
    if(!bRet)
    {
        #ifdef DEBUG_FAT12
        printf("\tfat12_ReWorkDir: Error when wirte sector[%d] in drive[%x]!\n", wSectorNO, pFile->byDevNO);
        #endif
        return FALSE;
    }
    return TRUE;
}
BOOL fat12_fRename(char far * strName, File far * pFile)
{
    BYTE buffer[512];
    BYTE far *pDirEntry = NULL;
    BOOL bRet;
    WORD wSectorNO;
    // 检验是否是系统文件
    if((pFile->byAttribute & ATTR_SYSTEM) && (pFile->byAttribute & ATTR_READ_ONLY))return FALSE;
    // 先找到文件所在扇区
    wSectorNO = fat12_GetDirSector(pFile, (BYTE far *)buffer, (void far *)&pDirEntry);
    if(!wSectorNO)return FALSE;
    
    // 将新的文件名写入到缓冲
    getFileName(strTOSuper(strName), (char far *)pFile->strName, (char far *)pFile->strExt);
    memcpy(pDirEntry, pFile, FILE_NAME_SIZE+FILE_EXT_SIZE); 

    // 将缓冲写入磁盘
    bRet = diskWriteSector(wSectorNO - 1, 1, pFile->byDevNO, (BYTE far *)buffer);
    if(!bRet)
    {
        #ifdef DEBUG_FAT12
        printf("\tfat12_fRename: Error when wirte sector[%d] in drive[%x]!\n", wSectorNO, pFile->byDevNO);
        #endif
        return FALSE;
    }
}

BOOL fat12_fDelect(File far * pFile)
{
    BYTE buffer[512];
    BYTE far *pDirEntry = NULL;
    BOOL bRet;
    WORD wSectorNO;
    // 检验是否是系统文件
    if((pFile->byAttribute & ATTR_SYSTEM) && (pFile->byAttribute & ATTR_READ_ONLY))return FALSE;
    // 先找到文件所在扇区
    wSectorNO = fat12_GetDirSector(pFile, (BYTE far *)buffer, (void far *)&pDirEntry);
    if(!wSectorNO)return FALSE;
    
    // 将删除标记写入到缓冲
    *(char far *)pFile->strName = 0xe5;
    memcpy(pDirEntry, pFile, FILE_NAME_SIZE+FILE_EXT_SIZE); 

    // 将缓冲写入磁盘
    bRet = diskWriteSector(wSectorNO - 1, 1, pFile->byDevNO, (BYTE far *)buffer);
    if(!bRet)
    {
        #ifdef DEBUG_FAT12
        printf("\tfat12_fRename: Error when wirte sector[%d] in drive[%x]!\n", wSectorNO, pFile->byDevNO);
        #endif
        return FALSE;
    }
}

BOOL fat12_fAttribute(File far * pFile, BYTE byAttribute)
{
    pFile->byAttribute = byAttribute;
    fat12_ReWorkDir(pFile);
}
