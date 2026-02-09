/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-02 18:09:25
 * @FilePath: /04_rocketpi_key_multi_button/src/main.c
 * @Description:此工程为rocketpi key按键移植MultiButton测试
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"
#include "multi_button.h"

#define USER_BUTTON_ACTIVE_LEVEL GPIO_PIN_SET

extern void SystemClock_Config(void);
extern volatile uint8_t key_flag;
static uint8_t ButtonPinLevel(uint8_t button_id);
static void ButtonSingleClickHandler(Button *btn);
static void ButtonDoubleClickHandler(Button *btn);
static void ButtonLongPressHandler(Button *btn);

static Button g_user_button;

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  button_init(&g_user_button, ButtonPinLevel, USER_BUTTON_ACTIVE_LEVEL, 0);
  button_attach(&g_user_button, BTN_SINGLE_CLICK, ButtonSingleClickHandler);
  button_attach(&g_user_button, BTN_DOUBLE_CLICK, ButtonDoubleClickHandler);
  button_attach(&g_user_button, BTN_LONG_PRESS_START, ButtonLongPressHandler);
  button_start(&g_user_button);
  while (1) {
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // 初始化晶振参数结构体
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // 初始化时钟参数结构体

  __HAL_RCC_PWR_CLK_ENABLE(); // 使能电源控制时钟
  __HAL_PWR_VOLTAGESCALING_CONFIG(
      PWR_REGULATOR_VOLTAGE_SCALE2); // 配置电压调节器电压缩放

  // 初始化HSE晶振和PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource =
      RCC_PLLSOURCE_HSE;           //* 使用外部晶振HSE作为PLL输入源
  RCC_OscInitStruct.PLL.PLLM = 4;  //* 分频系数
  RCC_OscInitStruct.PLL.PLLN = 84; //* 倍频系数
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; //* 主系统时钟分频系数
  RCC_OscInitStruct.PLL.PLLQ = 4;             //* USB时钟分频系数
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  // 初始化系统时钟
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2; // 配置时钟类型
  RCC_ClkInitStruct.SYSCLKSource =
      RCC_SYSCLKSOURCE_PLLCLK; //* 使用PLL输出作为系统时钟源
  RCC_ClkInitStruct.AHBCLKDivider =
      RCC_SYSCLK_DIV1; //* AHB时钟不分频，高性能总线用于CPU、内存、DMA等，影响CPU执行速度、DMA传输速度
  RCC_ClkInitStruct.APB1CLKDivider =
      RCC_HCLK_DIV2; //* APB1时钟分频2，低速总线用于定时器、UART、I2C、SPI等
  RCC_ClkInitStruct.APB2CLKDivider =
      RCC_HCLK_DIV1; //* APB2时钟不分频，高速总线用于ADC、USART、高级定时器等

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) !=
      HAL_OK) { //* 设置FLASH延时周期
    Error_Handler();
  }
}
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */

static uint8_t ButtonPinLevel(uint8_t button_id) {
  (void)button_id;
  return (uint8_t)HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY_Pin);
}

static void ButtonSingleClickHandler(Button *btn) {
  (void)btn;
  HAL_GPIO_TogglePin(LED_B_GPIO_Port, LED_B_Pin);
}

static void ButtonDoubleClickHandler(Button *btn) {
  (void)btn;
  HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
}

static void ButtonLongPressHandler(Button *btn) {
  (void)btn;
  HAL_GPIO_TogglePin(LED_P_GPIO_Port, LED_P_Pin);
}