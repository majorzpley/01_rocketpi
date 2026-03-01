/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 15:09:21
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-01 11:34:28
 * @FilePath: /29_rocketpi_sdio_card_fatfs/src/main.c
 * @Description:面向RocketPI-STM32F401RE开发板的单线SDIO-MicroSD卡FATFS读写演示工程
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "gpio.h"
#include "sdio.h"
#include "usart.h"

#include "debug_driver.h"
#include <stdio.h>
#include <string.h>

#ifndef _USE_MKFS
#define _USE_MKFS 1
#endif

#if (_USE_MKFS == 1)
#define FATFS_MKFS_BUFFER_SIZE 4096U
#endif
#define FATFS_SPEED_TEST_BUFFER_SIZE 4096U

static void SystemClock_Config(void);
static void FATFS_Test(void);
static void FATFS_SpeedTest(uint32_t kilobytes);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_USART2_UART_Init();
  MY_SDIO_SD_Init();
  MY_CRC_Init();
  MY_FATFS_Init();

  HAL_Delay(200);

  FATFS_Test();
  const uint32_t speed_test_kbytes = 512U;
  FATFS_SpeedTest(speed_test_kbytes);

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

static void FATFS_SpeedTest(uint32_t kilobytes) {
  const char speed_file[] = "sd_speed.bin";
  FRESULT res;
  UINT bytes_io = 0U;
  uint32_t total_bytes = 0U;
  uint32_t remaining = 0U;
  uint32_t start_tick = 0U;
  uint32_t elapsed_ms = 0U;
  uint32_t speed_kbs = 0U;
  uint8_t is_mounted = 0U;
  uint8_t file_opened = 0U;
  uint8_t file_created = 0U;
  static uint8_t transfer_buffer[FATFS_SPEED_TEST_BUFFER_SIZE];

  if (kilobytes == 0U) {
    printf("fatfs speed test: size parameter must be > 0\r\n");
    return;
  }

  if (kilobytes > (UINT32_MAX / 1024U)) {
    printf("fatfs speed test: size too large\r\n");
    return;
  }

  total_bytes = kilobytes * 1024U;

  for (uint32_t i = 0U; i < sizeof(transfer_buffer); ++i) {
    transfer_buffer[i] = (uint8_t)(i & 0xFFU);
  }

  printf("fatfs speed test: mounting %s\r\n", SDPath);
  res = f_mount(&SDFatFS, (TCHAR const *)SDPath, 1);
  if (res != FR_OK) {
    printf("fatfs speed test: mount failed (%d)\r\n", (int)res);
    return;
  }
  is_mounted = 1U;

  res = f_open(&SDFile, speed_file, FA_CREATE_ALWAYS | FA_WRITE);
  if (res != FR_OK) {
    printf("fatfs speed test: open for write failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  file_opened = 1U;
  file_created = 1U;

  printf("fatfs speed test: writing %lu KB\r\n", (unsigned long)kilobytes);
  remaining = total_bytes;
  start_tick = HAL_GetTick();
  while (remaining > 0U) {
    uint32_t chunk = (remaining > sizeof(transfer_buffer))
                         ? (uint32_t)sizeof(transfer_buffer)
                         : remaining;
    res = f_write(&SDFile, transfer_buffer, chunk, &bytes_io);
    if ((res != FR_OK) || (bytes_io != chunk)) {
      printf("fatfs speed test: write error (%d)\r\n", (int)res);
      goto cleanup;
    }
    remaining -= chunk;
  }

  res = f_sync(&SDFile);
  if (res != FR_OK) {
    printf("fatfs speed test: sync failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  elapsed_ms = HAL_GetTick() - start_tick;
  speed_kbs = (elapsed_ms > 0U) ? (uint32_t)(((uint64_t)total_bytes * 1000ULL) /
                                             ((uint64_t)elapsed_ms * 1024ULL))
                                : 0U;
  printf("fatfs speed test: write done in %lu ms (%lu KB/s)\r\n",
         (unsigned long)elapsed_ms, (unsigned long)speed_kbs);

  f_close(&SDFile);
  file_opened = 0U;

  res = f_open(&SDFile, speed_file, FA_READ);
  if (res != FR_OK) {
    printf("fatfs speed test: open for read failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  file_opened = 1U;

  printf("fatfs speed test: reading %lu KB\r\n", (unsigned long)kilobytes);
  remaining = total_bytes;
  start_tick = HAL_GetTick();
  while (remaining > 0U) {
    uint32_t chunk = (remaining > sizeof(transfer_buffer))
                         ? (uint32_t)sizeof(transfer_buffer)
                         : remaining;
    res = f_read(&SDFile, transfer_buffer, chunk, &bytes_io);
    if (res != FR_OK) {
      printf("fatfs speed test: read error (%d)\r\n", (int)res);
      goto cleanup;
    }
    if (bytes_io == 0U) {
      printf("fatfs speed test: unexpected end of file\r\n");
      goto cleanup;
    }
    remaining -= bytes_io;
  }
  elapsed_ms = HAL_GetTick() - start_tick;
  speed_kbs = (elapsed_ms > 0U) ? (uint32_t)(((uint64_t)total_bytes * 1000ULL) /
                                             ((uint64_t)elapsed_ms * 1024ULL))
                                : 0U;
  printf("fatfs speed test: read done in %lu ms (%lu KB/s)\r\n",
         (unsigned long)elapsed_ms, (unsigned long)speed_kbs);

  f_close(&SDFile);
  file_opened = 0U;

  printf("fatfs speed test: removing %s\r\n", speed_file);
  res = f_unlink(speed_file);
  if (res == FR_OK) {
    file_created = 0U;
  } else {
    printf("fatfs speed test: remove failed (%d)\r\n", (int)res);
  }

cleanup:
  if (file_opened) {
    f_close(&SDFile);
  }

  if (is_mounted && file_created) {
    FRESULT unlink_res = f_unlink(speed_file);
    if (unlink_res != FR_OK) {
      printf("fatfs speed test: cleanup remove %s failed (%d)\r\n", speed_file,
             (int)unlink_res);
    }
  }

  if (is_mounted) {
    f_mount(NULL, (TCHAR const *)SDPath, 0);
    printf("fatfs speed test: unmounted\r\n");
  }
}

static void FATFS_Test(void) {
  const char test_file[] = "rocketpi.txt";
  const char test_payload[] = "RocketPi FATFS SDIO write/read demo.\r\n";
  char read_buffer[sizeof(test_payload)] = {0};
  const UINT payload_len = (UINT)strlen(test_payload);
  UINT bytes_written = 0;
  UINT bytes_read = 0;
  FRESULT res;
  uint8_t is_mounted = 0;
  uint8_t file_opened = 0;

  printf("fatfs: mounting %s\r\n", SDPath);
  res = f_mount(&SDFatFS, (TCHAR const *)SDPath, 1);
  if (res == FR_OK) {
    is_mounted = 1;
  } else if (res == FR_NO_FILESYSTEM) {
#if (_USE_MKFS == 1)
#if (FATFS_MKFS_BUFFER_SIZE < 1024U)
#error "FATFS_MKFS_BUFFER_SIZE must be >= 1024 bytes"
#endif
    static uint8_t mkfs_work[FATFS_MKFS_BUFFER_SIZE];

    printf("fatfs: no filesystem, formatting...\r\n");
    res =
        f_mkfs((TCHAR const *)SDPath, FM_ANY, 0, mkfs_work, sizeof(mkfs_work));
    if (res != FR_OK) {
      printf("fatfs: format failed (%d)\r\n", (int)res);
      return;
    }

    printf("fatfs: format complete, remounting\r\n");
    res = f_mount(&SDFatFS, (TCHAR const *)SDPath, 1);
    if (res != FR_OK) {
      printf("fatfs: mount after format failed (%d)\r\n", (int)res);
      return;
    }
    is_mounted = 1;
#else
    printf("fatfs: no filesystem and mkfs disabled\r\n");
    return;
#endif
  } else {
    printf("fatfs: mount failed (%d)\r\n", (int)res);
    return;
  }

  printf("fatfs: creating %s\r\n", test_file);
  res = f_open(&SDFile, test_file, FA_CREATE_ALWAYS | FA_WRITE);
  if (res != FR_OK) {
    printf("fatfs: open for write failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  file_opened = 1;

  res = f_write(&SDFile, test_payload, payload_len, &bytes_written);
  if ((res != FR_OK) || (bytes_written != payload_len)) {
    printf("fatfs: write failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  printf("fatfs: wrote %lu bytes\r\n", (unsigned long)bytes_written);

  f_close(&SDFile);
  file_opened = 0;

  res = f_open(&SDFile, test_file, FA_READ);
  if (res != FR_OK) {
    printf("fatfs: open for read failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  file_opened = 1;

  memset(read_buffer, 0, sizeof(read_buffer));
  res = f_read(&SDFile, read_buffer, sizeof(read_buffer) - 1, &bytes_read);
  if (res != FR_OK) {
    printf("fatfs: read failed (%d)\r\n", (int)res);
    goto cleanup;
  }
  printf("fatfs: read %lu bytes\r\n", (unsigned long)bytes_read);

  f_close(&SDFile);
  file_opened = 0;

  if ((bytes_read == bytes_written) &&
      (strncmp(read_buffer, test_payload, payload_len) == 0)) {
    printf("fatfs: verification OK\r\n");
    printf("fatfs: content: %s", read_buffer);
  } else {
    printf("fatfs: verification failed\r\n");
  }

cleanup:
  if (file_opened) {
    f_close(&SDFile);
  }

  if (is_mounted) {
    f_mount(NULL, (TCHAR const *)SDPath, 0);
    printf("fatfs: unmounted\r\n");
  }
}