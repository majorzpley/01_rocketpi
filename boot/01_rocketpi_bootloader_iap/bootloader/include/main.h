/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-09 15:27:44
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-09 20:17:29
 * @FilePath: /01_rocketpi_bootloader_iap/bootloader/include/main.h
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

#define key_Pin GPIO_PIN_0
#define key_GPIO_Port GPIOA

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
