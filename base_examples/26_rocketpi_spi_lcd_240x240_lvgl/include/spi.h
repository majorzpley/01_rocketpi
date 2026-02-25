/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-17 15:57:13
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-17 15:57:27
 * @FilePath: /26_rocketpi_spi_lcd_240x240_lvgl/include/spi.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SPI_HandleTypeDef hspi1;

extern void MY_SPI1_Init(void);

#ifdef __cplusplus
}
#endif