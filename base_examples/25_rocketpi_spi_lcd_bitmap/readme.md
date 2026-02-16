<!--
 * @Author: majorzpley wyx1214844230@outlook.com
 * @Date: 2026-01-31 10:45:41
 * @LastEditors: majorzpley wyx1214844230@outlook.com
 * @LastEditTime: 2026-02-16 13:45:14
 * @FilePath: /25_rocketpi_spi_lcd_bitmap/readme.md
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
- 使用取模工具https://imagetools.rocketpi.club/ 取模图片绘制位图
使用方法
此工具的算法取自lvgl官方脚本,删除了lvgl一些冗余信息
![alt text](image.png)
# 实验现象
![alt text](image-1.png)
手机拍不清楚，见谅哈<br>
<img src="IMG_20260216_133855.jpg" alt="alt text" style="transform: rotate(deg);">
图片素材:<br>
![alt text](JayChou.png)