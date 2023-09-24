add_library(keypad SHARED
    8279.h
    8279.cpp
    keypad.cpp
    StdAfx.cpp
    keypad.h
    resource.h
    StdAfx.h
    ../device.h
    ../device.cpp
    keypad.def
    keypad.rc
    keypad.bmp
)
target_compile_definitions(keypad PRIVATE _USRDLL)
target_include_directories(keypad PUBLIC ..)
add_custom_command(TARGET keypad POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:keypad>" "${CMAKE_BINARY_DIR}/devices")
