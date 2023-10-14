find_package(WXRC REQUIRED)

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
    keypad.xrc
    ${CMAKE_CURRENT_BINARY_DIR}/resources.cpp
)
add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp"
    MAIN_DEPENDENCY "${CMAKE_CURRENT_SOURCE_DIR}/keypad.xrc"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    COMMAND "${WXRC_COMMAND}" "${CMAKE_CURRENT_SOURCE_DIR}/keypad.xrc" -v -c -o "${CMAKE_CURRENT_BINARY_DIR}/resources.cpp"
)
set_target_properties(keypad PROPERTIES FOLDER "Devices")
target_compile_definitions(keypad PRIVATE _USRDLL)
target_link_libraries(keypad PUBLIC device wx::core wx::xrc)
add_custom_command(TARGET keypad POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:keypad>" "${CMAKE_BINARY_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:keypad> "${CMAKE_BINARY_DIR}/devices" COMMAND_EXPAND_LISTS
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/keypad.png" "${CMAKE_BINARY_DIR}/devices"
)
add_dependencies(hippy keypad)
