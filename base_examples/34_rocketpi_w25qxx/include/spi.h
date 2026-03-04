/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-04 15:52:56
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-04 15:53:49
 * @FilePath: /34_rocketpi_w25qxx/include/spi.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SPI_HandleTypeDef hspi2;

extern void MY_SPI2_Init(void);

#ifdef __cplusplus
}
#endif