/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-11 14:40:43
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-13 16:41:11
 * @FilePath: /03_rocketpi_usb_audio_i2s/lib/USB_DEVICE_Target/usbd_conf.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __USBD_CONF__H__
#define __USBD_CONF__H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

#define USBD_MAX_NUM_INTERFACES     1U
#define USBD_MAX_NUM_CONFIGURATION     1U
#define USBD_MAX_STR_DESC_SIZ     512U
#define USBD_DEBUG_LEVEL     0U
#define USBD_LPM_ENABLED     0U
#define USBD_SELF_POWERED     1U
#define USBD_AUDIO_FREQ     16000U

#define DEVICE_FS 		0
#define DEVICE_HS 		1

/** Alias for memory allocation. */

#define USBD_malloc         (void *)USBD_static_malloc

/** Alias for memory release. */
#define USBD_free           USBD_static_free

/** Alias for memory set. */
#define USBD_memset         memset

/** Alias for memory copy. */
#define USBD_memcpy         memcpy

/** Alias for delay. */
#define USBD_Delay          HAL_Delay

/* DEBUG macros */

#if (USBD_DEBUG_LEVEL > 0)
#define USBD_UsrLog(...)    printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_UsrLog(...)
#endif /* (USBD_DEBUG_LEVEL > 0U) */

#if (USBD_DEBUG_LEVEL > 1)

#define USBD_ErrLog(...)    printf("ERROR: ");\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_ErrLog(...)
#endif /* (USBD_DEBUG_LEVEL > 1U) */

#if (USBD_DEBUG_LEVEL > 2)
#define USBD_DbgLog(...)    printf("DEBUG : ");\
                            printf(__VA_ARGS__);\
                            printf("\n");
#else
#define USBD_DbgLog(...)
#endif /* (USBD_DEBUG_LEVEL > 2U) */

void *USBD_static_malloc(uint32_t size);
void USBD_static_free(void *p);

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CONF__H__ */