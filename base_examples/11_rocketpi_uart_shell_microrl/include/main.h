/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-05 14:26:57
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-07 15:33:02
 * @FilePath: /11_rocketpi_uart_shell_microrl/include/main.h
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

extern void Error_Handler(void);

#define LED_B_Pin GPIO_PIN_1
#define LED_B_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_10
#define LED_G_GPIO_Port GPIOB
#define LED_P_Pin GPIO_PIN_14
#define LED_P_GPIO_Port GPIOB

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
