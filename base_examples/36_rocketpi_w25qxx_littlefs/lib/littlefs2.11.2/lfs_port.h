/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-05 11:54:10
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-05 11:57:50
 * @FilePath: /36_rocketpi_w25qxx_littlefs/lib/littlefs2.11.2/lfs_port.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */

/**
 * @file lfs_port.h
 * @brief Compatibility shim kept for existing includes. Prefer lfs_flash_port.h.
 */

#ifndef LFS_PORT_H
#define LFS_PORT_H

#include "lfs_flash_port.h"

#ifdef __cplusplus
extern "C" {
#endif

#define lfs_port_cfg lfs_flash_port_cfg

static inline int lfs_port_format(lfs_t *lfs)
{
    return lfs_flash_port_format(lfs);
}

static inline int lfs_port_mount(lfs_t *lfs)
{
    return lfs_flash_port_mount(lfs);
}

static inline int lfs_port_unmount(lfs_t *lfs)
{
    return lfs_flash_port_unmount(lfs);
}

#ifdef __cplusplus
}
#endif

#endif /* LFS_PORT_H */