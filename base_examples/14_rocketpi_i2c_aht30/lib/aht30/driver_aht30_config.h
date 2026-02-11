/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 14:17:01
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 14:41:43
 * @FilePath: /14_rocketpi_i2c_aht30/lib/aht30/driver_aht30_config.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include "main.h"

/**
 * @file driver_aht30_config.h
 * @brief 通过宏控制AHT30驱动所使用的I2C实现以及GPIO定义。
 */

#ifndef AHT30_USE_SOFT_I2C
#define AHT30_USE_SOFT_I2C 1 /**< 默认启用软件I2C。设为0可切换回HAL硬件I2C。 */
#endif

#if AHT30_USE_SOFT_I2C

#ifndef AHT30_SOFT_SCL_PORT
#define AHT30_SOFT_SCL_PORT AHT30_SCL_GPIO_Port
#endif

#ifndef AHT30_SOFT_SCL_PIN
#define AHT30_SOFT_SCL_PIN AHT30_SCL_Pin
#endif

#ifndef AHT30_SOFT_SDA_PORT
#define AHT30_SOFT_SDA_PORT AHT30_SDA_GPIO_Port
#endif

#ifndef AHT30_SOFT_SDA_PIN
#define AHT30_SOFT_SDA_PIN AHT30_SDA_Pin
#endif

#ifndef AHT30_SOFT_DELAY_TICKS
#define AHT30_SOFT_DELAY_TICKS 80U
#endif

#ifndef AHT30_SOFT_STRETCH_TICKS
#define AHT30_SOFT_STRETCH_TICKS 8000U
#endif

#else

#include "i2c.h"

#ifndef AHT30_I2C_HANDLE
#define AHT30_I2C_HANDLE hi2c1
#endif

#endif /* AHT30_USE_SOFT_I2C */