add_library(eprom SHARED
    eprom.h
    eprom.cpp
    eprom.def
    eprom.rc
    StdAfx.h
    StdAfx.cpp
    Resource.h
    res/eprom.rc2
)
set_target_properties(eprom PROPERTIES FOLDER "Devices")
target_compile_definitions(eprom PRIVATE _USRDLL)
target_link_libraries(eprom PUBLIC device wx::core)
add_custom_command(TARGET eprom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:eprom>" "${CMAKE_BINARY_DIR}/devices"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/devices/monitor.rom" "${CMAKE_BINARY_DIR}/devices"
)
add_dependencies(hippy eprom)
