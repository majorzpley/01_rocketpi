/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-27 10:58:22
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-27 10:58:31
 * @FilePath: /06_rocketpi_mqtt_ota/app/src/stm32f4xx_hal_msp.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "main.h"
void HAL_MspInit(void) {
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
}
