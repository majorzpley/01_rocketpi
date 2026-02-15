/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-15 21:03:21
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-15 21:03:56
 * @FilePath: /23_rocketpi_ws2812b/include/tim.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern TIM_HandleTypeDef htim3;

extern void MY_TIM3_Init(void);

extern void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif