/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-05 12:00:38
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-05 12:00:47
 * @FilePath: /36_rocketpi_w25qxx_littlefs/lib/littlefs2.11.2/lfs_flash_port.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
/**
 * @file lfs_flash_port.h
 * @brief STM32F401 internal Flash-backed block device glue for littlefs.
 */

#ifndef LFS_FLASH_PORT_H
#define LFS_FLASH_PORT_H

#include "lfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LFS_FLASH_PORT_START_ADDR   (0x08020000UL)  /* Sector 5 base */
#define LFS_FLASH_PORT_END_ADDR     (0x08080000UL)  /* One past sector 7 */
#define LFS_FLASH_PORT_BLOCK_SIZE   (0x20000UL)     /* 128 KiB sectors */
#define LFS_FLASH_PORT_BLOCK_COUNT  (3U)

extern const struct lfs_config lfs_flash_port_cfg;

int lfs_flash_port_format(lfs_t *lfs);
int lfs_flash_port_mount(lfs_t *lfs);
int lfs_flash_port_unmount(lfs_t *lfs);

#ifdef __cplusplus
}
#endif

#endif /* LFS_FLASH_PORT_H */