/**
  ******************************************************************************
  * File Name          : mbedtls.h
  * Description        : This file provides code for the configuration
  *                      of the mbedtls instances.
  ******************************************************************************
  ******************************************************************************
   * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __mbedtls_H
#define __mbedtls_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "config/mbedtls_config.h"
/* MBEDTLS init function */
void MY_MBEDTLS_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__mbedtls_H */

