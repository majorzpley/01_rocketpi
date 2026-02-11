/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:30:12
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-01-30 15:01:05
 * @FilePath: /00_rocketpi_template/src/stm32f4xx_hal_msp.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "main.h"
void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}