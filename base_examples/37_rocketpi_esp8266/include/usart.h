#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart6;

extern void MY_USART2_UART_Init(void);
extern void MY_USART6_UART_Init(void);

#ifdef __cplusplus
}
#endif