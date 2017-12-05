# add the include directory of the filter
include_directories(../include)
# declare our filter
set(FILTER_NAME controller)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})

adtf_add_filter(${FILTER_NAME}
    stdafx.h
    c_controller.h
    c_controller.cpp
)

target_link_libraries(${FILTER_NAME} ${OpenCV_LIBS})

adtf_set_folder(${FILTER_NAME} controller) 

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})
