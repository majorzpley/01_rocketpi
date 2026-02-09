<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-06 11:35:57
 * @FilePath: /09_rocketpi_uart_control_led_cjson/readme.md
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
- 使用cjson库来控制led亮灭
- 基于 rocketpi_uart_control_led 的串口协议，使用pio库中的 cJSON 官方库完成实际的 JSON 解析。
- 在 Core/Src/main.c 内实现一个阻塞式 UART 控制台，按命令批量控制 RocketPi 板载的三颗 LED (LED_B、LED_G、LED_P)。
- 上电后串口会打印 GPIO 映射、示例命令，并在每次处理完成后回显执行结果或错误 JSON。
# 四、注意事项
- 由于 cJSON 解析数组命令需要更多堆内存，需将 Heap_Size 调整为 0x800。若仍保持默认的 0x200，发送 {"led":["B","G","P"],"state":[1,0,1]} 等命令会因为 malloc 失败导致 invalid json。

- 需要在 platform.ini 文件中加入：
```ini
; add this folder to the linker paths so that ld script can be found
build_flags = -L.
; redirect ldscript
board_build.ldscript = STM32F401RETX_FLASH_CUSTOM.ld
```