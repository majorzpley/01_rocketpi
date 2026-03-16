#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_audio.h"

extern USBD_AUDIO_ItfTypeDef USBD_AUDIO_fops_FS;

extern void TransferComplete_CallBack_FS(void);

extern void HalfTransfer_CallBack_FS(void);

#ifdef __cplusplus
}
#endif