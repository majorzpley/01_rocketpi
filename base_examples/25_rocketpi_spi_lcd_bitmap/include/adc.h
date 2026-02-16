/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-16 12:26:49
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-16 12:27:56
 * @FilePath: /25_rocketpi_spi_lcd_bitmap/include/adc.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern ADC_HandleTypeDef hadc1;

extern void MY_ADC1_Init(void);

#ifdef __cplusplus
}
#endif