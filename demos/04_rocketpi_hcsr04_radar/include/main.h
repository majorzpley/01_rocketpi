/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-16 14:03:48
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-16 14:13:25
 * @FilePath: /04_rocketpi_hcsr04_radar/include/main.h
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

#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define SG90_IO_Pin GPIO_PIN_9
#define SG90_IO_GPIO_Port GPIOC
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TRIGGER_Pin GPIO_PIN_10
#define TRIGGER_GPIO_Port GPIOC
#define ECHO_Pin GPIO_PIN_11
#define ECHO_GPIO_Port GPIOC

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
