/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-15 21:57:56
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-15 21:58:04
 * @FilePath: /23_rocketpi_ws2812b/lib/ws2812b/driver_ws2812b_test.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include <stdint.h>

#include "driver_ws2812b.h"

/**
 * @file driver_ws2812b_test.h
 * @brief Ready-to-use pattern helpers for WS2812B demo sequences.
 * @author rocket
 */

/**
 * @brief Simple on/off blinking pattern across the entire strip.
 * @param red Red intensity (0-255).
 * @param green Green intensity (0-255).
 * @param blue Blue intensity (0-255).
 * @param on_time_ms Time to stay lit per cycle (milliseconds).
 * @param off_time_ms Time to stay dark per cycle (milliseconds).
 * @param duration_ms Total runtime of the pattern (0 => run forever).
 */
void ws2812b_test_blink(uint8_t red, uint8_t green, uint8_t blue,
                        uint32_t on_time_ms, uint32_t off_time_ms, uint32_t duration_ms);

/**
 * @brief Single-pixel chase effect that marches along the strip.
 * @param red Red intensity (0-255).
 * @param green Green intensity (0-255).
 * @param blue Blue intensity (0-255).
 * @param step_delay_ms Delay between pixel moves (milliseconds).
 * @param duration_ms Total runtime (0 => run forever).
 */
void ws2812b_test_chase(uint8_t red, uint8_t green, uint8_t blue,
                        uint32_t step_delay_ms, uint32_t duration_ms);

/**
 * @brief Continuous rainbow cycle shifting through the strip.
 * @param step_delay_ms Delay between hue shifts (milliseconds).
 * @param duration_ms Total runtime (0 => run forever).
 */
void ws2812b_test_rainbow(uint32_t step_delay_ms, uint32_t duration_ms);

/**
 * @brief "Breathing" effect that fades a color in and out.
 * @param red Red intensity (0-255).
 * @param green Green intensity (0-255).
 * @param blue Blue intensity (0-255).
 * @param step_delay_ms Delay between brightness steps (milliseconds).
 * @param duration_ms Total runtime (0 => run forever).
 */
void ws2812b_test_breathe(uint8_t red, uint8_t green, uint8_t blue,
                          uint32_t step_delay_ms, uint32_t duration_ms);

/**
 * @brief Theater-style sparse chase (every third pixel lit).
 * @param red Red intensity (0-255).
 * @param green Green intensity (0-255).
 * @param blue Blue intensity (0-255).
 * @param step_delay_ms Delay before shifting the offsets (milliseconds).
 * @param duration_ms Total runtime (0 => run forever).
 */
void ws2812b_test_theater_chase(uint8_t red, uint8_t green, uint8_t blue,
                                uint32_t step_delay_ms, uint32_t duration_ms);

/**
 * @brief Progressive wipe that blends from a start color to an end color.
 * @param start_r Start color red component (0-255).
 * @param start_g Start color green component (0-255).
 * @param start_b Start color blue component (0-255).
 * @param end_r Target color red component (0-255).
 * @param end_g Target color green component (0-255).
 * @param end_b Target color blue component (0-255).
 * @param step_delay_ms Delay between wipe steps (milliseconds).
 * @param duration_ms Total runtime (0 => run forever).
 */
void ws2812b_test_gradient_wipe(uint8_t start_r, uint8_t start_g, uint8_t start_b,
                                uint8_t end_r, uint8_t end_g, uint8_t end_b,
                                uint32_t step_delay_ms, uint32_t duration_ms);