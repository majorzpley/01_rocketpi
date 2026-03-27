/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-27 12:37:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-27 12:37:48
 * @FilePath: /06_rocketpi_mqtt_ota/ota/src/app/app.h
 * @Description:
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved.
 */
#ifndef ROCKETPI_APP_H
#define ROCKETPI_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the RocketPi smart-home demo logic.
 */
void App_Init(void);

/**
 * @brief Periodic application task. Call inside the main loop.
 */
void App_Loop(void);

#ifdef __cplusplus
}
#endif

#endif /* ROCKETPI_APP_H */