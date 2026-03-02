#include "fatfs_platform.h"

uint8_t BSP_PlatformIsDetected(void) {
  uint8_t status = SD_PRESENT;
  /* Check SD card detect pin */
  if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_PORT, SD_DETECT_PIN) != GPIO_PIN_RESET) {
    status = SD_NOT_PRESENT;
  }
  return status;
}