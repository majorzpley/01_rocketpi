/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-07 14:45:05
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-07 14:45:22
 * @FilePath: /40_rocketpi_crc/lib/crc/crc16_sw.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
/*
 * Copyright (c) 2017 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crc.h"

uint16_t __weak crc16(uint16_t poly, uint16_t seed, const uint8_t *src,
                      size_t len) {
  uint16_t crc = seed;
  size_t i, j;

  for (i = 0; i < len; i++) {
    crc ^= (uint16_t)(src[i] << 8U);

    for (j = 0; j < 8; j++) {
      if (crc & 0x8000UL) {
        crc = (crc << 1U) ^ poly;
      } else {
        crc = crc << 1U;
      }
    }
  }

  return crc;
}

uint16_t __weak crc16_reflect(uint16_t poly, uint16_t seed, const uint8_t *src,
                              size_t len) {
  uint16_t crc = seed;
  size_t i, j;

  for (i = 0; i < len; i++) {
    crc ^= (uint16_t)src[i];

    for (j = 0; j < 8; j++) {
      if (crc & 0x0001UL) {
        crc = (crc >> 1U) ^ poly;
      } else {
        crc = crc >> 1U;
      }
    }
  }

  return crc;
}

uint16_t __weak crc16_ccitt(uint16_t seed, const uint8_t *src, size_t len) {
  for (; len > 0; len--) {
    uint8_t e, f;

    e = seed ^ *src;
    ++src;
    f = e ^ (e << 4);
    seed = (seed >> 8) ^ ((uint16_t)f << 8) ^ ((uint16_t)f << 3) ^
           ((uint16_t)f >> 4);
  }

  return seed;
}

uint16_t __weak crc16_itu_t(uint16_t seed, const uint8_t *src, size_t len) {
  for (; len > 0; len--) {
    seed = (seed >> 8U) | (seed << 8U);
    seed ^= *src;
    ++src;
    seed ^= (seed & 0xffU) >> 4U;
    seed ^= seed << 12U;
    seed ^= (seed & 0xffU) << 5U;
  }

  return seed;
}