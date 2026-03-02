'''
Author: majorzpley wyx1214844230@outlook.com
Date: 2026-02-18 10:25:17
LastEditors: majorzpley wyx1214844230@outlook.com
LastEditTime: 2026-02-27 11:34:17
FilePath: /29_rocketpi_sdio_card_fatfs/export_hex.py
Description: 将bin文件生成hex文件
不用客气，这是你应该谢的!
Copyright (c) 2026 by ${git_name_email}, All Rights Reserved. 
'''
Import("env")
 
# # Custom HEX from ELF
 
env.AddPostAction(
 
    "$BUILD_DIR/${PROGNAME}.elf",
 
    env.VerboseAction(" ".join([
 
        "$OBJCOPY", "-O", "ihex", "-R", ".eeprom",
 
        '"$BUILD_DIR/${PROGNAME}.elf"', '"$BUILD_DIR/${PROGNAME}.hex"'  # 加个单引号
 
    ]), "Building $BUILD_DIR/${PROGNAME}.hex")
 
)