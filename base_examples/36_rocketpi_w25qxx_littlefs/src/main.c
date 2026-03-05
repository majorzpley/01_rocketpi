/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-05 13:21:51
 * @FilePath: /36_rocketpi_w25qxx_littlefs/src/main.c
 * @Description:此工程为rocketpi同时将内部flash与外部norflash挂载为littlefs文件系统
 * 不用客气，这是你应该谢的! Copyright (c) 2026 by
 * ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "gpio.h"
#include "spi.h"
#include "usart.h"

#include <stdbool.h>
#include <string.h>

#include "debug_driver.h"
#include "lfs_flash_port.h"
#include "lfs_nor_flash_port.h"
#include "lfs_port.h"

#define LFS_NOR_FLASH_TOTAL_SIZE (8UL * 1024UL * 1024UL) /* W25Q64 = 8 MiB */

typedef struct {
  int (*format)(lfs_t *lfs);
  int (*mount)(lfs_t *lfs);
  int (*unmount)(lfs_t *lfs);
  const char *label;
} lfs_storage_ops_t;

static lfs_t g_lfs;

static void SystemClock_Config(void);
static void LittleFS_Test(bool use_nor_flash);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_USART2_UART_Init();
  MY_SPI2_Init();

  LittleFS_Test(false); /* internal Flash */
  LittleFS_Test(true);  /* external NOR Flash*/
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

static void LittleFS_Test(bool use_nor_flash) {
  struct lfs_file file;
  char read_buffer[64] = {0};
  const char *path = use_nor_flash ? "nor_flash.txt" : "int_flash.txt";
  const char *payload = use_nor_flash ? "LittleFS NOR Flash Demo\r\n"
                                      : "LittleFS Internal Flash Demo\r\n";

  const lfs_storage_ops_t ops =
      use_nor_flash
          ? (lfs_storage_ops_t){lfs_nor_flash_port_format,
                                lfs_nor_flash_port_mount,
                                lfs_nor_flash_port_unmount, "NOR"}
          : (lfs_storage_ops_t){lfs_flash_port_format, lfs_flash_port_mount,
                                lfs_flash_port_unmount, "INT"};

  int err = ops.mount(&g_lfs);
  if (err != LFS_ERR_OK) {
    uart_printf("[%s] mount failed (%d), formatting...\r\n", ops.label, err);
    err = ops.format(&g_lfs);
    if (err == LFS_ERR_OK) {
      uart_printf("[%s] format done\r\n", ops.label);
      err = ops.mount(&g_lfs);
    } else {
      uart_printf("[%s] format failed (%d)\r\n", ops.label, err);
    }
  }

  if (err != LFS_ERR_OK) {
    uart_printf("[%s] mount failed (%d)\r\n", ops.label, err);
    return;
  }

  err = lfs_file_open(&g_lfs, &file, path,
                      LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC);
  if (err < 0) {
    uart_printf("[%s] open for write failed (%d)\r\n", ops.label, err);
    ops.unmount(&g_lfs);
    return;
  }

  lfs_ssize_t written = lfs_file_write(&g_lfs, &file, payload, strlen(payload));
  lfs_file_close(&g_lfs, &file);

  if (written < 0) {
    uart_printf("[%s] write failed (%ld)\r\n", ops.label, (long)written);
    ops.unmount(&g_lfs);
    return;
  }

  err = lfs_file_open(&g_lfs, &file, path, LFS_O_RDONLY);
  if (err < 0) {
    uart_printf("[%s] open for read failed (%d)\r\n", ops.label, err);
    ops.unmount(&g_lfs);
    return;
  }

  lfs_ssize_t read =
      lfs_file_read(&g_lfs, &file, read_buffer, sizeof(read_buffer) - 1U);
  lfs_file_close(&g_lfs, &file);

  if (read < 0) {
    uart_printf("[%s] read failed (%ld)\r\n", ops.label, (long)read);
    ops.unmount(&g_lfs);
    return;
  }

  read_buffer[read] = '\0';
  uart_printf("[%s] LittleFS OK (%ld bytes): %s\r\n", ops.label, (long)read,
              read_buffer);

  ops.unmount(&g_lfs);
}
