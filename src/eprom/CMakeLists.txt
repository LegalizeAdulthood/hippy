add_library(eprom SHARED
    eprom.h
    eprom.cpp
    eprom.def
    eprom.rc
    StdAfx.h
    StdAfx.cpp
    Resource.h
    ../device.h
    ../device.cpp
    ../hippy.h
    res/eprom.rc2
)
target_compile_definitions(eprom PRIVATE _USRDLL)
target_include_directories(eprom PUBLIC ..)
