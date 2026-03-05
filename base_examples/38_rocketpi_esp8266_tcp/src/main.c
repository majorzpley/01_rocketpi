/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-30 14:23:31
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-05 20:50:00
 * @FilePath: /38_rocketpi_esp8266_tcp/src/main.c
 * @Description:此工程为rocketpi模板工程
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */

#include "main.h"
#include "dma.h"
#include "gpio.h"
#include "usart.h"

#define MQTT_TRACE_ENABLED 1
#include "debug_driver.h"
#include "driver_esp8266_at.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#ifndef ESP8266_TCP_TEST_WIFI_SSID
#define ESP8266_TCP_TEST_WIFI_SSID "ASUS"
#endif
#ifndef ESP8266_TCP_TEST_WIFI_PASSWORD
#define ESP8266_TCP_TEST_WIFI_PASSWORD "12345wyx"
#endif
#ifndef ESP8266_TCP_TEST_REMOTE_HOST
#define ESP8266_TCP_TEST_REMOTE_HOST "192.168.1.77"
#endif
#ifndef ESP8266_TCP_TEST_REMOTE_PORT
#define ESP8266_TCP_TEST_REMOTE_PORT 8899U
#endif

static bool s_esp8266_tcp_link_ready = false;

static void SystemClock_Config(void);
static void esp8266_tcp_test_run(void);
static void esp8266_tcp_test_poll(void);
static void esp8266_tcp_print_ipd_payload(const esp8266_at_event_t *event);
static void esp8266_tcp_log_status(const char *label,
                                   esp8266_at_status_t status);
static void esp8266_tcp_print_ip_info(void);

int main(void) {
  HAL_Init();
  SystemClock_Config();
  MY_GPIO_Init();
  MY_DMA_Init();
  MY_USART2_UART_Init();
  MY_USART6_UART_Init();

  HAL_Delay(2000);
  esp8266_tcp_test_run();
  while (1) {
    esp8266_tcp_test_poll();
    HAL_Delay(10);
  }
}

static void SystemClock_Config(void) {
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

static void esp8266_tcp_log_status(const char *label,
                                   esp8266_at_status_t status) {
  if (label == NULL) {
    return;
  }
  printf("[ESP8266][TCP][%s] %s\r\n", label, esp8266_at_status_string(status));
}

static void esp8266_tcp_print_ip_info(void) {
  esp8266_at_ip_info_t ip_info;
  esp8266_at_status_t status = esp8266_at_query_ip_info(&ip_info);
  esp8266_tcp_log_status("CIPSTA?", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }

  printf("[ESP8266][IP] STA ip=%s gateway=%s netmask=%s\r\n",
         ip_info.station_ip, ip_info.station_gateway, ip_info.station_netmask);
  printf("[ESP8266][IP] AP  ip=%s gateway=%s netmask=%s\r\n", ip_info.softap_ip,
         ip_info.softap_gateway, ip_info.softap_netmask);
}

static void esp8266_tcp_print_ipd_payload(const esp8266_at_event_t *event) {
  if (event == NULL) {
    return;
  }

  const char *raw_line = event->raw_line;
  if (raw_line == NULL || raw_line[0] == '\0') {
    return;
  }

  const char *payload = NULL;
  unsigned int channel = 0U;
  unsigned int payload_length = 0U;
  bool channel_valid = false;
  bool length_valid = false;
  int payload_offset = 0;

  if (sscanf(raw_line, "+IPD,%u,%u:%n", &channel, &payload_length,
             &payload_offset) == 2) {
    channel_valid = true;
    length_valid = true;
    payload = raw_line + payload_offset;
  } else if (sscanf(raw_line, "+IPD,%u:%n", &payload_length, &payload_offset) ==
             1) {
    length_valid = true;
    payload = raw_line + payload_offset;
  } else {
    const char *colon = strchr(raw_line, ':');
    if (colon != NULL) {
      payload = colon + 1;
    }
  }

  if (payload == NULL || *payload == '\0') {
    printf("[ESP8266][TCP RX] (empty payload)\r\n");
    return;
  }

  char buffer[ESP8266_AT_MAX_LINE_LENGTH];
  size_t available = strlen(payload);
  size_t copy_len = available;

  if (length_valid && payload_length < copy_len) {
    copy_len = payload_length;
  }

  if (copy_len >= sizeof(buffer)) {
    copy_len = sizeof(buffer) - 1U;
  }

  memcpy(buffer, payload, copy_len);
  buffer[copy_len] = '\0';

  if (!length_valid) {
    payload_length = (unsigned int)copy_len;
  }

  if (channel_valid) {
    printf("[ESP8266][TCP RX][id=%u len=%u] %s\r\n", channel, payload_length,
           buffer);
  } else {
    printf("[ESP8266][TCP RX][len=%u] %s\r\n", payload_length, buffer);
  }
}

static void esp8266_tcp_test_run(void) {
  printf("\r\n=== ESP8266 TCP client test ===\r\n");
  s_esp8266_tcp_link_ready = false;

  esp8266_at_status_t status = esp8266_at_init();
  esp8266_tcp_log_status("init", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }

  esp8266_at_clear_events();

  status = esp8266_at_disable_echo(true);
  esp8266_tcp_log_status("ATE0", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }

  status = esp8266_at_set_wifi_mode(1U, false);
  esp8266_tcp_log_status("CWMODE", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }

  status = esp8266_at_connect_ap(ESP8266_TCP_TEST_WIFI_SSID,
                                 ESP8266_TCP_TEST_WIFI_PASSWORD, 20000U, false);
  esp8266_tcp_log_status("CWJAP", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }
  esp8266_tcp_print_ip_info();

  status = esp8266_at_send_command(ESP8266_AT_CMD_CIPMUX,
                                   ESP8266_AT_COMMAND_MODE_SET, "0",
                                   ESP8266_AT_DEFAULT_TIMEOUT_MS, false);
  esp8266_tcp_log_status("CIPMUX", status);
  if (status != ESP8266_AT_STATUS_OK) {
    return;
  }

  char start_args[ESP8266_AT_MAX_LINE_LENGTH];
  (void)snprintf(start_args, sizeof(start_args), "\"TCP\",\"%s\",%u",
                 ESP8266_TCP_TEST_REMOTE_HOST,
                 (unsigned int)ESP8266_TCP_TEST_REMOTE_PORT);

  status = esp8266_at_send_command(ESP8266_AT_CMD_CIPSTART,
                                   ESP8266_AT_COMMAND_MODE_SET, start_args,
                                   ESP8266_AT_DEFAULT_TIMEOUT_MS * 5U, false);
  esp8266_tcp_log_status("CIPSTART", status);
  if (status == ESP8266_AT_STATUS_OK) {
    s_esp8266_tcp_link_ready = true;
    printf("[ESP8266][TCP] connected to %s:%u\r\n",
           ESP8266_TCP_TEST_REMOTE_HOST,
           (unsigned int)ESP8266_TCP_TEST_REMOTE_PORT);
    printf("[ESP8266][TCP] waiting for incoming data...\r\n");
  }
}

static void esp8266_tcp_test_poll(void) {
  esp8266_at_event_t event;

  esp8266_at_poll();

  while (esp8266_at_fetch_event(&event)) {
    if (strncmp(event.raw_line, "+IPD", 4) == 0) {
      if (s_esp8266_tcp_link_ready) {
        esp8266_tcp_print_ipd_payload(&event);
      }
      continue;
    }

    if (strstr(event.raw_line, "CLOSED") != NULL) {
      if (s_esp8266_tcp_link_ready) {
        printf("[ESP8266][TCP] connection closed\r\n");
      }
      s_esp8266_tcp_link_ready = false;
      continue;
    }
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