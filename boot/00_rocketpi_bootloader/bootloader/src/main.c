/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-08 21:37:07
 * @FilePath: /00_rocketpi_bootloader/bootloader/src/main.c
 * @Description:此工程为rocketpi bootloader工程
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"

#include <stdbool.h>

#define APP_START_ADDRESS (0x08020000UL)
#define APP_MAX_SIZE_BYTES (0x60000UL)
#define APP_END_ADDRESS (APP_START_ADDRESS + APP_MAX_SIZE_BYTES)
#define SRAM_START_ADDRESS (0x20000000UL)
#define SRAM_SIZE_BYTES (96UL * 1024UL)
#define SRAM_END_ADDRESS (SRAM_START_ADDRESS + SRAM_SIZE_BYTES)

typedef void (*AppEntryFunc)(void);

static void SystemClock_Config(void);
static bool Bootloader_IsApplicationValid(void);
static void Bootloader_JumpToApplication(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();

  HAL_Delay(3000);

  if (Bootloader_IsApplicationValid()) {
    Bootloader_JumpToApplication();
  }

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

static bool Bootloader_IsApplicationValid(void) {
  uint32_t appStackPointer = *(__IO uint32_t *)(APP_START_ADDRESS);
  uint32_t appResetHandler = *(__IO uint32_t *)(APP_START_ADDRESS + 4U);

  bool stackPointerInRange = (appStackPointer >= SRAM_START_ADDRESS) &&
                             (appStackPointer <= SRAM_END_ADDRESS);
  bool resetHandlerInRange = (appResetHandler >= APP_START_ADDRESS) &&
                             (appResetHandler <= APP_END_ADDRESS);

  return stackPointerInRange && resetHandlerInRange;
}

static void Bootloader_JumpToApplication(void) {
  // todo 从应用程序起始地址读取栈指针（MSP）值
  uint32_t appStackPointer = *(__IO uint32_t *)(APP_START_ADDRESS);
  // todo 读取复位向量地址（向量表第二个字）
  uint32_t appResetHandler = *(__IO uint32_t *)(APP_START_ADDRESS + 4U);
  // todo 将复位向量转换为函数指针
  AppEntryFunc application = (AppEntryFunc)appResetHandler;

  __disable_irq();            // 关闭全局中断，防止跳转过程中断
  HAL_RCC_DeInit();           // 复位RCC时钟配置
  HAL_DeInit();               // 复位HAL库外设
  SysTick->CTRL = 0U;         // 关闭SysTick定时器
  SysTick->LOAD = 0U;         // 清零重载值
  SysTick->VAL = 0U;          // 清零当前值
  __set_MSP(appStackPointer); // 设置主栈指针为应用程序的栈指针
  SCB->VTOR = APP_START_ADDRESS; // 将向量表偏移寄存器指向应用程序
  __enable_irq();                // 重新使能中断

  application(); // 跳转到应用程序的复位处理函数
}