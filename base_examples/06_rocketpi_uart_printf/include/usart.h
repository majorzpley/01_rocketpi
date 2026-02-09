/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-04 21:46:53
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-04 21:48:49
 * @FilePath: /06_rocketpi_uart_printf/include/usart.h
 * @Description:
 * 不用客气，这是你应该谢的!
 */
#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern UART_HandleTypeDef huart2;

extern void MY_USART2_UART_Init(void);

#ifdef __cplusplus
}
#endif