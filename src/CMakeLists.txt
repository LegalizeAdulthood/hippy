cmake_minimum_required(VERSION 3.22)

project(hippy)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

include(CTest)

find_package(wxwidgets CONFIG REQUIRED)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_definitions(-D_AFXDLL -D_WIN32_WINNT=_WIN32_WINNT_WIN7 -DWINVER=_WIN32_WINNT_WIN7)
set(CMAKE_MFC_FLAG 2)

add_library(device STATIC
    device.h
    device.cpp
    hippy.h
    devices/device.xml
)
set_target_properties(device PROPERTIES FOLDER "Devices")
target_include_directories(device PUBLIC .)
target_link_libraries(device PUBLIC wx::base)
add_custom_command(TARGET device POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/devices/device.xml" "${CMAKE_BINARY_DIR}/devices")

add_library(hippy-lib STATIC
    disassembler.h
    disassembler.cpp
    utils.h
    utils.cpp
)
set_target_properties(hippy-lib PROPERTIES FOLDER "Libraries")
target_include_directories(hippy-lib PUBLIC .)
target_link_libraries(hippy-lib PUBLIC wx::base)

add_executable(hippy WIN32
    addrmng.h
    addrmng.cpp
    alu.h
    alu.cpp
    asmeditor.h
    asmeditor.cpp
    baseWnd.h
    baseWnd.cpp
    dasmwnd.h
    dasmwnd.cpp
    debugwnd.h
    debugwnd.cpp
    dialogs.h
    dialogs.cpp
    environment.h
    m6800.h
    m6800.cpp
    main.cpp
    memdump.h
    memdump.cpp
    registerwnd.h
    registerwnd.cpp
    resource.h
    stackwnd.h
    stackwnd.cpp
    xmlparser.h
    xmlparser.cpp

    Hippy.rc
    toolbar1.bmp
    toolbar2.bmp
    toolbar3.bmp
    vcpkg.json
    ../.github/workflows/build.yaml
)
target_link_libraries(hippy PUBLIC device hippy-lib wx::core wx::xrc)
set_property(TARGET hippy PROPERTY VS_DPI_AWARE "OFF")
set_directory_properties(PROPERTIES VS_STARTUP_PROJECT hippy)

add_subdirectory(keypad)
add_subdirectory(asm)
add_subdirectory(eprom)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
