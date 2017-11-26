# add the include directory of the filter
include_directories(../include)
# declare our filter
set(FILTER_NAME UltrasonicLogic)

adtf_add_filter(${FILTER_NAME}
    stdafx.h
        ultrasonicLogic.h
        ultrasonicLogic.cpp
)

adtf_set_folder(${FILTER_NAME} ultrasonicLogic)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
