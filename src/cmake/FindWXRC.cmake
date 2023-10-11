include(FindPackageHandleStandardArgs)

find_program(WXRC_COMMAND
    wxrc
    HINTS "${wxWidgets_ROOT_DIR}/tools/wxwidgets"
)
find_package_handle_standard_args(XRC REQUIRED_VARS WXRC_COMMAND)
