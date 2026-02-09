<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-04 21:30:45
 * @FilePath: /05_rocketpi_delay_us/readme.md
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
# 三、实验现象
1. 使用HAL库延时1us实际测得的数据：约为950ns到1.1us之间
![alt text](<屏幕截图 2026-02-04 105914.png>)
2. 使用寄存器翻转PC10：貌似是在1.1us->950ns->1us循环变化，和HAL库相比变化不大
3. 使用内联汇编翻转PC10，并延时1us：正好1us
![alt text](<屏幕截图 2026-02-04 212933.png>)