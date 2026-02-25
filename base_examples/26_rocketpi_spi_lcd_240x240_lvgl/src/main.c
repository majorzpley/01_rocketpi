/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-25 19:56:52
 * @FilePath: /26_rocketpi_spi_lcd_240x240_lvgl/src/main.c
 * @Description:移植lvgl9.2 并适配遥杆作为输入
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"

#include "demos/lv_demos.h"
#include "lv_conf.h"
#include "lvgl/porting/lv_port_disp.h"
#include "lvgl/porting/lv_port_indev.h"
#include "st7789.h"

#define ENABLE_ST7789_FPS_TEST 0

// todo 声明图片描述
//  LV_IMAGE_DECLARE(rgb565_grid_240x240);
LV_IMAGE_DECLARE(JayChou_rgb565_grid_240x240);

extern void SystemClock_Config(void);
extern void ui_show_image(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_USART2_UART_Init();
  MY_SPI1_Init();
  MY_ADC1_Init();
  MY_TIM3_Init();

#if ENABLE_ST7789_FPS_TEST
  ST7789_Init();
  ST7789_TestFrameRate(); /*帧率测试*/
#endif

  lv_init();
  lv_port_disp_init();  /*lcd初始化*/
  lv_port_indev_init(); /*输入设备摇杆初始化*/

  // todo lvgl官方提供的demos
  // lv_demo_widgets();
  // lv_demo_stress();
  lv_demo_keypad_encoder(); /* test input */
  // lv_demo_scroll();
  // todo 显示周杰伦七里香专辑
  // ui_show_image();

  lv_indev_t *keypad = lv_port_indev_get_keypad();
  lv_group_t *group = lv_group_get_default();
  if ((keypad != NULL) && (group != NULL)) {
    lv_indev_set_group(keypad, group);
  }
  while (1) {
    lv_timer_handler();
    HAL_Delay(1);
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // 初始化晶振参数结构体
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // 初始化时钟参数结构体

  __HAL_RCC_PWR_CLK_ENABLE(); // 使能电源控制时钟
  __HAL_PWR_VOLTAGESCALING_CONFIG(
      PWR_REGULATOR_VOLTAGE_SCALE2); // 配置电压调节器电压缩放

  // 初始化HSE晶振和PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource =
      RCC_PLLSOURCE_HSE;           //* 使用外部晶振HSE作为PLL输入源
  RCC_OscInitStruct.PLL.PLLM = 4;  //* 分频系数
  RCC_OscInitStruct.PLL.PLLN = 84; //* 倍频系数
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; //* 主系统时钟分频系数
  RCC_OscInitStruct.PLL.PLLQ = 4;             //* USB时钟分频系数
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }

  // 初始化系统时钟
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                RCC_CLOCKTYPE_PCLK1 |
                                RCC_CLOCKTYPE_PCLK2; // 配置时钟类型
  RCC_ClkInitStruct.SYSCLKSource =
      RCC_SYSCLKSOURCE_PLLCLK; //* 使用PLL输出作为系统时钟源
  RCC_ClkInitStruct.AHBCLKDivider =
      RCC_SYSCLK_DIV1; //* AHB时钟不分频，高性能总线用于CPU、内存、DMA等，影响CPU执行速度、DMA传输速度
  RCC_ClkInitStruct.APB1CLKDivider =
      RCC_HCLK_DIV2; //* APB1时钟分频2，低速总线用于定时器、UART、I2C、SPI等
  RCC_ClkInitStruct.APB2CLKDivider =
      RCC_HCLK_DIV1; //* APB2时钟不分频，高速总线用于ADC、USART、高级定时器等

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) !=
      HAL_OK) { //* 设置FLASH延时周期
    Error_Handler();
  }
}
void Error_Handler(void) {
  __disable_irq();
  while (1) {
  }
}
#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */

void ui_show_image(void) {
  lv_obj_t *img = lv_image_create(lv_screen_active());
  // lv_image_set_src(img, &rgb565_grid_240x240);
  lv_image_set_src(img, &JayChou_rgb565_grid_240x240);
  lv_obj_center(img);
}