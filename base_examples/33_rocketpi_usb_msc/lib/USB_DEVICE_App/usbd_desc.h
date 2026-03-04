/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-03 19:30:29
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-03 19:30:53
 * @FilePath: /32_rocketpi_usb_cdc/lib/USB_DEVICE/App/usbd_desc.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __USBD_DESC__C__
#define __USBD_DESC__C__

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define  USB_SIZ_STRING_SERIAL       0x1A

extern USBD_DescriptorsTypeDef FS_Desc;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_DESC__C__ */