/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-08 11:03:13
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-08 11:03:57
 * @FilePath: /12_rocketpi_uart_easylogger/include/usart.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
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