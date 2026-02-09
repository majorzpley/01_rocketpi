<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-07 16:57:04
 * @FilePath: /11_rocketpi_uart_shell_microrl/readme.md
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
集成 microrl 命令行库，在 USART2（115200 8N1）上提供交互式 shell，并扩展了简单的板载功能控制。
1. shell 接口
- 通过串口工具连接 USART2，复位后会显示 IRin > 提示符。
- 支持 help / version / echo 等 microrl 默认命令。
- 自持自动补全以及历史命令回看
2. LED 控制命令
- 实现 led <blue|green|pink> <on|off|toggle> 指令，可在 shell 中直接点亮/熄灭/翻转三色 LED（LED_B/LED_G/LED_P 引脚）。
- 命令支持大小写混输，错误输入会返回提示。
3. 按键回显优化
- 接收中断使用环形缓冲，兼容 CR / CRLF 终端换行，保证回显与命令解析稳定。
# 四、注意事项
- 需要在 platform.ini 文件中加入：
```ini
; add this folder to the linker paths so that ld script can be found
build_flags = -L.
; redirect ldscript
board_build.ldscript = STM32F401RETX_FLASH_CUSTOM.ld
```
# 五、volatile关键字
- volatile 关键字用于告诉编译器某个变量的值可能会在程序的控制之外被改变，应该每次使用时都从**内存**中重新读取，而**不是使用缓存在寄存器中的值**。
- 主要作用：
    - 防止编译器优化
        - 编译器不会对volatile变量进行激进的优化
        - 每次访问都会生成对应的内存操作指令
    - 处理硬件寄存器
        - 硬件寄存器的值可能被外部硬件改变
        - 如GPIO、UART、定时器等硬件外设的寄存器
    - 多线程/中断共享变量
        - 中断服务程序(ISR)可能改变变量值
        - 多任务环境中的共享变量
    - 内存映射I/O
        - 涉及直接硬件访问的变量
# 六、实验说明
可参考下图：
![alt text](QQ_1770454603867.png)