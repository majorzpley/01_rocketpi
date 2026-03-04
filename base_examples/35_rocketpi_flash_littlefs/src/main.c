/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-04 22:11:48
 * @FilePath: /35_rocketpi_flash_littlefs/src/main.c
 * @Description:此工程为rocketpi使用内部的flash后两个扇区做littlefs文件系统
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"
#include "usart.h"

#include <stdio.h>
#include <string.h>

#include "debug_driver.h"
#include "lfs.h"
#include "lfs_port.h"

static lfs_t g_lfs;

static void SystemClock_Config(void);
static void LittleFS_Test(void);
static void LittleFS_Log(const char *msg);
static void LittleFS_LogError(const char *msg, int err);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_USART2_UART_Init();

  LittleFS_Test();

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

static void LittleFS_Test(void) {
  struct lfs_file file;
  char read_buffer[64] = {0};
  const char *path = "flash.txt";
  const char *payload = "LittleFS STM32F401 Flash Demo\r\n";

  int err = lfs_port_mount(&g_lfs);
  if (err != LFS_ERR_OK) {
    LittleFS_LogError("LittleFS: initial mount failed", err);
    LittleFS_Log("LittleFS: formatting flash...\r\n");
    err = lfs_port_format(&g_lfs);
    if (err == LFS_ERR_OK) {
      LittleFS_Log("LittleFS: format done\r\n");
      err = lfs_port_mount(&g_lfs);
    } else {
      LittleFS_LogError("LittleFS: format failed", err);
    }
  }

  if (err != LFS_ERR_OK) {
    LittleFS_LogError("LittleFS: mount failed", err);
    return;
  }

  err = lfs_file_open(&g_lfs, &file, path,
                      LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
  if (err < 0) {
    LittleFS_LogError("LittleFS: open for write failed", err);
    lfs_port_unmount(&g_lfs);
    return;
  }

  lfs_ssize_t written = lfs_file_write(&g_lfs, &file, payload, strlen(payload));
  lfs_file_close(&g_lfs, &file);

  if (written < 0) {
    LittleFS_LogError("LittleFS: write failed", (int)written);
    lfs_port_unmount(&g_lfs);
    return;
  }

  err = lfs_file_open(&g_lfs, &file, path, LFS_O_RDONLY);
  if (err < 0) {
    LittleFS_LogError("LittleFS: open for read failed", err);
    lfs_port_unmount(&g_lfs);
    return;
  }

  lfs_ssize_t read =
      lfs_file_read(&g_lfs, &file, read_buffer, sizeof(read_buffer) - 1U);
  lfs_file_close(&g_lfs, &file);

  if (read < 0) {
    LittleFS_LogError("LittleFS: read failed", (int)read);
    lfs_port_unmount(&g_lfs);
    return;
  }

  read_buffer[read] = '\0';

  char log_buffer[128];
  int len =
      snprintf(log_buffer, sizeof(log_buffer),
               "LittleFS OK (%ld bytes): %s\r\n", (long)read, read_buffer);
  if ((len > 0) && (len < (int)sizeof(log_buffer))) {
    LittleFS_Log(log_buffer);
  }

  lfs_port_unmount(&g_lfs);
}

static void LittleFS_Log(const char *msg) {
  if (msg == NULL) {
    return;
  }
  HAL_UART_Transmit(&huart2, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

static void LittleFS_LogError(const char *msg, int err) {
  char buffer[96];
  int len = snprintf(buffer, sizeof(buffer), "%s (err=%d)\r\n", msg, err);
  if ((len > 0) && (len < (int)sizeof(buffer))) {
    LittleFS_Log(buffer);
  }
}