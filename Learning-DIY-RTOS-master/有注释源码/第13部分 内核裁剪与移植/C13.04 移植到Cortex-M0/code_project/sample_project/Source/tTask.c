/*************************************** Copyright (c)******************************************************
** File name            :   tTask.c
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS的任务管理接口实现
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

/**********************************************************************************************************
** Function name        :   tTaskInit
** Descriptions         :   初始化任务结构
** parameters           :   task        要初始化的任务结构
** parameters           :   entry       任务的入口函数
** parameters           :   param       传递给任务的运行参数
** Returned value       :   无
***********************************************************************************************************/
void tTaskInit (tTask * task, void (*entry)(void *), void *param, uint32_t prio, uint32_t * stack, uint32_t size)
{
    tTaskStackInit(task, entry, param, stack, size);

    task->slice = TINYOS_SLICE_MAX;                     // 初始化任务的时间片计数
    task->prio = prio;                                  // 设置任务的优先级
    task->state = TINYOS_TASK_STATE_RDY;                // 设置任务为就绪状态
    task->suspendCount = 0;                             // 初始挂起次数为0
    task->clean = (void(*)(void *))0;                   // 设置清理函数
    task->cleanParam = (void *)0;                       // 设置传递给清理函数的参数
    task->requestDeleteFlag = 0;                        // 请求删除标记

    task->waitEvent = (tEvent *)0;                      // 没有等待事件
    task->eventMsg = (void *)0;                         // 没有等待事件
    task->waitEventResult = tErrorNoError;              // 没有等待事件错误

    tNodeInit(&(task->delayNode));                      // 初始化延时结点
    
    tNodeInit(&(task->linkNode));                       // 初始化链接结点

    tTaskSchedRdy(task);                                // 将任务插入就绪队列

#if TINYOS_ENABLE_HOOKS == 1
    tHooksTaskInit(task);
#endif
}

/**********************************************************************************************************
** Function name        :   tTaskSuspend
** Descriptions         :   挂起指定的任务
** parameters           :   task        待挂起的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskSuspend (tTask * task) 
{
    // 进入临界区
    uint32_t status = tTaskEnterCritical();

    // 不允许对已经进入延时状态的任务挂起
    if (!(task->state & TINYOS_TASK_STATE_DELAYED)) 
    {
        // 增加挂起计数，仅当该任务被执行第一次挂起操作时，才考虑是否
        // 要执行任务切换操作
        if (++task->suspendCount <= 1)
        {
            // 设置挂起标志
            task->state |= TINYOS_TASK_STATE_SUSPEND;

            // 挂起方式很简单，就是将其从就绪队列中移除，这样调度器就不会发现他
            // 也就没法切换到该任务运行
            tTaskSchedUnRdy(task);

            // 当然，这个任务可能是自己，那么就切换到其它任务
            if (task == currentTask)
            {
                tTaskSched();
            }
        }
    }

    // 退出临界区
    tTaskExitCritical(status); 
}

/**********************************************************************************************************
** Function name        :   tTaskWakeUp
** Descriptions         :   唤醒被挂起的任务
** parameters           :   task        待唤醒的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskWakeUp (tTask * task)
{
    // 进入临界区
    uint32_t status = tTaskEnterCritical();

     // 检查任务是否处于挂起状态
    if (task->state & TINYOS_TASK_STATE_SUSPEND) 
    {
        // 递减挂起计数，如果为0了，则清除挂起标志，同时设置进入就绪状态
        if (--task->suspendCount == 0) 
        {
            // 清除挂起标志
            task->state &= ~TINYOS_TASK_STATE_SUSPEND;

            // 同时将任务放回就绪队列中
            tTaskSchedRdy(task);

            // 唤醒过程中，可能有更高优先级的任务就绪，执行一次任务调度
            tTaskSched();
        }
    }

    // 退出临界区
    tTaskExitCritical(status); 
}

/**********************************************************************************************************
** Function name        :   tTaskSetCleanCallFunc
** Descriptions         :   设置任务被删除时调用的清理函数
** parameters           :   task  待设置的任务
** parameters           :   clean  清理函数入口地址
** parameters           :   param  传递给清理函数的参数
** Returned value       :   无
***********************************************************************************************************/
void tTaskSetCleanCallFunc (tTask * task, void (*clean)(void * param), void * param) 
{
    task->clean = clean;
    task->cleanParam = param;
}

/**********************************************************************************************************
** Function name        :   tTaskForceDelete
** Descriptions         :   强制删除指定的任务
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskForceDelete (tTask * task) 
{
    // 进入临界区
    uint32_t status = tTaskEnterCritical();

     // 如果任务处于延时状态，则从延时队列中删除
    if (task->state & TINYOS_TASK_STATE_DELAYED) 
    {
        tTimeTaskRemove(task);
    }
		// 如果任务不处于挂起状态，那么就是就绪态，从就绪表中删除
    else if (!(task->state & TINYOS_TASK_STATE_SUSPEND)) 
    {
        tTaskSchedRemove(task);
    }

    // 删除时，如果有设置清理函数，则调用清理函数
    if (task->clean) 
    {
        task->clean(task->cleanParam);
    }

    // 如果删除的是自己，那么需要切换至另一个任务，所以执行一次任务调度
    if (currentTask == task) 
    {
        tTaskSched();
    }

    // 退出临界区
    tTaskExitCritical(status); 
}

/**********************************************************************************************************
** Function name        :   tTaskRequestDelete
** Descriptions         :   请求删除某个任务，由任务自己决定是否删除自己
** parameters           :   task  需要删除的任务
** Returned value       :   无
***********************************************************************************************************/
void tTaskRequestDelete (tTask * task)
{
   // 进入临界区
    uint32_t status = tTaskEnterCritical();

    // 设置清除删除标记
    task->requestDeleteFlag = 1;

    // 退出临界区
    tTaskExitCritical(status); 
}

/**********************************************************************************************************
** Function name        :   tTaskIsRequestedDelete
** Descriptions         :   是否已经被请求删除自己
** parameters           :   无
** Returned value       :   非0表示请求删除，0表示无请求
***********************************************************************************************************/
uint8_t tTaskIsRequestedDelete (void)
{
    uint8_t delete;

   // 进入临界区
    uint32_t status = tTaskEnterCritical();

    // 获取请求删除标记
    delete = currentTask->requestDeleteFlag;

    // 退出临界区
    tTaskExitCritical(status); 

    return delete;
}

/**********************************************************************************************************
** Function name        :   tTaskDeleteSelf
** Descriptions         :   删除自己
** parameters           :   无
** Returned value       :   无
***********************************************************************************************************/
void tTaskDeleteSelf (void)
{
    // 进入临界区
    uint32_t status = tTaskEnterCritical();

    // 任务在调用该函数时，必须是处于就绪状态，不可能处于延时或挂起等其它状态
    // 所以，只需要从就绪队列中移除即可
    tTaskSchedRemove(currentTask);

    // 删除时，如果有设置清理函数，则调用清理函数
    if (currentTask->clean)
    {
        currentTask->clean(currentTask->cleanParam);
    }

    // 接下来，肯定是切换到其它任务去运行
    tTaskSched();

    // 退出临界区
    tTaskExitCritical(status); 
}

/**********************************************************************************************************
** Function name        :   tTaskGetInfo
** Descriptions         :   获取任务相关信息
** parameters           :   task 需要查询的任务
** parameters           :   info 任务信息存储结构
** Returned value       :   无
***********************************************************************************************************/
void tTaskGetInfo (tTask * task, tTaskInfo * info)
{
    uint32_t * stackEnd;

    // 进入临界区
    uint32_t status = tTaskEnterCritical();

    info->delayTicks = task->delayTicks;                // 延时信息
    info->prio = task->prio;                            // 任务优先级
    info->state = task->state;                          // 任务状态
    info->slice = task->slice;                          // 剩余时间片
    info->suspendCount = task->suspendCount;            // 被挂起的次数
    info->stackSize = task->stackSize;

    // 计算堆栈使用量
    info->stackFree = 0;
    stackEnd = task->stackBase;
    while ((*stackEnd++ == 0) && (stackEnd <= task->stackBase + task->stackSize / sizeof(tTaskStack)))
    {
        info->stackFree++;
    }

    // 转换成字节数
    info->stackFree *= sizeof(tTaskStack);

    // 退出临界区
    tTaskExitCritical(status); 
}

