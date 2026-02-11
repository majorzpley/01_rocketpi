/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 15:35:21
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 15:47:57
 * @FilePath: /15_rocketpi_i2c_at24cxx/src/gpio.c
 * /05_rocketpi_delay_us/home/majorzpley/01_project_manager/02_pio_projects/00_rocketpi_template/src/gpio.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "gpio.h"

void MY_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  __HAL_RCC_GPIOA_CLK_ENABLE(); // 使能GPIOA时钟
  __HAL_RCC_GPIOH_CLK_ENABLE(); // 使能GPIOH时钟
  __HAL_RCC_GPIOC_CLK_ENABLE(); // 使能GPIOC时钟

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, AT24CXX_SCL_Pin | AT24CXX_SDA_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : AT24CXX_SCL_Pin AT24CXX_SDA_Pin */
  GPIO_InitStruct.Pin = AT24CXX_SCL_Pin | AT24CXX_SDA_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}