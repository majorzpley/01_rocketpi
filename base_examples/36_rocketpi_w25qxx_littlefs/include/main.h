/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 15:15:44
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-05 12:06:09
 * @FilePath: /36_rocketpi_w25qxx_littlefs/include/main.h
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

#define W25QXX_MISO_Pin GPIO_PIN_2
#define W25QXX_MISO_GPIO_Port GPIOC
#define LED_R_Pin GPIO_PIN_1
#define LED_R_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_10
#define LED_G_GPIO_Port GPIOB
#define W25QXX_CS_Pin GPIO_PIN_12
#define W25QXX_CS_GPIO_Port GPIOB
#define W25QXX_SCK_Pin GPIO_PIN_13
#define W25QXX_SCK_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_14
#define LED_B_GPIO_Port GPIOB
#define W25QXX_MOSI_Pin GPIO_PIN_15
#define W25QXX_MOSI_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
