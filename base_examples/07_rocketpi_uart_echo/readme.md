<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-05 14:21:32
 * @FilePath: /07_rocketpi_uart_echo/readme.md
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
- 使用 USART2 以 115200-8-N-1 配置，通过 DMA 空闲中断持续接收数据并立即回显发送。
- 上电后自动启动 DMA 接收，并在串口输出提示 RocketPi UART echo ready. 告知用户示例状态。
- 任意字符（含换行、中文等）都会按照接收顺序原样返回，可用来验证串口连线或上位机发送功能。
- 当出现串口错误（溢出、噪声等）时自动重新启动 DMA，确保回显功能持续可用。
# 四、实验现象
![alt text](<屏幕截图 2026-02-05 142103.png>)