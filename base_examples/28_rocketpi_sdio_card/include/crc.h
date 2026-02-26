/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-26 12:45:26
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-26 12:46:01
 * @FilePath: /28_rocketpi_sdio_card/include/crc.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern CRC_HandleTypeDef hcrc;

extern void MY_CRC_Init(void);

#ifdef __cplusplus
}
#endif