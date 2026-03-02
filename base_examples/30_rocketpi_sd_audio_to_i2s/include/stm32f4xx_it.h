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
extern void DMA1_Stream4_IRQHandler(void);
extern void SPI1_IRQHandler(void);
extern void SPI2_IRQHandler(void);
extern void SDIO_IRQHandler(void);
extern void DMA2_Stream3_IRQHandler(void);
extern void DMA2_Stream5_IRQHandler(void);
extern void DMA2_Stream6_IRQHandler(void);

#ifdef __cplusplus
}
#endif