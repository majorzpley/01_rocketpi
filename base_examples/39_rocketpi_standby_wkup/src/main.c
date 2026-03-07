/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-07 11:30:38
 * @FilePath: /39_rocketpi_standby_wkup/src/main.c
 * @Description:此工程为rocketpi低功耗模式standby演示工程
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"

static void SystemClock_Config(void);
static void EnterStandbyMode(void);
static void SetAllLeds(GPIO_PinState state);
static void EnableAllPeripheralClocks(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  /*
   * 打开所有可用外设时钟，让普通工作模式的功耗尽可能大
   * 这里的功耗大不了多少，只有当实际配置并让这些外设工作电流才会尽可能的大
   */
  EnableAllPeripheralClocks();

  /* 判断当前是否是从 Standby 模式唤醒 */
  uint8_t woke_from_standby = (__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET);

  /* 处理唤醒后的流程 */
  if (woke_from_standby) {
    // 如果从standby模式唤醒
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
    __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
    SetAllLeds(GPIO_PIN_RESET); //打开LED
  } else {
    SetAllLeds(GPIO_PIN_SET); //关闭LED
  }

  /* 延迟 3 秒后再进入 Standby，方便观察与退出 */
  HAL_Delay(3000);
  EnterStandbyMode();

  while (1) {
  }
}

static void SystemClock_Config(void) {
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
  RCC_OscInitStruct.PLL.PLLQ = 7;             //* USB时钟分频系数
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

static void EnterStandbyMode(void) {
  /* 进入 Standby 前先清除旧的唤醒标志并重新使能 PA0 为唤醒源 */
  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1); //禁用唤醒引脚1
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);         //清除唤醒标志
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1); //使能唤醒引脚1，配置PA作为唤醒源
  HAL_PWR_EnterSTANDBYMode();               //进入待机模式
}

static void SetAllLeds(GPIO_PinState state) {
  /* 同步控制三色 LED，便于统一显示状态 */
  HAL_GPIO_WritePin(LED_B_GPIO_Port, LED_B_Pin, state);
  HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, state);
  HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, state);
}

static void EnableAllPeripheralClocks(void) {
  /* AHB1 总线：GPIO、DMA 等 */
#if defined(__HAL_RCC_GPIOA_CLK_ENABLE)
  __HAL_RCC_GPIOA_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOB_CLK_ENABLE)
  __HAL_RCC_GPIOB_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOC_CLK_ENABLE)
  __HAL_RCC_GPIOC_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOD_CLK_ENABLE)
  __HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOE_CLK_ENABLE)
  __HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_GPIOH_CLK_ENABLE)
  __HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_DMA1_CLK_ENABLE)
  __HAL_RCC_DMA1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_DMA2_CLK_ENABLE)
  __HAL_RCC_DMA2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_CRC_CLK_ENABLE)
  __HAL_RCC_CRC_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_BKPSRAM_CLK_ENABLE)
  __HAL_RCC_BKPSRAM_CLK_ENABLE();
#endif

  /* AHB2 总线：USB OTG、RNG 等 */
#if defined(__HAL_RCC_USB_OTG_FS_CLK_ENABLE)
  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_RNG_CLK_ENABLE)
  __HAL_RCC_RNG_CLK_ENABLE();
#endif

  /* APB1 总线：定时器、串口、I2C 等 */
#if defined(__HAL_RCC_TIM2_CLK_ENABLE)
  __HAL_RCC_TIM2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM3_CLK_ENABLE)
  __HAL_RCC_TIM3_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM4_CLK_ENABLE)
  __HAL_RCC_TIM4_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM5_CLK_ENABLE)
  __HAL_RCC_TIM5_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM6_CLK_ENABLE)
  __HAL_RCC_TIM6_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM7_CLK_ENABLE)
  __HAL_RCC_TIM7_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SPI2_CLK_ENABLE)
  __HAL_RCC_SPI2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SPI3_CLK_ENABLE)
  __HAL_RCC_SPI3_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_I2C1_CLK_ENABLE)
  __HAL_RCC_I2C1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_I2C2_CLK_ENABLE)
  __HAL_RCC_I2C2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_I2C3_CLK_ENABLE)
  __HAL_RCC_I2C3_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_USART2_CLK_ENABLE)
  __HAL_RCC_USART2_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_USART3_CLK_ENABLE)
  __HAL_RCC_USART3_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_UART4_CLK_ENABLE)
  __HAL_RCC_UART4_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_UART5_CLK_ENABLE)
  __HAL_RCC_UART5_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_PWR_CLK_ENABLE)
  __HAL_RCC_PWR_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_DAC_CLK_ENABLE)
  __HAL_RCC_DAC_CLK_ENABLE();
#endif

  /* APB2 总线：高速外设 */
#if defined(__HAL_RCC_TIM1_CLK_ENABLE)
  __HAL_RCC_TIM1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM9_CLK_ENABLE)
  __HAL_RCC_TIM9_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM10_CLK_ENABLE)
  __HAL_RCC_TIM10_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_TIM11_CLK_ENABLE)
  __HAL_RCC_TIM11_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_ADC1_CLK_ENABLE)
  __HAL_RCC_ADC1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SDIO_CLK_ENABLE)
  __HAL_RCC_SDIO_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SPI1_CLK_ENABLE)
  __HAL_RCC_SPI1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SPI4_CLK_ENABLE)
  __HAL_RCC_SPI4_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_SYSCFG_CLK_ENABLE)
  __HAL_RCC_SYSCFG_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_USART1_CLK_ENABLE)
  __HAL_RCC_USART1_CLK_ENABLE();
#endif
#if defined(__HAL_RCC_USART6_CLK_ENABLE)
  __HAL_RCC_USART6_CLK_ENABLE();
#endif
}