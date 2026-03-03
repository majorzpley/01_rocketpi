/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-27 12:10:50
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-28 23:16:41
 * @FilePath: /29_rocketpi_sdio_card_fatfs/lib/FATFS/App/fatfs.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "fatfs.h"

uint8_t retSD;  /* Return value for SD */
char SDPath[4]; /* SD logical drive path */
FATFS SDFatFS;  /* File system object for SD logical drive */
FIL SDFile;     /* File object for SD */

void MY_FATFS_Init(void) {
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);
}

/**
 * @brief  Gets Time from RTC
 * @param  None
 * @retval Time in DWORD
 */
DWORD get_fattime(void) { return 0; }
