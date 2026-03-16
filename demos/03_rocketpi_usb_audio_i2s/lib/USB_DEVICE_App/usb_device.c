/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-11 14:40:43
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-13 15:33:13
 * @FilePath: /03_rocketpi_usb_audio_i2s/lib/USB_DEVICE_App/usb_device.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "usb_device.h"
#include "usbd_audio.h"
#include "usbd_audio_if.h"
#include "usbd_core.h"
#include "usbd_desc.h"

/* USB Device Core handle declaration. */
USBD_HandleTypeDef hUsbDeviceFS;

/**
 * Init USB device Library, add supported class and start the library
 * @retval None
 */
void MY_USB_DEVICE_Init(void) {
  /* Init Device Library, add supported class and start the library. */
  if (USBD_Init(&hUsbDeviceFS, &FS_Desc, DEVICE_FS) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_AUDIO) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_AUDIO_RegisterInterface(&hUsbDeviceFS, &USBD_AUDIO_fops_FS) !=
      USBD_OK) {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
    Error_Handler();
  }
}