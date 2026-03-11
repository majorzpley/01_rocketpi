<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-03-11 14:26:40
 * @FilePath: /02_don't_approach_me_alarm/readme.md
 * @Description: 
 * 不用客气，这是你应该谢的!
 * Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
-->
# 一、debug问题
遇到的问题可以参考这篇帖子：https://community.platformio.org/t/python-error-on-vscode-cannot-start-debug-session/53407/5<br>
- 开发分支新增了对 Python 3.14 的支持
```bash
pio upgrade --dev
```

# 二、PlatformIO 配合 clangd 插件解决方案
由于微软自带插件的智能扫描运行起来太慢，故采用此方案，参考此篇文章：https://blog.csdn.net/weixin_44434849/article/details/127539447

在 *platform.ini* 中添加
```ini
build_flags = -Ilib -Isrc
```
在命令行输入：
```bash
pio run -t compiledb
```
即可生成.json文件
# 三、实验说明
## 功能概述
- 红外遥控调整雷达mg58f18参数：感应距离阈值（越小越远，100~65000，步进 3000）与 OUT 输出保持时间（步进 1 秒）。
- **雷达 OUT 上升沿非阻塞蜂鸣三声，红外按键触发短促蜂鸣一声作为反馈。**
- 按键可将当前参数写入雷达模组 Flash，写入后自动查询并打印实际生效值。
- 串口输出关键状态：**雷达 OUT 电平跳变、参数设置结果、Flash 保存后的实测参数等。**
## 按键映射（HX1838/NEC 常用键值）
- CH- (0x45)：减小距离阈值（距离更远）。
- CH+ (0x47)：增大距离阈值（距离更近）。
- VOL+ (0x15)：增加 OUT 保持/延迟时间 1 秒。
- VOL- (0x07)：减少 OUT 保持/延迟时间 1 秒（最小 1000 ms）。
PLAY/PAUSE (0x43)：保存当前参数到雷达 Flash，并回读打印。
## 行为说明
- **红外按键被接收后，立即排入队列，由主循环处理，避免阻塞中断。**
- **蜂鸣器使用非阻塞状态机驱动**，不影响主循环。
- **雷达 OUT 电平变化会在串口打印 “OUT LOW/HIGH -> HIGH/LOW”，上升沿触发三声蜂鸣。**
## 硬件连接
![alt text](image.png)
![alt text](1770540443067.jpg)
## 使用方式
1) 编译并下载固件到板卡（工程入口 Core/Src/main.c 已调用 app_init/app_poll/app_on_exti）。
2) 上电后通过串口监视日志，使用NEC 遥控按上述键位调整参数。
3) 需要固化参数时按 PLAY/PAUSE，等待串口打印保存结果与回读值。
4) 雷达检测到目标时，OUT 置高并蜂鸣提示。
![alt text](image-1.png)