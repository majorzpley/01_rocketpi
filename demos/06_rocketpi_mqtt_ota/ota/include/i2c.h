/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-27 12:20:38
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-27 12:21:11
 * @FilePath: /06_rocketpi_mqtt_ota/ota/include/i2c.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern I2C_HandleTypeDef hi2c1;

extern void MY_I2C1_Init(void);

#ifdef __cplusplus
}
#endif