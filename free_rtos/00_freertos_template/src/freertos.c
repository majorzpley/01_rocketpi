/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-30 11:12:47
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-30 12:12:27
 * @FilePath: /00_freertos_template/src/freertos.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "main.h"
#include "task.h"

osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};
/* Definitions for LEDP_TASK */
osThreadId_t LEDP_TASKHandle;
const osThreadAttr_t LEDP_TASK_attributes = {
    .name = "LEDP_TASK",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for LEDG_TASK */
osThreadId_t LEDG_TASKHandle;
const osThreadAttr_t LEDG_TASK_attributes = {
    .name = "LEDG_TASK",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};
/* Definitions for LEDB_TASK */
osThreadId_t LEDB_TASKHandle;
const osThreadAttr_t LEDB_TASK_attributes = {
    .name = "LEDB_TASK",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityLow,
};

void StartDefaultTask(void *argument);
void ledp_task(void *argument);
void ledg_task(void *argument);
void ledb_task(void *argument);

void MY_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

void MY_FREERTOS_Init(void) {
  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle =
      osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of LEDP_TASK */
  LEDP_TASKHandle = osThreadNew(ledp_task, NULL, &LEDP_TASK_attributes);

  /* creation of LEDG_TASK */
  LEDG_TASKHandle = osThreadNew(ledg_task, NULL, &LEDG_TASK_attributes);

  /* creation of LEDB_TASK */
  LEDB_TASKHandle = osThreadNew(ledb_task, NULL, &LEDB_TASK_attributes);
}

void StartDefaultTask(void *argument) {
  /* Infinite loop */
  for (;;) {
    osDelay(1);
  }
}

/**
 * @brief Function implementing the LEDP_TASK thread.
 * @param argument: Not used
 * @retval None
 */
void ledp_task(void *argument) {
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(LED_P_GPIO_Port, LED_P_Pin);
    osDelay(500);
  }
}

/**
 * @brief Function implementing the LEDG_TASK thread.
 * @param argument: Not used
 * @retval None
 */
void ledg_task(void *argument) {
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
    osDelay(500);
  }
}

/**
 * @brief Function implementing the LEDB_TASK thread.
 * @param argument: Not used
 * @retval None
 */
void ledb_task(void *argument) {
  /* Infinite loop */
  for (;;) {
    HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
    osDelay(500);
  }
}