add_executable(eprom WIN32
    eprom.h
    eprom.cpp
    StdAfx.h
    StdAfx.cpp
    Resource.h
    eprom.rc
    ../device.h
    ../device.cpp
    ../hippy.h
    res/eprom.rc2
)
target_include_directories(eprom PUBLIC ..)
