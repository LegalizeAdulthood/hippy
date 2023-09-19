add_library(keypad SHARED
    8279.h
    8279.cpp
    8279.def
    8279.rc
    keypad.cpp
    StdAfx.cpp
    keypad.h
    resource.h
    StdAfx.h
    ../device.h
    ../device.cpp
    keypad.bmp
    ../res/keypad.bmp
)
target_compile_definitions(keypad PRIVATE _USRDLL)
target_include_directories(keypad PUBLIC ..)
