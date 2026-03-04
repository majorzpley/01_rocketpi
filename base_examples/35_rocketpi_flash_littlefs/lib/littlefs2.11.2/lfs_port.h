/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-04 22:05:07
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-04 22:05:19
 * @FilePath: /35_rocketpi_flash_littlefs/lib/littlefs2.11.2/lfs_port.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */

/**
 * @file lfs_port.h
 * @brief STM32F401 flash-backed block device glue for littlefs.
 */

#ifndef LFS_PORT_H
#define LFS_PORT_H

#include "lfs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LFS_PORT_FLASH_START_ADDR   (0x08040000UL)  /* Sector 6 base */
#define LFS_PORT_FLASH_END_ADDR     (0x08080000UL)  /* One past sector 7 */
#define LFS_PORT_FLASH_BLOCK_SIZE   (0x20000UL)     /* 128 KiB sectors */
#define LFS_PORT_FLASH_BLOCK_COUNT  (2U)

extern const struct lfs_config lfs_port_cfg;

int lfs_port_format(lfs_t *lfs);
int lfs_port_mount(lfs_t *lfs);
int lfs_port_unmount(lfs_t *lfs);

#ifdef __cplusplus
}
#endif

#endif /* LFS_PORT_H */