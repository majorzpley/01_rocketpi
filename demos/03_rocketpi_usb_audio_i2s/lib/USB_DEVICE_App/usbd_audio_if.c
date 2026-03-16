/**
 ******************************************************************************
 * @file           : usbd_audio_if.c
 * @version        : v1.0_Cube
 * @brief          : Generic media access layer.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "usbd_audio_if.h"

#include "i2s.h"
#include "main.h"
#include "spectrum.h"
#include <string.h>

typedef enum {
  AUDIO_HW_STATE_IDLE = 0,
  AUDIO_HW_STATE_RUNNING
} audio_hw_state_t;

typedef struct {
  audio_hw_state_t state;
  uint8_t volume;
  uint8_t muted;
  uint8_t underrun_active;
  uint8_t start_pending;
} audio_hw_ctx_t;

#define AUDIO_STREAM_GAP_FLUSH_MS 60U
#define AUDIO_USB_CHANNELS 2U

static audio_hw_ctx_t audio_ctx = {0};
static volatile uint32_t audio_underrun_events = 0U;
static volatile uint32_t audio_underrun_silence_bytes = 0U;
static volatile uint32_t audio_last_packet_ms = 0U;

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t AUDIO_Init_FS(uint32_t AudioFreq, uint32_t Volume,
                            uint32_t options);
static int8_t AUDIO_DeInit_FS(uint32_t options);
static int8_t AUDIO_AudioCmd_FS(uint8_t *pbuf, uint32_t size, uint8_t cmd);
static int8_t AUDIO_VolumeCtl_FS(uint8_t vol);
static int8_t AUDIO_MuteCtl_FS(uint8_t cmd);
static int8_t AUDIO_PeriodicTC_FS(uint8_t *pbuf, uint32_t size, uint8_t cmd);
static int8_t AUDIO_GetState_FS(void);

static USBD_AUDIO_HandleTypeDef *Audio_GetClassHandle(void);
static int8_t Audio_StartPlayback(void);
static void Audio_StopPlayback(void);
static uint32_t Audio_GetReadableBytes(void);
static void Audio_CheckUnderrun(void);
static void Audio_ResetUsbBuffer(void);
static void Audio_ZeroBufferRegion(USBD_AUDIO_HandleTypeDef *haudio,
                                   uint32_t start, uint32_t length);
static HAL_StatusTypeDef Audio_BeginDma(USBD_AUDIO_HandleTypeDef *haudio);
static void Audio_TryStartPending(void);
static void Audio_HandlePacketArrival(uint32_t bytes);
static void Audio_RequestRestart(void);

USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops_FS = {
    AUDIO_Init_FS,      AUDIO_DeInit_FS,  AUDIO_AudioCmd_FS,
    AUDIO_VolumeCtl_FS, AUDIO_MuteCtl_FS, AUDIO_PeriodicTC_FS,
    AUDIO_GetState_FS,
};

/**
 * @brief  Initializes the AUDIO media low layer over USB FS IP
 * @param  AudioFreq: Audio frequency used to play the audio stream.
 * @param  Volume: Initial volume level (from 0 (Mute) to 100 (Max))
 * @param  options: Reserved for future use
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_Init_FS(uint32_t AudioFreq, uint32_t Volume,
                            uint32_t options) {
  UNUSED(AudioFreq);
  UNUSED(options);

  audio_ctx.state = AUDIO_HW_STATE_IDLE;
  audio_ctx.volume = (uint8_t)Volume;
  audio_ctx.muted = 0U;
  audio_ctx.start_pending = 0U;
  Audio_StopPlayback();

  return (USBD_OK);
}

/**
 * @brief  De-Initializes the AUDIO media low layer
 * @param  options: Reserved for future use
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_DeInit_FS(uint32_t options) {
  UNUSED(options);
  Audio_StopPlayback();
  return (USBD_OK);
}

/**
 * @brief  Handles AUDIO command.
 * @param  pbuf: Pointer to buffer of data to be sent
 * @param  size: Number of data to be sent (in bytes)
 * @param  cmd: Command opcode
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_AudioCmd_FS(uint8_t *pbuf, uint32_t size, uint8_t cmd) {
  UNUSED(pbuf);
  UNUSED(size);

  switch (cmd) {
  case AUDIO_CMD_START:
    Audio_StopPlayback();
    return Audio_StartPlayback();

  case AUDIO_CMD_PLAY:
    if (audio_ctx.state != AUDIO_HW_STATE_RUNNING) {
      return Audio_StartPlayback();
    }
    return (int8_t)USBD_OK;

  case AUDIO_CMD_STOP:
    Audio_StopPlayback();
    return (int8_t)USBD_OK;

  default:
    return (int8_t)USBD_FAIL;
  }
}

/**
 * @brief  Controls AUDIO Volume.
 * @param  vol: volume level (0..100)
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_VolumeCtl_FS(uint8_t vol) {
  audio_ctx.volume = vol;
  return (USBD_OK);
}

/**
 * @brief  Controls AUDIO Mute.
 * @param  cmd: command opcode
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_MuteCtl_FS(uint8_t cmd) {
  audio_ctx.muted = (cmd != 0U) ? 1U : 0U;
  return (USBD_OK);
}

/**
 * @brief  AUDIO_PeriodicT_FS
 * @param  cmd: Command opcode
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_PeriodicTC_FS(uint8_t *pbuf, uint32_t size, uint8_t cmd) {
  UNUSED(cmd);

  Audio_HandlePacketArrival(size);

  if ((audio_ctx.muted != 0U) && (pbuf != NULL) && (size > 0U)) {
    (void)memset(pbuf, 0, size);
  }

  if ((pbuf != NULL) &&
      (size >= (uint32_t)(AUDIO_USB_CHANNELS * sizeof(int16_t)))) {
    Spectrum_UpdateFromInterleaved((const int16_t *)pbuf,
                                   size / sizeof(int16_t), AUDIO_USB_CHANNELS);
  }

  Audio_TryStartPending();

  return (USBD_OK);
}

/**
 * @brief  Gets AUDIO State.
 * @retval USBD_OK if all operations are OK else USBD_FAIL
 */
static int8_t AUDIO_GetState_FS(void) {
  return (audio_ctx.state == AUDIO_HW_STATE_RUNNING) ? (int8_t)USBD_OK
                                                     : (int8_t)USBD_FAIL;
}

/**
 * @brief  Manages the DMA full transfer complete event.
 * @retval None
 */
void TransferComplete_CallBack_FS(void) {
  USBD_AUDIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_FULL);
}

/**
 * @brief  Manages the DMA Half transfer complete event.
 * @retval None
 */
void HalfTransfer_CallBack_FS(void) {
  USBD_AUDIO_Sync(&hUsbDeviceFS, AUDIO_OFFSET_HALF);
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
static USBD_AUDIO_HandleTypeDef *Audio_GetClassHandle(void) {
  return (USBD_AUDIO_HandleTypeDef *)hUsbDeviceFS.pClassData;
}

static int8_t Audio_StartPlayback(void) {
  if (audio_ctx.state == AUDIO_HW_STATE_RUNNING) {
    return (int8_t)USBD_OK;
  }

  USBD_AUDIO_HandleTypeDef *haudio = Audio_GetClassHandle();

  if (haudio == NULL) {
    return (int8_t)USBD_FAIL;
  }

  if (haudio->rd_enable == 0U) {
    audio_ctx.start_pending = 1U;
    return (int8_t)USBD_OK;
  }

  if (Audio_BeginDma(haudio) != HAL_OK) {
    return (int8_t)USBD_FAIL;
  }

  return (int8_t)USBD_OK;
}

static void Audio_StopPlayback(void) {
  if (audio_ctx.state == AUDIO_HW_STATE_RUNNING) {
    (void)HAL_I2S_DMAStop(&hi2s2);
  }

  audio_ctx.state = AUDIO_HW_STATE_IDLE;
  audio_ctx.underrun_active = 0U;
  audio_ctx.start_pending = 0U;
  Audio_ResetUsbBuffer();
}

static void Audio_ResetUsbBuffer(void) {
  USBD_AUDIO_HandleTypeDef *haudio = Audio_GetClassHandle();

  if (haudio == NULL) {
    return;
  }

  haudio->wr_ptr = 0U;
  haudio->rd_ptr = 0U;
  haudio->offset = AUDIO_OFFSET_UNKNOWN;
  haudio->rd_enable = 0U;
  (void)memset(haudio->buffer, 0, AUDIO_TOTAL_BUF_SIZE);
  audio_last_packet_ms = 0U;
}

static void Audio_ZeroBufferRegion(USBD_AUDIO_HandleTypeDef *haudio,
                                   uint32_t start, uint32_t length) {
  if ((haudio == NULL) || (length == 0U)) {
    return;
  }

  uint32_t remaining = length;

  while (remaining > 0U) {
    uint32_t contiguous = AUDIO_TOTAL_BUF_SIZE - start;
    if (contiguous > remaining) {
      contiguous = remaining;
    }

    (void)memset(&haudio->buffer[start], 0, contiguous);

    remaining -= contiguous;
    start += contiguous;
    if (start >= AUDIO_TOTAL_BUF_SIZE) {
      start = 0U;
    }
  }
}

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance == hi2s2.Instance) {
    TransferComplete_CallBack_FS();
    Audio_CheckUnderrun();
  }
}

void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance == hi2s2.Instance) {
    HalfTransfer_CallBack_FS();
    Audio_CheckUnderrun();
  }
}

static HAL_StatusTypeDef Audio_BeginDma(USBD_AUDIO_HandleTypeDef *haudio) {
  uint32_t samples = AUDIO_TOTAL_BUF_SIZE / 2U;

  HAL_StatusTypeDef status = HAL_I2S_Transmit_DMA(
      &hi2s2, (uint16_t *)haudio->buffer, (uint16_t)samples);
  if (status == HAL_OK) {
    audio_ctx.state = AUDIO_HW_STATE_RUNNING;
    audio_ctx.underrun_active = 0U;
    audio_ctx.start_pending = 0U;
  }

  return status;
}

static void Audio_TryStartPending(void) {
  if ((audio_ctx.start_pending == 0U) ||
      (audio_ctx.state == AUDIO_HW_STATE_RUNNING)) {
    return;
  }

  USBD_AUDIO_HandleTypeDef *haudio = Audio_GetClassHandle();

  if ((haudio == NULL) || (haudio->rd_enable == 0U)) {
    return;
  }

  (void)Audio_BeginDma(haudio);
}

static void Audio_HandlePacketArrival(uint32_t bytes) {
  uint32_t now = HAL_GetTick();
  uint8_t flush_required = 0U;

  if (bytes < AUDIO_OUT_PACKET) {
    flush_required = 1U;
  } else if (audio_last_packet_ms != 0U) {
    uint32_t delta = now - audio_last_packet_ms;
    if (delta > AUDIO_STREAM_GAP_FLUSH_MS) {
      flush_required = 1U;
    }
  }

  if (flush_required != 0U) {
    Audio_RequestRestart();
  }

  audio_last_packet_ms = now;
}

static void Audio_RequestRestart(void) {
  uint8_t restart_needed = (audio_ctx.state == AUDIO_HW_STATE_RUNNING)
                               ? 1U
                               : audio_ctx.start_pending;
  Audio_StopPlayback();
  audio_ctx.start_pending = restart_needed;
}

void HAL_I2S_ErrorCallback(I2S_HandleTypeDef *hi2s) {
  if (hi2s->Instance == hi2s2.Instance) {
    Audio_StopPlayback();
  }
}

static uint32_t Audio_GetReadableBytes(void) {
  USBD_AUDIO_HandleTypeDef *haudio = Audio_GetClassHandle();

  if ((haudio == NULL) || (haudio->rd_enable == 0U)) {
    return 0U;
  }

  uint32_t wr = haudio->wr_ptr;
  uint32_t rd = haudio->rd_ptr;

  if (wr >= rd) {
    return wr - rd;
  }

  return (AUDIO_TOTAL_BUF_SIZE - rd) + wr;
}

static void Audio_CheckUnderrun(void) {
  if (audio_ctx.state != AUDIO_HW_STATE_RUNNING) {
    return;
  }

  if (audio_last_packet_ms != 0U) {
    uint32_t now = HAL_GetTick();
    if ((now - audio_last_packet_ms) > AUDIO_STREAM_GAP_FLUSH_MS) {
      Audio_RequestRestart();
      return;
    }
  }

  USBD_AUDIO_HandleTypeDef *haudio = Audio_GetClassHandle();

  if ((haudio == NULL) || (haudio->rd_enable == 0U)) {
    return;
  }

  uint32_t chunk_bytes = AUDIO_TOTAL_BUF_SIZE / 2U;
  uint32_t readable = Audio_GetReadableBytes();

  if (readable > chunk_bytes) {
    readable = chunk_bytes;
  }

  if (readable >= chunk_bytes) {
    audio_ctx.underrun_active = 0U;
    return;
  }

  uint32_t missing = chunk_bytes - readable;
  uint32_t start = haudio->rd_ptr + readable;
  if (start >= AUDIO_TOTAL_BUF_SIZE) {
    start -= AUDIO_TOTAL_BUF_SIZE;
  }

  Audio_ZeroBufferRegion(haudio, start, missing);

  audio_underrun_silence_bytes += missing;

  if (audio_ctx.underrun_active == 0U) {
    audio_ctx.underrun_active = 1U;
    audio_underrun_events++;
  }
}
