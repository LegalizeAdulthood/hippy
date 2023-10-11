add_library(keypad SHARED
    8279.h
    8279.cpp
    keypad.cpp
    StdAfx.cpp
    keypad.h
    resource.h
    StdAfx.h
    keypad.def
    keypad.rc
    keypad.bmp
    keypad.png
)
set_target_properties(keypad PROPERTIES FOLDER "Devices")
target_compile_definitions(keypad PRIVATE _USRDLL)
target_link_libraries(keypad PUBLIC device wx::core)
add_custom_command(TARGET keypad POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:keypad>" "${CMAKE_BINARY_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/keypad.png" "${CMAKE_BINARY_DIR}/devices"
)
add_dependencies(hippy keypad)
