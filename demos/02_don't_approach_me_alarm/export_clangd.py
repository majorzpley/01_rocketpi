'''
Author: majorzpley wyx1214844230@outlook.com
Date: 2026-02-27 23:51:54
LastEditors: majorzpley wyx1214844230@outlook.com
LastEditTime: 2026-03-11 13:26:16
FilePath: /02_don't_approach_me_alarm/export_clangd.py
Description: 自动生成compile_commands.json文件
不用客气，这是你应该谢的!
Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
'''
import os
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
# env.Replace(COMPILATIONDB_PATH=os.path.join("$BUILD_DIR", "compile_commands.json"))
env.Replace(COMPILATIONDB_PATH=os.path.join("./", "compile_commands.json"))

# 添加构建后钩子：在每次成功构建后自动生成 compile_commands.json
def after_build(source, target, env):
    print("构建完成，自动生成 compile_commands.json...")
    env.Execute("pio run -t compiledb")

# 注册钩子，在构建完成后执行
env.AddPostAction("buildprog", after_build)