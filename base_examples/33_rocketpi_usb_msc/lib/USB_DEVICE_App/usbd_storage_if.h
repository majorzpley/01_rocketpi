#ifndef __USBD_STORAGE_IF_H__
#define __USBD_STORAGE_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_msc.h"

/** STORAGE Interface callback. */
extern USBD_StorageTypeDef USBD_Storage_Interface_fops_FS;

void MSC_FlashStorage_Init(void);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_STORAGE_IF_H__ */