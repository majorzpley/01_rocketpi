/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-25 21:55:20
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-25 21:55:29
 * @FilePath: /27_rocketpi_i2s/src/dma.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "dma.h"

void MY_DMA_Init(void) {

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
}