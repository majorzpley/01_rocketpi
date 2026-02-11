/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-11 21:42:43
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 21:42:59
 * @FilePath: /18_rocketpi_pwm_motor/lib/motor_l9110/driver_motor_l9110_test.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
/**
 * @file driver_motor_l9110_test.h
 * @brief Non-blocking demo sequence for the L9110 driver.
 *  L9110 驱动的简单演示状态机（非阻塞）。
 */

#ifndef DRIVER_MOTOR_L9110_TEST_H
#define DRIVER_MOTOR_L9110_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

void motor_l9110_test_init(void);
/* 放在主循环中定期调用，驱动状态机前进 */
void motor_l9110_test_task(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_MOTOR_L9110_TEST_H */