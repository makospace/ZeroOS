/*************************************** Copyright (c)******************************************************
** File name            :   switch.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS任务切换中与CPU相关的函数。
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2016-06-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
**********************************************************************************************************/
#include "tinyOS.h"
#include "tCpu.h"

// 在任务切换中，主要依赖了PendSV进行切换。PendSV其中的一个很重要的作用便是用于支持RTOS的任务切换。
// 实现方法为：
// 1、首先将PendSV的中断优先配置为最低。这样只有在其它所有中断完成后，才会触发该中断；
//    实现方法为：向NVIC_SYSPRI2写NVIC_PENDSV_PRI
// 2、在需要中断切换时，设置挂起位为1，手动触发。这样，当没有其它中断发生时，将会引发PendSV中断。
//    实现方法为：向NVIC_INT_CTRL写NVIC_PENDSVSET
// 3、在PendSV中，执行任务切换操作。
#define NVIC_INT_CTRL       0xE000ED04      // 中断控制及状态寄存器
#define NVIC_PENDSVSET      0x10000000      // 触发软件中断的值
#define NVIC_SYSPRI2        0xE000ED22      // 系统优先级寄存器
#define NVIC_PENDSV_PRI     0x000000FF      // 配置优先级

#define MEM32(addr)         *(volatile unsigned long *)(addr)
#define MEM8(addr)          *(volatile unsigned char *)(addr)

// 下面的代码中，用到了C文件嵌入ARM汇编
// 基本语法为:__asm 返回值 函数名(参数声明) {....}， 更具体的用法见Keil编译器手册，此处不再详注。

/**********************************************************************************************************
** Function name        :   tTaskEnterCritical
** Descriptions         :   进入临界区
** parameters           :   无
** Returned value       :   进入之前的临界区状态值
***********************************************************************************************************/
uint32_t tTaskEnterCritical (void)
{
    uint32_t primask = __get_PRIMASK();
    __disable_irq();        // CPSID I
    return primask;
}

/**********************************************************************************************************
** Function name        :   tTaskExitCritical
** Descriptions         :   退出临界区,恢复之前的临界区状态
** parameters           :   status 进入临界区之前的CPU
** Returned value       :   进入临界区之前的临界区状态值
***********************************************************************************************************/
void tTaskExitCritical (uint32_t status) {
    __set_PRIMASK(status);
}

#if defined(__TARGET_CPU_CORTEX_M0)

/**********************************************************************************************************
** Function name        :   tTaskStackInit
** Descriptions         :   初始化任务堆栈
** parameters           :   task        要初始化的任务结构
** parameters           :   entry       任务的入口函数
** parameters           :   stack/size  栈起始地址及大小
** Returned value       :   无
***********************************************************************************************************/
void tTaskStackInit(tTask * task, void (*entry)(void *), void *param, uint32_t * stack, uint32_t size) {
    uint32_t * stackTop;

    // 为了简化代码，tinyOS无论是在启动时切换至第一个任务，还是在运行过程中在不同间任务切换
    // 所执行的操作都是先保存当前任务的运行环境参数（CPU寄存器值）的堆栈中(如果已经运行运行起来的话)，然后再
    // 取出从下一个任务的堆栈中取出之前的运行环境参数，然后恢复到CPU寄存器
    // 对于切换至之前从没有运行过的任务，我们为它配置一个“虚假的”保存现场，然后使用该现场恢复。
    task->stackBase = stack;
    task->stackSize = size;
    memset(stack, 0, size);

    // 注意以下两点：
    // 1、不需要用到的寄存器，直接填了寄存器号，方便在IDE调试时查看效果；
    // 2、顺序不能变，要结合PendSV_Handler以及CPU对异常的处理流程来理解
    stackTop = stack + size / sizeof(tTaskStack);
    *(--stackTop) = (unsigned long)(1<<24);                // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stackTop) = (unsigned long)entry;                  // 程序的入口地址
    *(--stackTop) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stackTop) = (unsigned long)0x12;                   // R12, 未用
    *(--stackTop) = (unsigned long)0x3;                    // R3, 未用
    *(--stackTop) = (unsigned long)0x2;                    // R2, 未用
    *(--stackTop) = (unsigned long)0x1;                    // R1, 未用
    *(--stackTop) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stackTop) = (unsigned long)0x7;                    // R7, 未用
    *(--stackTop) = (unsigned long)0x6;                    // R6, 未用
    *(--stackTop) = (unsigned long)0x5;                    // R5, 未用
    *(--stackTop) = (unsigned long)0x4;                    // R4, 未用
    *(--stackTop) = (unsigned long)0x11;                   // R11, 未用
    *(--stackTop) = (unsigned long)0x10;                   // R10, 未用
    *(--stackTop) = (unsigned long)0x9;                    // R9, 未用
    *(--stackTop) = (unsigned long)0x8;                    // R8, 未用

    task->stack = stackTop;                             // 保存最终的值
}

/**********************************************************************************************************
** Function name        :   PendSV_Handler
** Descriptions         :   PendSV异常处理函数。很有些会奇怪，看不到这个函数有在哪里调用。实际上，只要保持函数头不变
**                          void PendSV_Handler (), 在PendSV发生时，该函数会被自动调用
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
__asm void PendSV_Handler ()
{
    IMPORT saveAndLoadStackAddr

    MRS     R0, PSP                   // 获取当前任务的堆栈指针
    
    MOVS    R1, #32
    SUBS    R0, R1                    // 预先计算, R0此时得出的最后的堆栈地址,方便后面用stmia保存
                                      //     保存的地址是当前任务的PSP堆栈中,便于下次恢复
    STMIA   R1!, {R4-R7}              //     将除异常自动保存的寄存器这外的其它寄存器自动保存起来{R4, R11}
    MOV     R4, R8                    // 在cortex-m0不STMDB不支持访问R8~R11寄存器,所以下面通过R4~R7间接写入
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R1!, {R4-R7}

    BL      saveAndLoadStackAddr        // 调用函数：参数通过R0传递，返回值也通过R0传递 
    
    LDMIA   R0!, {R4-R7}                // cortex-m0不支持LDMIA访问R8-R11,所以通过R4-R7间接取出
    MOV     R8, R4                      
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    LDMIA   R0!, {R4-R7}                // 取出R4-R7

    MSR     PSP, R0                     // 最后，恢复真正的堆栈指针到PSP

    MOVS    R0, #2                      // 生成0xFFFFFFFD
    MVNS    R0, R0
    BX      R0                          // 最后返回，此时任务就会从堆栈中取出LR值，恢复到上次运行的位置
}

#elif defined(__TARGET_CPU_CORTEX_M3)
void tTaskStackInit(tTask * task, void (*entry)(void *), void *param, uint32_t * stack, uint32_t size) {
    uint32_t * stackTop;

    // 为了简化代码，tinyOS无论是在启动时切换至第一个任务，还是在运行过程中在不同间任务切换
    // 所执行的操作都是先保存当前任务的运行环境参数（CPU寄存器值）的堆栈中(如果已经运行运行起来的话)，然后再
    // 取出从下一个任务的堆栈中取出之前的运行环境参数，然后恢复到CPU寄存器
    // 对于切换至之前从没有运行过的任务，我们为它配置一个“虚假的”保存现场，然后使用该现场恢复。
    task->stackBase = stack;
    task->stackSize = size;
    memset(stack, 0, size);

    // 注意以下两点：
    // 1、不需要用到的寄存器，直接填了寄存器号，方便在IDE调试时查看效果；
    // 2、顺序不能变，要结合PendSV_Handler以及CPU对异常的处理流程来理解
    stackTop = stack + size / sizeof(tTaskStack);
    *(--stackTop) = (unsigned long)(1<<24);                // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
    *(--stackTop) = (unsigned long)entry;                  // 程序的入口地址
    *(--stackTop) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stackTop) = (unsigned long)0x12;                   // R12, 未用
    *(--stackTop) = (unsigned long)0x3;                    // R3, 未用
    *(--stackTop) = (unsigned long)0x2;                    // R2, 未用
    *(--stackTop) = (unsigned long)0x1;                    // R1, 未用
    *(--stackTop) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stackTop) = (unsigned long)0x7;                    // R7, 未用
    *(--stackTop) = (unsigned long)0x6;                    // R6, 未用
    *(--stackTop) = (unsigned long)0x5;                    // R5, 未用
    *(--stackTop) = (unsigned long)0x4;                    // R4, 未用
    *(--stackTop) = (unsigned long)0x11;                   // R11, 未用
    *(--stackTop) = (unsigned long)0x10;                   // R10, 未用
    *(--stackTop) = (unsigned long)0x9;                    // R9, 未用
    *(--stackTop) = (unsigned long)0x8;                    // R8, 未用

    task->stack = stackTop;                             // 保存最终的值
}

__asm void PendSV_Handler (void) { 
    IMPORT saveAndLoadStackAddr
    
    // 切换第一个任务时,由于设置了PSP=MSP，所以下面的STMDB保存会将R4~R11
    // 保存到系统启动时默认的MSP堆栈中，而不是某个任务
    MRS     R0, PSP                 
    STMDB   R0!, {R4-R11}               // 将R4~R11保存到当前任务栈，也就是PSP指向的堆栈
    BL      saveAndLoadStackAddr        // 调用函数：参数通过R0传递，返回值也通过R0传递 
    LDMIA   R0!, {R4-R11}               // 从下一任务的堆栈中，恢复R4~R11
    MSR     PSP, R0
    
    MOV     LR, #0xFFFFFFFD             // 指明返回异常时使用PSP。注意，这时LR不是程序返回地址
    BX      LR
}

#endif

uint32_t saveAndLoadStackAddr (uint32_t stackAddr) {
    if (currentTask != (tTask *)0) {                    // 第一次切换时，当前任务为0
        currentTask->stack = (uint32_t *)stackAddr;     // 所以不会保存
    }
    currentTask = nextTask;                     
    return (uint32_t)currentTask->stack;                // 取下一任务堆栈地址
}


/**********************************************************************************************************
** Function name        :   tTaskRunFirst
** Descriptions         :   在启动tinyOS时，调用该函数，将切换至第一个任务运行
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskRunFirst()
{
    // 这里设置了一个标记，PSP = MSP, 二者都指向同一个堆栈
   __set_PSP(__get_MSP());

    MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;   // 向NVIC_SYSPRI2写NVIC_PENDSV_PRI，设置其为最低优先级

    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;    // 向NVIC_INT_CTRL写NVIC_PENDSVSET，用于PendSV

    // 可以看到，这个函数是没有返回
    // 这是因为，一旦触发PendSV后，将会在PendSV后立即进行任务切换，切换至第1个任务运行
    // 此后，tinyOS将负责管理所有任务的运行，永远不会返回到该函数运行
}

/**********************************************************************************************************
** Function name        :   tTaskSwitch
** Descriptions         :   进行一次任务切换，tinyOS会预先配置好currentTask和nextTask, 然后调用该函数，切换至
**                          nextTask运行
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskSwitch()
{
    // 和tTaskRunFirst, 这个函数会在某个任务中调用，然后触发PendSV切换至其它任务
    // 之后的某个时候，将会再次切换到该任务运行，此时，开始运行该行代码, 返回到
    // tTaskSwitch调用处继续往下运行
    MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;  // 向NVIC_INT_CTRL写NVIC_PENDSVSET，用于PendSV
}
