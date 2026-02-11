/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 15:35:21
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 12:03:18
 * @FilePath: /14_rocketpi_i2c_aht30/src/gpio.c
 * /05_rocketpi_delay_us/home/majorzpley/01_project_manager/02_pio_projects/00_rocketpi_template/src/gpio.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "gpio.h"

void MY_GPIO_Init(void) {
  __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
  __HAL_RCC_GPIOH_CLK_ENABLE(); // 使能GPIOH时钟
  __HAL_RCC_GPIOB_CLK_ENABLE(); // 使能GPIOB时钟
}