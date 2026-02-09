/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-02 22:11:29
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-02 22:12:03
 * @FilePath: /05_rocketpi_delay_us/include/tim.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"


extern TIM_HandleTypeDef htim11;


extern void MY_TIM11_Init(void);


#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */