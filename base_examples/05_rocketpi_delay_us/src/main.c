/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-04 21:36:39
 * @FilePath: /05_rocketpi_delay_us/src/main.c
 * @Description:此工程为rocketpi实现us级精确延时，使用PC10管脚测试
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"
#include "tim.h"

// #define USE_HAL
// #define USE_REG
#define USE_ASM

extern void SystemClock_Config(void);
extern uint16_t micros16(void);
extern void delay_us(uint16_t us);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_TIM11_Init();
  HAL_TIM_Base_Start(&htim11); // 启动定时器11
  while (1) {
#ifdef USE_HAL
    // todo 使用HAL库操作PC10引脚，每隔1us翻转一次状态
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_10);
    delay_us(1);
    //! 实际测得时间：950ns到1.1us之间
#elif defined USE_REG
    // todo 直接操作寄存器操作PC10引脚，每隔1us翻转一次状态
    // GPIOC->ODR ^= (1 << 10);
    // delay_us(1);
    //! 实际测得时间：貌似是在1.1us->950ns->1us循环变化，和HAL库相比变化不大
#elif defined USE_ASM
    // todo 使用内联汇编操作寄存器每隔1us翻转PC10
    /*
     *汇编代码说明：
     * ldr r0, =0x40020814  ; 将GPIOC的ODR寄存器地址加载到r0寄存器
     * ldr r1, [r0]         ; 从ODR寄存器读取当前值到r1寄存器
     * eor r1, r1, #0x400    ;
     *对r1寄存器的值进行异或操作，翻转PC10位（0x400对应第10位） str r1, [r0] ;
     *将修改后的值写回ODR寄存器 这样就实现了对PC10引脚的翻转操作
     */
    __asm volatile("ldr r0, =0x40020814\n"
                   "ldr r1, [r0]\n"
                   "eor r1, r1, #0x400\n"
                   "str r1, [r0]\n"
                   :
                   :
                   : "r0", "r1");
    delay_us(1);
    //! 实际测得时间：刚好是1us
#endif
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
uint16_t micros16(void) { return __HAL_TIM_GET_COUNTER(&htim11); }

void delay_us(uint16_t us) {
  uint16_t start = micros16();
  while ((uint16_t)(micros16() - start) < us) {
    ;
  }
}