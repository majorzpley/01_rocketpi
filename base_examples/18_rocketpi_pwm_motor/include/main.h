/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 15:04:57
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 21:19:18
 * @FilePath: /18_rocketpi_pwm_motor/include/main.h
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

#define LED_R_Pin GPIO_PIN_1
#define LED_R_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define LED_G_Pin GPIO_PIN_10
#define LED_G_GPIO_Port GPIOB
#define LED_B_Pin GPIO_PIN_14
#define LED_B_GPIO_Port GPIOB
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define MOTOR_INB_Pin GPIO_PIN_6
#define MOTOR_INB_GPIO_Port GPIOB
#define MOTOR_INA_Pin GPIO_PIN_7
#define MOTOR_INA_GPIO_Port GPIOB

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
