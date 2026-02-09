/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-05 12:30:48
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-05 14:05:17
 * @FilePath: /07_rocketpi_uart_echo/src/dma.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "dma.h"

void MY_DMA_Init(void) {
  // Enable DMA controller clock
  __HAL_RCC_DMA1_CLK_ENABLE();
  // DMA1_Stream5_IRQn interrupt configuration
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  // DMA1_Stream6_IRQn interrupt configuration
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
}