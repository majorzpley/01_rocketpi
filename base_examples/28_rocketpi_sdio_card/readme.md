<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-26 13:45:16
 * @FilePath: /28_rocketpi_sdio_card/readme.md
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
面向 RocketPI STM32F401RE 开发板的 单线 SDIO MicroSD卡读写 演示工程。主要特性：
- 实现MicroSD卡的读写测试，可使用阻塞方式或DMA方式，只需一个宏定义即可选择
## 硬件连接
![](image.png)
- 插入tf卡
- 目前测试支持品牌（闪迪，朗科）

![alt text](image-1.png)
![alt text](662a5b70-9878-4181-9bf5-2a94035df349.png)