/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 14:55:34
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 15:45:57
 * @FilePath: /15_rocketpi_i2c_at24cxx/include/main.h
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

#define AT24CXX_SCL_Pin GPIO_PIN_14
#define AT24CXX_SCL_GPIO_Port GPIOC
#define AT24CXX_SDA_Pin GPIO_PIN_15
#define AT24CXX_SDA_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
