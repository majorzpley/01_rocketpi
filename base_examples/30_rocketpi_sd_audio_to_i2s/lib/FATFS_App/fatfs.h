/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-27 12:11:45
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-01 11:26:27
 * @FilePath: /29_rocketpi_sdio_card_fatfs/lib/FATFS_App/fatfs.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __fatfs_H
#define __fatfs_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "ff.h"
#include "ff_gen_drv.h"
#include "sd_diskio.h" /* defines SD_Driver as external */

extern uint8_t retSD; /* Return value for SD */
extern char SDPath[4]; /* SD logical drive path */
extern FATFS SDFatFS; /* File system object for SD logical drive */
extern FIL SDFile; /* File object for SD */

extern void MY_FATFS_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__fatfs_H */