add_library(8279 SHARED
    8279_dll.cpp
    8279_dll.def
    8279_dll.rc
    keypad.cpp
    StdAfx.cpp
    8279_dll.h
    keypad.h
    resource.h
    StdAfx.h
    ../device.h
    ../device.cpp
    keypad.bmp
    ../res/keypad.bmp
    ReadMe.txt
)
target_compile_definitions(8279 PRIVATE _USRDLL)
target_include_directories(8279 PUBLIC ..)
