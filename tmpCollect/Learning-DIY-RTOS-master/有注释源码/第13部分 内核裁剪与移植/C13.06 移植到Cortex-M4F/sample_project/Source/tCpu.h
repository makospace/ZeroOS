/*************************************** Copyright (c)******************************************************
** File name            :   tCPU.h
** Latest modified Date :   2016-06-01
** Latest Version       :   0.1
** Descriptions         :   tinyOS的处理器接口相关实现
**
**--------------------------------------------------------------------------------------------------------
** Created by           :   01课堂 lishutong
** Created date         :   2020-01-01
** Version              :   1.0
** Descriptions         :   The original version
**
**--------------------------------------------------------------------------------------------------------
** Copyright            :   版权所有，禁止用于商业用途
** Author Blog          :   http://ilishutong.com
**********************************************************************************************************/
#ifndef TCPU_H
#define TCPU_H

#ifdef __TARGET_CPU_CORTEX_M0
	#include "ARMCM0.h" 
#elif defined(__TARGET_CPU_CORTEX_M3)
	#include "ARMCM3.h" 
#elif defined(__TARGET_CPU_CORTEX_M4)
	#include "ARMCM4.h" 
#elif defined(__TARGET_CPU_CORTEX_M4FP)
	#include "ARMCM4_FP.h" 
#endif

#endif

