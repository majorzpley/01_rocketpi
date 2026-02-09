/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-05 14:02:14
 * @FilePath: /07_rocketpi_uart_echo/src/main.c
 * @Description:串口dma终端实验
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "usart.h"
#include <string.h>

#define UART_DMA_BUFFER_SIZE 128U /* 单次回显缓存大小 */

extern void SystemClock_Config(void);

static HAL_StatusTypeDef uart_restart_rx_dma(void);
static void uart_send_string(const char *msg);
static void uart_send_banner(void);

static uint8_t g_uart_rx_buffer[UART_DMA_BUFFER_SIZE];
static uint8_t g_uart_tx_buffer[UART_DMA_BUFFER_SIZE];

/* 重新启动串口空闲中断 DMA 接收，始终准备下一帧数据 */
static HAL_StatusTypeDef uart_restart_rx_dma(void) {
  HAL_StatusTypeDef status = HAL_UARTEx_ReceiveToIdle_DMA(
      &huart2, g_uart_rx_buffer, UART_DMA_BUFFER_SIZE);
  if (status == HAL_OK) {
    if (huart2.hdmarx != NULL) {
      __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
    }
  }
  return status;
}

/* 阻塞方式发送字符串，主要用于提示和回显 */
static void uart_send_string(const char *msg) {
  if ((msg == NULL) || (*msg == '\0')) {
    return;
  }
  const size_t length = strlen(msg);
  if (length == 0U) {
    return;
  }
  if (HAL_UART_Transmit(&huart2, (uint8_t *)msg, (uint16_t)length,
                        HAL_MAX_DELAY) != HAL_OK) {
    Error_Handler();
  }
}

/* 打印启动提示，告知用户当前示例用途 */
static void uart_send_banner(void) {
  uart_send_string("\r\nRocketPi UART echo ready.\r\n");
  uart_send_string("Type any text and it will be echoed back.\r\n> ");
}

/* 串口接收到一帧数据后立即回显，并重新打开 DMA 接收 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
  if ((huart != &huart2) || (Size == 0U)) {
    return;
  }

  const uint16_t copy_size =
      (Size <= UART_DMA_BUFFER_SIZE) ? Size : UART_DMA_BUFFER_SIZE;
  memcpy(g_uart_tx_buffer, g_uart_rx_buffer, copy_size);

  if (uart_restart_rx_dma() != HAL_OK) {
    Error_Handler();
  }

  if (HAL_UART_Transmit(&huart2, g_uart_tx_buffer, copy_size, HAL_MAX_DELAY) !=
      HAL_OK) {
    Error_Handler();
  }
}

/* 若发生串口错误则重新启动 DMA 接收，保证示例持续运行 */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if (huart != &huart2) {
    return;
  }

  if (uart_restart_rx_dma() != HAL_OK) {
    Error_Handler();
  }
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_USART2_UART_Init();
  /* 上电后先启动 DMA 接收，再输出提示信息 */
  if (uart_restart_rx_dma() != HAL_OK) {
    Error_Handler();
  }
  uart_send_banner();
  while (1) {
    HAL_Delay(1);
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