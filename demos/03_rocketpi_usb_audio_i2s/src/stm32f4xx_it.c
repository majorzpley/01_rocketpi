/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:29:26
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-11 15:58:01
 * @FilePath: /03_rocketpi_usb_audio_i2s/src/stm32f4xx_it.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "stm32f4xx_it.h"
#include "main.h"

extern PCD_HandleTypeDef hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef hdma_spi2_tx;
extern I2S_HandleTypeDef hi2s2;
extern DMA_HandleTypeDef hdma_spi1_tx;
extern SPI_HandleTypeDef hspi1;
extern DMA_HandleTypeDef hdma_tim3_ch4_up;
/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Pre-fetch fault, memory access fault.
 */
void BusFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void) {
  while (1) {
  }
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void) {}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void) {}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void) {}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void) { HAL_IncTick(); }

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
 * @brief This function handles DMA1 stream2 global interrupt.
 */
void DMA1_Stream2_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_tim3_ch4_up); }

/**
 * @brief This function handles DMA1 stream4 global interrupt.
 */
void DMA1_Stream4_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_spi2_tx); }

/**
 * @brief This function handles SPI1 global interrupt.
 */
void SPI1_IRQHandler(void) { HAL_SPI_IRQHandler(&hspi1); }

/**
 * @brief This function handles SPI2 global interrupt.
 */
void SPI2_IRQHandler(void) { HAL_I2S_IRQHandler(&hi2s2); }

/**
 * @brief This function handles DMA2 stream3 global interrupt.
 */
void DMA2_Stream3_IRQHandler(void) { HAL_DMA_IRQHandler(&hdma_spi1_tx); }

/**
 * @brief This function handles USB On The Go FS global interrupt.
 */
void OTG_FS_IRQHandler(void) { HAL_PCD_IRQHandler(&hpcd_USB_OTG_FS); }