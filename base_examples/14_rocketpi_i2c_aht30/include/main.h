/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-09 14:51:00
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 11:49:56
 * @FilePath: /14_rocketpi_i2c_aht30/include/main.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define AHT30_SCL_Pin GPIO_PIN_8
#define AHT30_SCL_GPIO_Port GPIOB
#define AHT30_SDA_Pin GPIO_PIN_9
#define AHT30_SDA_GPIO_Port GPIOB

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
