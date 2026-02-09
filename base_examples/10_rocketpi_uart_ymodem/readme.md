<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-07 15:00:07
 * @FilePath: /10_rocketpi_uart_ymodem/readme.md
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
- 测试ymodem接收功能时，需要将 main 文件中的 **"#define YMODEM_MODE_SEND"** 宏打开，暂时有问题
- 测试ymodem发送功能时，需要将 main 文件中的 **"#define YMODEM_MODE_SEND"** 宏关闭，接收文件为工程目录下的 "rocketpi_demo.txt"。

# 四、注意事项
- 需要在 platform.ini 文件中加入：
```ini
; add this folder to the linker paths so that ld script can be found
build_flags = -L.
; redirect ldscript
board_build.ldscript = STM32F401RETX_FLASH_CUSTOM.ld
```
