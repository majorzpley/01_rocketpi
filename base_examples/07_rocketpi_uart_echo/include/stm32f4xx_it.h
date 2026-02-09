/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-04 22:16:07
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-05 12:38:06
 * @FilePath: /07_rocketpi_uart_echo/include/stm32f4xx_it.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
extern void DMA1_Stream5_IRQHandler(void);
extern void DMA1_Stream6_IRQHandler(void);
extern void USART2_IRQHandler(void);

#ifdef __cplusplus
}
#endif