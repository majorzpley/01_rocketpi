'''
Author: majorzpley wyx1214844230@outlook.com
Date: 2026-02-20 19:54:19
LastEditors: majorzpley wyx1214844230@outlook.com
LastEditTime: 2026-02-23 12:19:10
FilePath: /26_rocketpi_spi_lcd_240x240_lvgl/import_custom_files.py
Description: 
不用客气，这是你应该谢的!
Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
'''
from pathlib import Path

Import("env")  # type: ignore
env = env  # type: ignore
# 别管上面这行，留着就行

build_dir = Path('$BUILD_DIR')
project_dir = Path('$PROJECT_DIR')

# 把文件夹整体递归加入编译
env.BuildSources(
    str(build_dir / 'lvgl' / 'porting'),
    str(project_dir / 'lib' / 'lvgl' / 'porting')  # 假设libx 是放在项目的lib 目录下
)


env.BuildSources(
    str(build_dir / 'lvgl' / 'demos'),
    str(project_dir / 'lib' / 'lvgl' / 'demos')  # 假设libx 是放在项目的lib 目录下
)


env.BuildSources(
    str(build_dir / 'lvgl' / 'src'),
    str(project_dir / 'lib' / 'lvgl' / 'src')  # 假设libx 是放在项目的lib 目录下
)