#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define LCD_RST_Pin GPIO_PIN_1
#define LCD_RST_GPIO_Port GPIOC
#define LCD_DC_Pin GPIO_PIN_3
#define LCD_DC_GPIO_Port GPIOC
#define LCD_CS_Pin GPIO_PIN_4
#define LCD_CS_GPIO_Port GPIOA
#define LCD_BL_Pin GPIO_PIN_5
#define LCD_BL_GPIO_Port GPIOB

extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
