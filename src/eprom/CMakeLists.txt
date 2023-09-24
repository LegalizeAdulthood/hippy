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
target_compile_definitions(eprom PRIVATE _USRDLL)
target_link_libraries(eprom PUBLIC device)
add_custom_command(TARGET eprom POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:eprom>" "${CMAKE_BINARY_DIR}/devices")
