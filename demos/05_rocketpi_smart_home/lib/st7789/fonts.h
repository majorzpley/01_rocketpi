/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-19 15:14:51
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-19 15:14:58
 * @FilePath: /05_rocketpi_smart_home/lib/st7789/fonts.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __FONTS_H__
#define __FONTS_H__
 
#include <stdint.h>
 
typedef struct {
    const uint8_t width;
    uint8_t height;
    const uint16_t *data;
} FontDef;

extern FontDef Font16x24;


 
#endif // __FONTS_H__