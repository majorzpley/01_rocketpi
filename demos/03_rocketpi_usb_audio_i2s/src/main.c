/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-13 15:03:52
 * @FilePath: /03_rocketpi_usb_audio_i2s/src/main.c
 * @Description:此工程为rocketpi USB CDC 回环示例
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "i2s.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"

#include "driver_ws2812b.h"
#include "spectrum.h"
#include "st7789.h"
#include "usbd_audio.h"

#define WS2812_LED_COUNT 30U
#define WS2812_UPDATE_INTERVAL_MS 20U

#define WS2812_VIS_MODE_SPECTRUM_BAR 0U
#define WS2812_VIS_MODE_ENERGY_SWEEP 1U
#define WS2812_VIS_MODE_RAINBOW_GLOW 2U

#ifndef WS2812_VISUAL_MODE
#define WS2812_VISUAL_MODE WS2812_VIS_MODE_SPECTRUM_BAR
#endif

static uint8_t ws2812_ready = 0U;
static uint32_t ws2812_last_update_ms = 0U;

extern void SystemClock_Config(void);
static void Visualizer_UpdateStrip(void);
static void Visualizer_ModeSpectrumBar(uint32_t leds, uint32_t bins,
                                       uint16_t max_height);
static void Visualizer_ModeEnergySweep(uint32_t leds, uint32_t bins,
                                       uint16_t max_height);
static void Visualizer_ModeRainbowGlow(uint32_t leds, uint32_t bins,
                                       uint16_t max_height);
static void Visualizer_Color565ToRGB(uint16_t color565, uint8_t *r, uint8_t *g,
                                     uint8_t *b);
static void Visualizer_ColorWheel(uint32_t pos, uint32_t total, uint8_t *r,
                                  uint8_t *g, uint8_t *b);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_I2S2_Init();
  MY_USB_DEVICE_Init();
  MY_SPI1_Init();
  MY_USART2_UART_Init();
  MY_TIM3_Init();

  ST7789_Init();
  Spectrum_Init(USBD_AUDIO_FREQ);
  if (ws2812b_init(WS2812_LED_COUNT)) {
    ws2812_ready = 1U;
    ws2812b_fill(0U, 0U, 0U);
    (void)ws2812b_refresh();
  }
  while (1) {
    Spectrum_DrawIfDue();
    Visualizer_UpdateStrip();
  }
}

void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0}; // 初始化晶振参数结构体
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0}; // 初始化时钟参数结构体

  __HAL_RCC_PWR_CLK_ENABLE(); // 使能电源控制时钟
  __HAL_PWR_VOLTAGESCALING_CONFIG(
      PWR_REGULATOR_VOLTAGE_SCALE2); // 配置电压调节器电压缩放
  /*HSE输入频率：通常为 8MHz（外部晶振）
    PLL输出：(HSE / PLLM) × PLLN = (8 / 4) × 168 = 336MHz
    USB时钟： 336MHz / PLLQ = 336 / 7 = 48MHz*/
  // 初始化HSE晶振和PLL
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource =
      RCC_PLLSOURCE_HSE;            //* 使用外部晶振HSE作为PLL输入源
  RCC_OscInitStruct.PLL.PLLM = 4;   //* 分频系数
  RCC_OscInitStruct.PLL.PLLN = 168; //* 倍频系数
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; //* 主系统时钟分频系数
  RCC_OscInitStruct.PLL.PLLQ = 7;             //* USB时钟分频系数
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

static void Visualizer_UpdateStrip(void) {
  if ((ws2812_ready == 0U) || ws2812b_is_busy()) {
    return;
  }

  uint32_t now = HAL_GetTick();
  if ((now - ws2812_last_update_ms) < WS2812_UPDATE_INTERVAL_MS) {
    return;
  }
  ws2812_last_update_ms = now;

  uint32_t leds = ws2812b_get_led_count();
  uint32_t bins = Spectrum_GetBinCount();
  if ((leds == 0U) || (bins == 0U)) {
    return;
  }

  uint16_t max_height = Spectrum_GetMaxHeight();
  if (max_height == 0U) {
    max_height = 1U;
  }

  switch (WS2812_VISUAL_MODE) {
  case WS2812_VIS_MODE_SPECTRUM_BAR:
    Visualizer_ModeSpectrumBar(leds, bins, max_height);
    break;
  case WS2812_VIS_MODE_ENERGY_SWEEP:
    Visualizer_ModeEnergySweep(leds, bins, max_height);
    break;
  case WS2812_VIS_MODE_RAINBOW_GLOW:
    Visualizer_ModeRainbowGlow(leds, bins, max_height);
    break;
  default:
    Visualizer_ModeSpectrumBar(leds, bins, max_height);
    break;
  }

  (void)ws2812b_refresh();
}

static void Visualizer_ModeSpectrumBar(uint32_t leds, uint32_t bins,
                                       uint16_t max_height) {
  for (uint32_t led = 0U; led < leds; ++led) {
    uint32_t bin = (led * bins) / leds;
    uint16_t level = Spectrum_GetLevel(bin);
    uint16_t color565 = Spectrum_GetBinColor(bin);

    uint8_t r, g, b;
    Visualizer_Color565ToRGB(color565, &r, &g, &b);

    uint32_t scale = ((uint32_t)level * 255U) / max_height;
    r = (uint8_t)((r * scale) / 255U);
    g = (uint8_t)((g * scale) / 255U);
    b = (uint8_t)((b * scale) / 255U);

    (void)ws2812b_set_pixel((uint16_t)led, r, g, b);
  }
}

static void Visualizer_ModeEnergySweep(uint32_t leds, uint32_t bins,
                                       uint16_t max_height) {
  uint32_t sum = 0U;
  for (uint32_t bin = 0U; bin < bins; ++bin) {
    sum += Spectrum_GetLevel(bin);
  }
  uint32_t avg = (bins > 0U) ? (sum / bins) : 0U;
  uint32_t active_leds = (avg * leds) / max_height;
  if (active_leds > leds) {
    active_leds = leds;
  }

  for (uint32_t led = 0U; led < leds; ++led) {
    if (led < active_leds) {
      uint32_t bin = (led * bins) / leds;
      uint16_t color565 = Spectrum_GetBinColor(bin);
      uint8_t r, g, b;
      Visualizer_Color565ToRGB(color565, &r, &g, &b);
      (void)ws2812b_set_pixel((uint16_t)led, r, g, b);
    } else {
      (void)ws2812b_set_pixel((uint16_t)led, 0U, 0U, 0U);
    }
  }
}

static void Visualizer_ModeRainbowGlow(uint32_t leds, uint32_t bins,
                                       uint16_t max_height) {
  if (leds == 0U) {
    return;
  }

  for (uint32_t led = 0U; led < leds; ++led) {
    uint8_t r, g, b;
    Visualizer_ColorWheel(led, leds, &r, &g, &b);

    uint32_t bin = (led * bins) / leds;
    uint16_t level = Spectrum_GetLevel(bin);
    uint32_t scale = ((uint32_t)level * 255U) / max_height;
    r = (uint8_t)((r * scale) / 255U);
    g = (uint8_t)((g * scale) / 255U);
    b = (uint8_t)((b * scale) / 255U);

    (void)ws2812b_set_pixel((uint16_t)led, r, g, b);
  }
}

static void Visualizer_Color565ToRGB(uint16_t color565, uint8_t *r, uint8_t *g,
                                     uint8_t *b) {
  if (r != NULL) {
    *r = (uint8_t)(((color565 >> 11) & 0x1FU) << 3);
  }
  if (g != NULL) {
    *g = (uint8_t)(((color565 >> 5) & 0x3FU) << 2);
  }
  if (b != NULL) {
    *b = (uint8_t)((color565 & 0x1FU) << 3);
  }
}

static void Visualizer_ColorWheel(uint32_t pos, uint32_t total, uint8_t *r,
                                  uint8_t *g, uint8_t *b) {
  if ((total == 0U) || (r == NULL) || (g == NULL) || (b == NULL)) {
    return;
  }

  uint32_t angle = (pos * 1536U) / total;
  angle %= 1536U;
  uint32_t region = angle / 256U;
  uint32_t offset = angle % 256U;

  switch (region) {
  case 0:
    *r = 255U;
    *g = (uint8_t)offset;
    *b = 0U;
    break;
  case 1:
    *r = (uint8_t)(255U - offset);
    *g = 255U;
    *b = 0U;
    break;
  case 2:
    *r = 0U;
    *g = 255U;
    *b = (uint8_t)offset;
    break;
  case 3:
    *r = 0U;
    *g = (uint8_t)(255U - offset);
    *b = 255U;
    break;
  case 4:
    *r = (uint8_t)offset;
    *g = 0U;
    *b = 255U;
    break;
  default:
    *r = 255U;
    *g = 0U;
    *b = (uint8_t)(255U - offset);
    break;
  }
}