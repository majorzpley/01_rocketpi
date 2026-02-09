/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-02 22:11:08
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-03 08:46:21
 * @FilePath: /05_rocketpi_delay_us/src/tim.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "tim.h"

TIM_HandleTypeDef htim11;

/* TIM11 init function */
void MY_TIM11_Init(void) {

  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 84 - 1; // 1MHz计数频率，1us计数一次
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;       // 向上计数模式
  htim11.Init.Period = 65535;                         // 最大计数值
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1; // 不分频
  htim11.Init.AutoReloadPreload =
      TIM_AUTORELOAD_PRELOAD_DISABLE; // 自动重装载预装载禁用
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK) {
    Error_Handler();
  }
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *tim_baseHandle) {

  if (tim_baseHandle->Instance == TIM11) {
    __HAL_RCC_TIM11_CLK_ENABLE();
  }
}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *tim_baseHandle) {

  if (tim_baseHandle->Instance == TIM11) {
    __HAL_RCC_TIM11_CLK_DISABLE();
  }
}
