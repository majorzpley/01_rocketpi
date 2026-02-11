<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 14:50:26
 * @FilePath: /14_rocketpi_i2c_aht30/readme.md
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
面向 RocketPI STM32F401RE 开发板的 AHT30温湿度 演示工程。主要特性：
- 驱动AHT30在串口上打印温湿度。
- 提供 driver_aht30 基础驱动。
- driver_aht30_test 直接调用测试，自主选择轮询时间。
- soft_i2c 提供通用GPIO bit-bang I2C主机，可复用到其它外设。
- driver_aht30_config.h 允许在软件/硬件I2C之间切换并集中定义GPIO与时序宏。
# 四、注意事项
## 软硬件i2c切换
![alt text](QQ_1770792495980.png)
## 硬件连接
![alt text](image.png)
# 五、实验现象
![alt text](QQ20260211-145002.gif)