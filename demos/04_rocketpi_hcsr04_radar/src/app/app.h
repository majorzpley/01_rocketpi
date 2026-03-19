/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-03-19 10:03:57
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-19 10:04:04
 * @FilePath: /04_rocketpi_hcsr04_radar/src/app/app.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef APP_H
#define APP_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialise the radar demo application.
 */
void App_Init(void);

/**
 * @brief Perform one radar update step.
 *
 * Call this from the main loop after App_Init() to keep the
 * servo, sensor, and display synchronised.
 */
void App_Update(void);

/**
 * @brief Convenience helper that calls App_Init() and then
 *        repeatedly executes App_Update() forever.
 */
void App_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_H */