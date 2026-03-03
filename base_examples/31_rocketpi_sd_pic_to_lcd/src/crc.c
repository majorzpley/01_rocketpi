/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-03 12:23:00
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-03 12:28:08
 * @FilePath: /31_rocketpi_sd_pic_to_lcd/src/crc.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "crc.h"

CRC_HandleTypeDef hcrc;

/* CRC init function */
void MY_CRC_Init(void) {
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_CRC_MspInit(CRC_HandleTypeDef *crcHandle) {

  if (crcHandle->Instance == CRC) {
    /* CRC clock enable */
    __HAL_RCC_CRC_CLK_ENABLE();
  }
}

void HAL_CRC_MspDeInit(CRC_HandleTypeDef *crcHandle) {

  if (crcHandle->Instance == CRC) {
    /* Peripheral clock disable */
    __HAL_RCC_CRC_CLK_DISABLE();
  }
}
