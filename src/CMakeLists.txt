cmake_minimum_required(VERSION 3.22)

project(hippy)

add_definitions(-D_AFXDLL -D_WIN32_WINNT=_WIN32_WINNT_WINXP)
set(CMAKE_MFC_FLAG 2)

add_executable(hippy WIN32
    addrmng.h
    alu.h
    asmeditor.h
    baseWnd.h
    dasmwnd.h
    debugwnd.h
    device.h
    dialogs.h
    disassembler.h
    environment.h
    hippy.h
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
    device.cpp
    dialogs.cpp
    disassembler.cpp
    m6800.cpp
    main.cpp
    memdump.cpp
    registerwnd.cpp
    stackwnd.cpp
    xmlparser.cpp
    Hippy.rc
)

add_subdirectory(8279_dll)
add_subdirectory(asm)
add_subdirectory(eprom)
