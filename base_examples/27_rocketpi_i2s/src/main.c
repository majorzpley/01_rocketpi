/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-26 11:41:30
 * @FilePath: /27_rocketpi_i2s/src/main.c
 * @Description:此工程为rocketpi的把edge-tts语音转成I2S需要的PCM数据,并驱动I2S模块MAX98357A播放音频
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "i2s.h"

#include "audio.h"

typedef enum {
  AUDIO_PLAYBACK_STATE_IDLE = 0,
  AUDIO_PLAYBACK_STATE_RUNNING,
  AUDIO_PLAYBACK_STATE_DONE,
  AUDIO_PLAYBACK_STATE_ERROR
} audio_playback_state_t;

typedef struct {
  uint32_t next_index;
  uint32_t samples_remaining;
  audio_playback_state_t state;
} audio_playback_ctrl_t;

#define AUDIO_DMA_MAX_TRANSFER_SAMPLES 65535U

static volatile audio_playback_ctrl_t audio_ctrl = {0};

static void SystemClock_Config(void);
static HAL_StatusTypeDef Audio_StartNextChunk(void);
static void Audio_BeginPlayback(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_I2S2_Init();

  Audio_BeginPlayback();
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

static HAL_StatusTypeDef Audio_StartNextChunk(void) {
  if (audio_ctrl.samples_remaining == 0U) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_DONE;
    return HAL_OK;
  }

  uint32_t chunk =
      (audio_ctrl.samples_remaining > AUDIO_DMA_MAX_TRANSFER_SAMPLES)
          ? AUDIO_DMA_MAX_TRANSFER_SAMPLES
          : audio_ctrl.samples_remaining;

  const uint16_t *chunk_ptr =
      (const uint16_t *)&audio_track[audio_ctrl.next_index];
  HAL_StatusTypeDef status =
      HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t *)chunk_ptr, (uint16_t)chunk);

  if (status == HAL_OK) {
    audio_ctrl.next_index += chunk;
    audio_ctrl.samples_remaining -= chunk;
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_RUNNING;
  }

  return status;
}

static void Audio_BeginPlayback(void) {
  audio_ctrl.next_index = 0U;
  audio_ctrl.samples_remaining = (uint32_t)AUDIO_TRACK_SAMPLE_COUNT;
  audio_ctrl.state = AUDIO_PLAYBACK_STATE_IDLE;

  if (audio_ctrl.samples_remaining == 0U) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_DONE;
    return;
  }

  if (Audio_StartNextChunk() != HAL_OK) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_ERROR;
    Error_Handler();
  }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance != hi2s2.Instance) {
    return;
  }

  if (audio_ctrl.samples_remaining == 0U) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_DONE;
    return;
  }

  if (Audio_StartNextChunk() != HAL_OK) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_ERROR;
    Error_Handler();
  }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance != hi2s2.Instance) {
    return;
  }

  audio_ctrl.state = AUDIO_PLAYBACK_STATE_ERROR;
  Error_Handler();
}