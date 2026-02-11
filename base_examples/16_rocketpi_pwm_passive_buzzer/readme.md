<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-11 19:03:35
 * @FilePath: /16_rocketpi_pwm_passive_buzzer/readme.md
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
# 实验说明
面向 Rocket-PI STM32F401RE 开发板的 无源蜂鸣器 演示工程。主要特性：
- 实现无源蜂鸣器演奏音乐
- 蜂鸣器哔哔两声，接着开始演奏
硬件连接
![alt text](image.png)
# 实验演示
<audio controls>
<source src="./buzzer.m4a" type="audio/mpeg">
您的浏览器不支持 audio 标签。
</audio>

![alt text](QQ_1770807789664.png)