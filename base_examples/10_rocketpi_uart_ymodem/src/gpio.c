/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 15:35:21
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-06 19:09:05
 * @FilePath: /10_rocketpi_uart_ymodem/src/gpio.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "gpio.h"

void MY_GPIO_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
  __HAL_RCC_GPIOH_CLK_ENABLE(); // 使能GPIOH时钟
}