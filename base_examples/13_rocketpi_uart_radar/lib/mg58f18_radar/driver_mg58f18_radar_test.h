/*
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-02-08 16:58:54
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-08 17:00:13
 * @FilePath: /13_rocketpi_uart_radar/lib/mg58f18_radar/driver_mg58f18_radar_test.h
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
 */
/**
 * @file driver_mg58f18_radar_test.h
 * @brief MG58F18 雷达驱动的测试入口。
 */

#ifndef DRIVER_MG58F18_RADAR_TEST_H
#define DRIVER_MG58F18_RADAR_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/** 执行一轮完整的协议交互测试并打印结果。 */
void mg58f18_radar_test_run(void);
/** 持续轮询并打印串口数据与 OUT 引脚变化。 */
void mg58f18_radar_test_poll(void);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_MG58F18_RADAR_TEST_H */