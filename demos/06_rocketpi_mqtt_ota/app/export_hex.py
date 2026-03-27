'''
Author: majorzpley wyx1214844230@outlook.com
Date: 2026-03-25 08:05:34
LastEditors: majorzpley wyx1214844230@outlook.com
LastEditTime: 2026-03-25 08:05:44
FilePath: /06_rocketpi_mqtt_ota/app/export_hex.py
Description: 
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