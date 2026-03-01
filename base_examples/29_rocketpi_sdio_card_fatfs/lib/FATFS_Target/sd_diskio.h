/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-27 12:20:52
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-01 11:26:56
 * @FilePath: /29_rocketpi_sdio_card_fatfs/lib/FATFS_Target/sd_diskio.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __SD_DISKIO_H
#define __SD_DISKIO_H

/* Includes ------------------------------------------------------------------*/
#include "bsp_driver_sd.h"
#include "ff_gen_drv.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern const Diskio_drvTypeDef  SD_Driver;

#endif /* __SD_DISKIO_H */