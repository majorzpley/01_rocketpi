/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-04 13:31:26
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-04 14:29:43
 * @FilePath: /33_rocketpi_usb_msc/lib/USB_DEVICE_App/usb_device.c
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#include "usb_device.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_msc.h"
#include "usbd_storage_if.h"

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
  if (USBD_RegisterClass(&hUsbDeviceFS, &USBD_MSC) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_MSC_RegisterStorage(&hUsbDeviceFS,
                               &USBD_Storage_Interface_fops_FS) != USBD_OK) {
    Error_Handler();
  }
  if (USBD_Start(&hUsbDeviceFS) != USBD_OK) {
    Error_Handler();
  }
}