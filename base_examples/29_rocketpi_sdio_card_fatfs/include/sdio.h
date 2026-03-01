/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-27 11:36:50
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-28 23:10:19
 * @FilePath: /29_rocketpi_sdio_card_fatfs/include/sdio.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern SD_HandleTypeDef hsd;

extern void MY_SDIO_SD_Init(void);

#ifdef __cplusplus
}
#endif