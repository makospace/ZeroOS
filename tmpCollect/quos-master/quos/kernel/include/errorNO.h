#ifndef ERRORNO_H
#define ERRORNO_H

	#define OK 1

	// 申请内存失败，内存不足
	#define NO_MEMORY -1
	// 没有权限执行该操作
	#define NO_RIGHT -2
	// 无效操作
	#define INVALIDATION -3
	// 该驱动器不支持int13扩展读写
	#define CAN_NOT_USER_LBA -4
	// 打开文件失败
	#define OPEN_FILE_FAILE -5
	// 读取文件失败
	#define READ_FILE_FAILE -6
	// SEEK出错
	#define SEEK_FILE_FAILE -7
	

	// 系统中使用的BUG编号
	#define BUG_file_freeFileNo_1 1
	#define BUG_file_getFileStruct_1 2
	#define BUG_fat12_fSeek_1 3
	#define BUG_freeProcessNO_1 4


	// 系统中使用的致命错误编号
	#define PANIC_file_freeFileNo_1 1
	#define PANIC_freeProcessNO_1 2

#endif // ERRORNO_H
