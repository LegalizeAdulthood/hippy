cmake_minimum_required(VERSION 3.22)

project(hippy)

find_package(wxwidgets CONFIG REQUIRED)

add_definitions(-D_AFXDLL -D_WIN32_WINNT=_WIN32_WINNT_WINXP)
set(CMAKE_MFC_FLAG 2)

add_library(device STATIC
    device.h
    device.cpp
    hippy.h
)
target_include_directories(device PUBLIC .)
target_link_libraries(device PUBLIC wx::base)

add_executable(hippy WIN32
    addrmng.h
    alu.h
    asmeditor.h
    baseWnd.h
    dasmwnd.h
    debugwnd.h
    dialogs.h
    disassembler.h
    environment.h
    m6800.h
    memdump.h
    registerwnd.h
    resource.h
    stackwnd.h
    xmlparser.h
    addrmng.cpp
    alu.cpp
    asmeditor.cpp
    baseWnd.cpp
    dasmwnd.cpp
    debugwnd.cpp
    dialogs.cpp
    disassembler.cpp
    m6800.cpp
    main.cpp
    memdump.cpp
    registerwnd.cpp
    stackwnd.cpp
    xmlparser.cpp
    Hippy.rc
    toolbar1.bmp
    toolbar2.bmp
    toolbar3.bmp
    vcpkg.json
)
target_link_libraries(hippy PUBLIC device)
set_property(TARGET hippy PROPERTY VS_DPI_AWARE "OFF")
add_custom_command(TARGET hippy POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/devices/device.xml" "${CMAKE_BINARY_DIR}/devices")

add_subdirectory(keypad)
add_subdirectory(asm)
add_subdirectory(eprom)
