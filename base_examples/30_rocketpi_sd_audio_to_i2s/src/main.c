/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-02 23:43:28
 * @FilePath: /30_rocketpi_sd_audio_to_i2s/src/main.c
 * @Description:此工程为rocketpi从SD卡读取16-bitPCM，经I2S2+DMA输出，可播放SD流和内置音轨。（拷贝整个audio目录至sd卡根目录）
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "crc.h"
#include "dma.h"
#include "fatfs.h"
#include "gpio.h"
#include "i2s.h"
#include "sdio.h"
#include "spi.h"
#include "usart.h"

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "audio.h"
#include "debug_driver.h"
#include "st7789.h"

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

typedef enum {
  AUDIO_SD_STATE_IDLE = 0,
  AUDIO_SD_STATE_PLAYING,
  AUDIO_SD_STATE_DONE,
  AUDIO_SD_STATE_ERROR
} audio_sd_state_t;

typedef struct {
  FIL file;
  UINT half_bytes;
  UINT frame_bytes;
  UINT bytes_next;
  uint8_t *buf0;
  uint8_t *buf1;
  uint8_t *read_buf;
  uint8_t mounted;
  uint8_t file_opened;
  audio_sd_state_t state;
  FRESULT last_res;
  HAL_StatusTypeDef last_hal;
} audio_sd_playback_t;

#ifndef _USE_MKFS
#define _USE_MKFS 1
#endif

#if (_USE_MKFS == 1)
#define FATFS_MKFS_BUFFER_SIZE 4096U
#endif
#define FATFS_SPEED_TEST_BUFFER_SIZE 4096U

#define AUDIO_DMA_MAX_TRANSFER_SAMPLES 65535U
#define AUDIO_FILE_DIRECTORY "audio"
#define AUDIO_FILE_NAME "audio.bin"
#define AUDIO_FILE_PATH_MAX 64U
// #define AUDIO_STREAM_BUFFER_BYTES 5120U
#define AUDIO_STREAM_BUFFER_BYTES 8192U
// #define AUDIO_STREAM_BUFFER_BYTES 7680U

#define AUDIO_VOLUME_PERCENT 100U /* 0-100%，用于数字音量缩放 */
#define AUDIO_VOLUME_BUFFER_SAMPLES                                            \
  2048U /* 内置音轨播放时的临时缓冲区大小（采样数） */

#define SPECTRUM_BIN_COUNT 40U
#define SPECTRUM_WINDOW_FRAMES 256U
#define SPECTRUM_DRAW_INTERVAL_MS 40U
#define SPECTRUM_MAX_HEIGHT (ST7789_HIGHT - 10U)
#define SPECTRUM_BAR_GAP 2U
#define SPECTRUM_SMOOTH_ALPHA 0.30f
#define SPECTRUM_DECAY_ALPHA 0.90f
#define SPECTRUM_PEAK_INIT 2000.0f
#ifndef M_PI
#define M_PI 3.1415926f
#endif

static volatile audio_playback_ctrl_t audio_ctrl = {0};
static uint16_t audio_sd_buffer[AUDIO_STREAM_BUFFER_BYTES / sizeof(uint16_t)];
static audio_sd_playback_t audio_sd_ctx = {0};
static volatile uint8_t audio_stream_from_sd = 0U;
static volatile uint8_t audio_sd_dma_done = 0U;
/* 记录当前 I2S 外设是否已经初始化并处于工作状态 */
static uint8_t audio_output_active = 0U;
static uint16_t audio_volume_q15 = 0U;
static int16_t audio_volume_buffer[AUDIO_VOLUME_BUFFER_SAMPLES];
static float spectrum_coeff[SPECTRUM_BIN_COUNT];
static float spectrum_smooth[SPECTRUM_BIN_COUNT];
static uint16_t spectrum_levels[SPECTRUM_BIN_COUNT];
static float spectrum_peak_avg = SPECTRUM_PEAK_INIT;
static uint32_t spectrum_last_draw_ms = 0U;
typedef struct {
  float pos;
  uint16_t color;
} spectrum_stop_t;
static const spectrum_stop_t spectrum_gradient[] = {
    {0.0f, BLUE}, {0.25f, CYAN}, {0.50f, GREEN}, {0.75f, YELLOW}, {1.0f, RED}};

static void SystemClock_Config(void);
static void fatfs_speed_test(uint32_t kilobytes);
static HAL_StatusTypeDef audio_start_next_chunk(void);
static void audio_sd_stop_and_cleanup(void);
static FRESULT audio_sd_start_playback(const char *directory,
                                       const char *file_name,
                                       uint8_t *work_buffer,
                                       UINT work_buffer_bytes);
static void audio_sd_process_playback(void);
static void audio_output_force_idle(void);
static void audio_output_ensure_enabled(void);
static void spectrum_init(void);
static void spectrum_update_from_buffer(const int16_t *samples,
                                        size_t sample_count);
static void spectrum_draw_if_due(void);
static uint16_t spectrum_lerp_color(uint16_t c0, uint16_t c1, float t);
static uint16_t spectrum_color_for_bin(uint32_t index);
static uint16_t audio_compute_volume_q15(void);
static int16_t audio_apply_volume_to_sample(int16_t sample, uint16_t gain_q15);
static void audio_apply_volume_to_buffer(int16_t *buffer, size_t samples,
                                         uint16_t gain_q15);

/* 将 I2S/DMA 完全停止并把引脚拉至静音状态 */
static void audio_output_force_idle(void) {
  (void)HAL_I2S_DMAStop(&hi2s2);

  if (audio_output_active) {
    (void)HAL_I2S_DeInit(&hi2s2);
    audio_output_active = 0U;
  }

  __HAL_RCC_GPIOB_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15,
                    GPIO_PIN_RESET);
}

/* 若 I2S 处于关闭状态则重新初始化供输出使用 */
static void audio_output_ensure_enabled(void) {
  if (audio_output_active) {
    return;
  }

  MY_I2S2_Init();
  audio_output_active = 1U;
}

static uint16_t audio_compute_volume_q15(void) {
  uint32_t percent = AUDIO_VOLUME_PERCENT;
  if (percent > 100U) {
    percent = 100U;
  }

  return (uint16_t)(((percent * 32767U) + 50U) / 100U);
}

static int16_t audio_apply_volume_to_sample(int16_t sample, uint16_t gain_q15) {
  int32_t scaled = (int32_t)sample * (int32_t)gain_q15;
  scaled += (1 << 14); /* 四舍五入 */
  scaled >>= 15;

  if (scaled > 32767) {
    scaled = 32767;
  } else if (scaled < -32768) {
    scaled = -32768;
  }

  return (int16_t)scaled;
}

static void audio_apply_volume_to_buffer(int16_t *buffer, size_t samples,
                                         uint16_t gain_q15) {
  if ((buffer == NULL) || (samples == 0U)) {
    return;
  }

  if (gain_q15 == 0U) {
    memset(buffer, 0, samples * sizeof(int16_t));
    return;
  }

  if (gain_q15 >= 32767U) {
    return; /* 100% 音量，无需处理 */
  }

  for (size_t i = 0U; i < samples; ++i) {
    buffer[i] = audio_apply_volume_to_sample(buffer[i], gain_q15);
  }
}

static HAL_StatusTypeDef audio_start_next_chunk(void) {
  if (audio_ctrl.samples_remaining == 0U) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_DONE;
    return HAL_OK;
  }

  uint32_t chunk_limit =
      (AUDIO_VOLUME_BUFFER_SAMPLES < AUDIO_DMA_MAX_TRANSFER_SAMPLES)
          ? AUDIO_VOLUME_BUFFER_SAMPLES
          : AUDIO_DMA_MAX_TRANSFER_SAMPLES;
  uint32_t chunk = (audio_ctrl.samples_remaining > chunk_limit)
                       ? chunk_limit
                       : audio_ctrl.samples_remaining;

  const int16_t *chunk_ptr =
      (const int16_t *)&audio_track[audio_ctrl.next_index];
  memcpy(audio_volume_buffer, chunk_ptr, chunk * sizeof(int16_t));
  audio_apply_volume_to_buffer(audio_volume_buffer, chunk, audio_volume_q15);
  HAL_StatusTypeDef status = HAL_I2S_Transmit_DMA(
      &hi2s2, (uint16_t *)audio_volume_buffer, (uint16_t)chunk);

  if (status == HAL_OK) {
    audio_ctrl.next_index += chunk;
    audio_ctrl.samples_remaining -= chunk;
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_RUNNING;
  }

  return status;
}

/* stop_output=1 时连同 I2S 一起关闭，0 仅重置文件/缓存状态 */
static void audio_sd_stop_and_cleanup_ex(uint8_t stop_output) {
  if (stop_output) {
    audio_output_force_idle();
  }

  if (audio_sd_ctx.file_opened) {
    f_close(&audio_sd_ctx.file);
    audio_sd_ctx.file_opened = 0U;
  }

  if (audio_sd_ctx.mounted) {
    f_mount(NULL, (TCHAR const *)SDPath, 0);
    audio_sd_ctx.mounted = 0U;
  }

  audio_stream_from_sd = 0U;
}

static void audio_sd_stop_and_cleanup(void) {
  audio_sd_stop_and_cleanup_ex(1U);
}

static FRESULT audio_sd_start_playback(const char *directory,
                                       const char *file_name,
                                       uint8_t *work_buffer,
                                       UINT work_buffer_bytes) {
  audio_volume_q15 = audio_compute_volume_q15();
  const UINT frame_bytes = (UINT)(AUDIO_NUM_CHANNELS * sizeof(int16_t));
  if ((work_buffer == NULL) || (work_buffer_bytes < (2U * frame_bytes)) ||
      ((work_buffer_bytes % frame_bytes) != 0U)) {
    return FR_INVALID_PARAMETER;
  }

  if (audio_sd_ctx.state == AUDIO_SD_STATE_PLAYING) {
    return FR_LOCKED;
  }

  audio_sd_stop_and_cleanup_ex(
      0U); /* 仅重置 FatFs 句柄，保留 I2S 以便随后立即启动播放 */
  memset(&audio_sd_ctx, 0, sizeof(audio_sd_ctx));

  UINT half_bytes = work_buffer_bytes / 2U;
  const UINT max_bytes = AUDIO_DMA_MAX_TRANSFER_SAMPLES * sizeof(uint16_t);
  if (half_bytes > max_bytes) {
    half_bytes = max_bytes;
  }
  const UINT align = (512U > frame_bytes) ? 512U : frame_bytes;
  half_bytes -= (half_bytes % align);
  if (half_bytes == 0U) {
    return FR_INVALID_PARAMETER;
  }

  audio_sd_ctx.buf0 = work_buffer;
  audio_sd_ctx.buf1 = work_buffer + half_bytes;
  audio_sd_ctx.read_buf = audio_sd_ctx.buf1;
  audio_sd_ctx.half_bytes = half_bytes;
  audio_sd_ctx.frame_bytes = frame_bytes;
  audio_sd_ctx.state = AUDIO_SD_STATE_IDLE;

  char file_path[AUDIO_FILE_PATH_MAX];
  int written = ((directory != NULL) && (directory[0] != '\0'))
                    ? snprintf(file_path, sizeof(file_path), "%s/%s", directory,
                               file_name)
                    : snprintf(file_path, sizeof(file_path), "%s", file_name);
  if ((written <= 0) || ((size_t)written >= sizeof(file_path))) {
    return FR_INVALID_NAME;
  }

  FRESULT res = f_mount(&SDFatFS, (TCHAR const *)SDPath, 1);
  if (res != FR_OK) {
    printf("audio(fs): mount failed (%d)\r\n", (int)res);
    audio_sd_ctx.last_res = res;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    return res;
  }
  audio_sd_ctx.mounted = 1U;

  res = f_open(&audio_sd_ctx.file, file_path, FA_READ);
  if (res != FR_OK) {
    printf("audio(fs): open %s failed (%d)\r\n", file_path, (int)res);
    audio_sd_ctx.last_res = res;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    return res;
  }
  audio_sd_ctx.file_opened = 1U;

  UINT bytes0 = 0U;
  UINT bytes1 = 0U;
  res = f_read(&audio_sd_ctx.file, audio_sd_ctx.buf0, half_bytes, &bytes0);
  if (res != FR_OK) {
    printf("audio(fs): first read failed (%d)\r\n", (int)res);
    audio_sd_ctx.last_res = res;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    return res;
  }
  bytes0 -= (bytes0 % frame_bytes);
  if (bytes0 > 0U) {
    audio_apply_volume_to_buffer((int16_t *)audio_sd_ctx.buf0,
                                 bytes0 / sizeof(int16_t), audio_volume_q15);
  }
  if (bytes0 == 0U) {
    printf("audio(fs): file empty\r\n");
    audio_sd_ctx.state = AUDIO_SD_STATE_DONE;
    audio_sd_stop_and_cleanup();
    return FR_OK;
  }

  res = f_read(&audio_sd_ctx.file, audio_sd_ctx.buf1, half_bytes, &bytes1);
  if (res != FR_OK) {
    printf("audio(fs): second read failed (%d)\r\n", (int)res);
    audio_sd_ctx.last_res = res;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    return res;
  }
  bytes1 -= (bytes1 % frame_bytes);
  if (bytes1 > 0U) {
    audio_apply_volume_to_buffer((int16_t *)audio_sd_ctx.buf1,
                                 bytes1 / sizeof(int16_t), audio_volume_q15);
  }
  audio_sd_ctx.bytes_next = bytes1;

  audio_stream_from_sd = 1U;
  audio_sd_dma_done = 0U;
  audio_output_ensure_enabled(); /* SD 播放准备驱动 DMA 前确保 I2S 已启动 */
  HAL_StatusTypeDef hal =
      HAL_I2S_Transmit_DMA(&hi2s2, (uint16_t *)audio_sd_ctx.buf0,
                           (uint16_t)(bytes0 / sizeof(uint16_t)));
  if (hal != HAL_OK) {
    printf("audio(fs): start DMA failed (%ld)\r\n", (long)hal);
    audio_sd_ctx.last_res = FR_INT_ERR;
    audio_sd_ctx.last_hal = hal;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    return FR_INT_ERR;
  }

  spectrum_update_from_buffer((const int16_t *)audio_sd_ctx.buf0,
                              bytes0 / sizeof(uint16_t));

  audio_sd_ctx.state = AUDIO_SD_STATE_PLAYING;
  return FR_OK;
}

static void audio_sd_process_playback(void) {
  if (audio_sd_ctx.state != AUDIO_SD_STATE_PLAYING) {
    return;
  }

  if (audio_sd_dma_done == 0U) {
    return;
  }
  audio_sd_dma_done = 0U;

  if (audio_sd_ctx.bytes_next == 0U) {
    HAL_I2S_DMAStop(&hi2s2);
    audio_sd_ctx.state = AUDIO_SD_STATE_DONE;
    audio_sd_stop_and_cleanup();
    return;
  }

  uint8_t *play_buf = audio_sd_ctx.read_buf;
  UINT play_bytes = audio_sd_ctx.bytes_next;
  audio_sd_ctx.read_buf =
      (play_buf == audio_sd_ctx.buf0) ? audio_sd_ctx.buf1 : audio_sd_ctx.buf0;
  HAL_StatusTypeDef hal = HAL_I2S_Transmit_DMA(
      &hi2s2, (uint16_t *)play_buf, (uint16_t)(play_bytes / sizeof(uint16_t)));
  if (hal != HAL_OK) {
    printf("audio(fs): DMA start failed (%ld)\r\n", (long)hal);
    audio_sd_ctx.last_res = FR_INT_ERR;
    audio_sd_ctx.last_hal = hal;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    return;
  }

  spectrum_update_from_buffer((const int16_t *)play_buf,
                              play_bytes / sizeof(uint16_t));

  UINT bytes_read = 0U;
  FRESULT res = f_read(&audio_sd_ctx.file, audio_sd_ctx.read_buf,
                       audio_sd_ctx.half_bytes, &bytes_read);
  if (res != FR_OK) {
    printf("audio(fs): read failed (%d)\r\n", (int)res);
    audio_sd_ctx.last_res = res;
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_stop_and_cleanup();
    HAL_I2S_DMAStop(&hi2s2);
    return;
  }
  bytes_read -= (bytes_read % audio_sd_ctx.frame_bytes);
  if (bytes_read > 0U) {
    audio_apply_volume_to_buffer((int16_t *)audio_sd_ctx.read_buf,
                                 bytes_read / sizeof(int16_t),
                                 audio_volume_q15);
  }
  audio_sd_ctx.bytes_next = bytes_read;
}

static void spectrum_init(void) {
  const float bin_width_hz =
      ((float)AUDIO_SAMPLE_RATE_HZ * 0.5f) / (float)SPECTRUM_BIN_COUNT;

  for (uint32_t i = 0U; i < SPECTRUM_BIN_COUNT; ++i) {
    float freq = bin_width_hz * (float)(i + 1U);
    float omega = 2.0f * (float)M_PI * freq / (float)AUDIO_SAMPLE_RATE_HZ;
    spectrum_coeff[i] = 2.0f * cosf(omega);
    spectrum_smooth[i] = 0.0f;
    spectrum_levels[i] = 0U;
  }

  spectrum_peak_avg = SPECTRUM_PEAK_INIT;
  spectrum_last_draw_ms = HAL_GetTick();
}

static void spectrum_update_from_buffer(const int16_t *samples,
                                        size_t sample_count) {
  if ((samples == NULL) || (sample_count < AUDIO_NUM_CHANNELS)) {
    return;
  }

  size_t frames = sample_count / AUDIO_NUM_CHANNELS;
  size_t window = (frames < (size_t)SPECTRUM_WINDOW_FRAMES)
                      ? frames
                      : (size_t)SPECTRUM_WINDOW_FRAMES;
  if (window < 8U) {
    return;
  }

  for (uint32_t bin = 0U; bin < SPECTRUM_BIN_COUNT; ++bin) {
    float coeff = spectrum_coeff[bin];
    float q0 = 0.0f;
    float q1 = 0.0f;
    float q2 = 0.0f;

    for (size_t n = 0U; n < window; ++n) {
      float x = (float)samples[n * AUDIO_NUM_CHANNELS];
      q0 = (coeff * q1) - q2 + x;
      q2 = q1;
      q1 = q0;
    }

    float power = (q1 * q1) + (q2 * q2) - (coeff * q1 * q2);
    if (power < 0.0f) {
      power = 0.0f;
    }
    float mag = sqrtf(power) / (float)window;

    spectrum_peak_avg = (SPECTRUM_DECAY_ALPHA * spectrum_peak_avg) +
                        ((1.0f - SPECTRUM_DECAY_ALPHA) * mag);
    float scaled =
        (mag * (float)SPECTRUM_MAX_HEIGHT) / (spectrum_peak_avg + 1.0f);
    if (scaled < 0.0f) {
      scaled = 0.0f;
    }
    if (scaled > (float)SPECTRUM_MAX_HEIGHT) {
      scaled = (float)SPECTRUM_MAX_HEIGHT;
    }

    float smooth = (1.0f - SPECTRUM_SMOOTH_ALPHA) * spectrum_smooth[bin] +
                   (SPECTRUM_SMOOTH_ALPHA * scaled);
    spectrum_smooth[bin] = smooth;
    spectrum_levels[bin] = (uint16_t)smooth;
  }
}

static void spectrum_draw_if_due(void) {
  uint32_t now = HAL_GetTick();
  if ((now - spectrum_last_draw_ms) < SPECTRUM_DRAW_INTERVAL_MS) {
    return;
  }
  spectrum_last_draw_ms = now;

  const uint16_t bar_width = ST7789_WIDTH / SPECTRUM_BIN_COUNT;
  uint16_t usable_width = (bar_width > SPECTRUM_BAR_GAP)
                              ? (bar_width - SPECTRUM_BAR_GAP)
                              : bar_width;
  if (usable_width == 0U) {
    usable_width = 1U;
  }
  const uint16_t y_base = ST7789_HIGHT - 1U;

  for (uint32_t i = 0U; i < SPECTRUM_BIN_COUNT; ++i) {
    uint16_t h = spectrum_levels[i];
    uint16_t x0 = (uint16_t)(i * bar_width);
    x0 = (uint16_t)(x0 + ((bar_width - usable_width) / 2U));
    uint16_t x1 = x0 + usable_width - 1U;
    if (x1 >= ST7789_WIDTH) {
      x1 = ST7789_WIDTH - 1U;
    }

    ST7789_FillRect(x0, 0U, x1, y_base, BLACK);

    if (h > 0U) {
      uint16_t y0 = (h >= ST7789_HIGHT) ? 0U : (uint16_t)(ST7789_HIGHT - h);
      uint16_t color = spectrum_color_for_bin(i);
      ST7789_FillRect(x0, y0, x1, y_base, color);
    }
  }
}

static uint16_t spectrum_lerp_color(uint16_t c0, uint16_t c1, float t) {
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;

  float r0 = (float)((c0 >> 11) & 0x1FU);
  float g0 = (float)((c0 >> 5) & 0x3FU);
  float b0 = (float)(c0 & 0x1FU);

  float r1 = (float)((c1 >> 11) & 0x1FU);
  float g1 = (float)((c1 >> 5) & 0x3FU);
  float b1 = (float)(c1 & 0x1FU);

  float r = r0 + (r1 - r0) * t;
  float g = g0 + (g1 - g0) * t;
  float b = b0 + (b1 - b0) * t;

  uint16_t ri = (uint16_t)(r + 0.5f);
  uint16_t gi = (uint16_t)(g + 0.5f);
  uint16_t bi = (uint16_t)(b + 0.5f);

  if (ri > 0x1F)
    ri = 0x1F;
  if (gi > 0x3F)
    gi = 0x3F;
  if (bi > 0x1F)
    bi = 0x1F;

  return (uint16_t)((ri << 11) | (gi << 5) | bi);
}

static uint16_t spectrum_color_for_bin(uint32_t index) {
  const size_t stop_count =
      sizeof(spectrum_gradient) / sizeof(spectrum_gradient[0]);
  if (stop_count == 0U) {
    return WHITE;
  }

  float t = (SPECTRUM_BIN_COUNT > 1U)
                ? ((float)index / (float)(SPECTRUM_BIN_COUNT - 1U))
                : 0.0f;
  if (t <= spectrum_gradient[0].pos) {
    return spectrum_gradient[0].color;
  }
  if (t >= spectrum_gradient[stop_count - 1U].pos) {
    return spectrum_gradient[stop_count - 1U].color;
  }

  for (size_t i = 0U; i < (stop_count - 1U); ++i) {
    float p0 = spectrum_gradient[i].pos;
    float p1 = spectrum_gradient[i + 1U].pos;
    if ((t >= p0) && (t <= p1)) {
      float local_t = (p1 > p0) ? ((t - p0) / (p1 - p0)) : 0.0f;
      return spectrum_lerp_color(spectrum_gradient[i].color,
                                 spectrum_gradient[i + 1U].color, local_t);
    }
  }

  return spectrum_gradient[stop_count - 1U].color;
}

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_USART2_UART_Init();
  MY_SPI1_Init();
  MY_SDIO_SD_Init();
  MY_FATFS_Init();
  MY_CRC_Init();
  MY_I2S2_Init();

  audio_output_active = 1U;
  audio_output_force_idle(); /* CubeMX 先初始化 I2S，这里立即关掉避免开机噪声 */
  HAL_Delay(200);
  ST7789_Init();
  ST7789_Clear(BLACK);
  spectrum_init();

  const uint32_t speed_test_kbytes =
      512U; /* Adjust size to profile different transfers */
  fatfs_speed_test(speed_test_kbytes);

  FRESULT audio_res = audio_sd_start_playback(
      AUDIO_FILE_DIRECTORY, AUDIO_FILE_NAME, (uint8_t *)audio_sd_buffer,
      (UINT)sizeof(audio_sd_buffer));
  if (audio_res != FR_OK) {
    printf("audio(fs): playback failed (%d)\r\n", (int)audio_res);
  }
  uint8_t audio_playback_reported = 0U;
  while (1) {
    /* 播放状态机：SD DMA 半双缓冲 + 频谱刷新 */
    audio_sd_process_playback();
    spectrum_draw_if_due();

    if (audio_playback_reported == 0U) {
      if (audio_sd_ctx.state == AUDIO_SD_STATE_DONE) {
        printf("audio(fs): playback done\r\n");
        audio_playback_reported = 1U;
      } else if (audio_sd_ctx.state == AUDIO_SD_STATE_ERROR) {
        printf("audio(fs): playback error (%d, hal %ld)\r\n",
               (int)audio_sd_ctx.last_res, (long)audio_sd_ctx.last_hal);
        audio_playback_reported = 1U;
      }
    }
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

static void fatfs_speed_test(uint32_t kilobytes) {
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

/* I2S DMA 传输完成回调：区分 SD 流/内置音轨的续传逻辑 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance != hi2s2.Instance) {
    return;
  }

  if (audio_stream_from_sd) {
    audio_sd_dma_done = 1U;
    return;
  }

  if (audio_ctrl.samples_remaining == 0U) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_DONE;
    audio_output_force_idle();
    return;
  }

  if (audio_start_next_chunk() != HAL_OK) {
    audio_ctrl.state = AUDIO_PLAYBACK_STATE_ERROR;
    Error_Handler();
  }
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance != hi2s2.Instance) {
    return;
  }

  if (audio_stream_from_sd) {
    audio_sd_ctx.state = AUDIO_SD_STATE_ERROR;
    audio_sd_ctx.last_hal = HAL_ERROR;
    audio_sd_ctx.last_res = FR_INT_ERR;
    audio_sd_dma_done = 1U;
    audio_sd_stop_and_cleanup();
    return;
  }

  audio_ctrl.state = AUDIO_PLAYBACK_STATE_ERROR;
  Error_Handler();
}