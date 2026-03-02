/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-27 12:16:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-27 12:16:52
 * @FilePath: /29_rocketpi_sdio_card_fatfs/lib/FATFS/Target/fatfs_platform.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#include "stm32f4xx_hal.h"
/* Defines ------------------------------------------------------------------*/
#define SD_PRESENT               ((uint8_t)0x01)  /* also in bsp_driver_sd.h */
#define SD_NOT_PRESENT           ((uint8_t)0x00)  /* also in bsp_driver_sd.h */
#define SD_DETECT_PIN         GPIO_PIN_0
#define SD_DETECT_GPIO_PORT   GPIOC
/* Prototypes ---------------------------------------------------------------*/
uint8_t	BSP_PlatformIsDetected(void);