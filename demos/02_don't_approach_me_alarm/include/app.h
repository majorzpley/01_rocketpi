#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern void app_init(void);
extern void app_poll(void);
extern void app_on_exti(uint16_t gpio_pin);

#ifdef __cplusplus
}
#endif