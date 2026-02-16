/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-15 22:52:28
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-15 22:52:35
 * @FilePath: /24_rocketpi_spi_lcd_speedtest/lib/st7789/fonts.h
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