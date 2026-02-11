/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 17:39:06
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 17:56:25
 * @FilePath: /16_rocketpi_pwm_passive_buzzer/src/tim.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "tim.h"

TIM_HandleTypeDef htim3;

/* TIM3 init function */
void MY_TIM3_Init(void) {
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  // 370*84/84MHz=370us≈2.7kHz
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 84 - 1;               //时钟预分频:84MHz/84=1MHz
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP; //向上计数
  htim3.Init.Period = 369;                     //自动重装载值370us
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1; // PWM模式1
  sConfigOC.Pulse = 185; //比较值，即占空比计数值,185/370=50%
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim3);
}

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *tim_pwmHandle) {

  if (tim_pwmHandle->Instance == TIM3) {
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *timHandle) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (timHandle->Instance == TIM3) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PB0     ------> TIM3_CH3
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
}

void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *tim_pwmHandle) {

  if (tim_pwmHandle->Instance == TIM3) {
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();
  }
}
