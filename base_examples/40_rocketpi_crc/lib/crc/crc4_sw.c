/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-07 17:20:47
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-07 17:20:57
 * @FilePath: /40_rocketpi_crc/lib/crc/crc4_sw.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
/*
 * Copyright (c) 2023 Michal Morsisko
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "crc.h"

uint8_t __weak crc4(const uint8_t *src, size_t len, uint8_t polynomial,
                    uint8_t initial_value, bool reversed) {
  uint8_t crc = initial_value;
  size_t i, j, k;

  for (i = 0; i < len; i++) {
    for (j = 0; j < 2; j++) {
      crc ^= ((src[i] >> (4 * (1 - j))) & 0xf);

      for (k = 0; k < 4; k++) {
        if (reversed) {
          if (crc & 0x01) {
            crc = (crc >> 1) ^ polynomial;
          } else {
            crc >>= 1;
          }
        } else {
          if (crc & 0x8) {
            crc = (crc << 1) ^ polynomial;
          } else {
            crc <<= 1;
          }
        }
      }
    }
  }

  return crc & 0xF;
}

uint8_t __weak crc4_ti(uint8_t seed, const uint8_t *src, size_t len) {
  static const uint8_t lookup[8] = {0x03, 0x65, 0xcf, 0xa9,
                                    0xb8, 0xde, 0x74, 0x12};
  uint8_t index;

  for (size_t i = 0; i < len; i++) {
    for (size_t j = 0U; j < 2U; j++) {
      index = seed ^ ((src[i] >> (4 * (1 - j))) & 0xf);
      seed = (lookup[index >> 1] >> (1 - (index & 1)) * 4) & 0xf;
    }
  }

  return seed;
}