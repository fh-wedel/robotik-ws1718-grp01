# add the include directory of the filter
include_directories(../include)

# declare our filter
set(FILTER_NAME OneLineDetect)

INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})


adtf_add_filter(${FILTER_NAME}
    stdafx.h
        oneLineDetect.h
        oneLineDetect.cpp
)

target_link_libraries(${FILTER_NAME} ${OpenCV_LIBS})

adtf_set_folder(${FILTER_NAME} OneLineDetect)

# Specify where it should be installed to
adtf_install_plugin(${FILTER_NAME} ${CMAKE_INSTALL_BINARY})

if(WIN32)
    install(FILES "${OpenCV_DIR}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin/opencv_world${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}d.dll" DESTINATION ${CMAKE_INSTALL_BINARY} CONFIGURATIONS Debug)
    install(FILES "${OpenCV_DIR}/${OpenCV_ARCH}/${OpenCV_RUNTIME}/bin/opencv_world${OpenCV_VERSION_MAJOR}${OpenCV_VERSION_MINOR}${OpenCV_VERSION_PATCH}.dll" DESTINATION ${CMAKE_INSTALL_BINARY} CONFIGURATIONS RelWithDebInfo Release)

    install(FILES "${Qt_DIR}/bin/QtTest4.dll" DESTINATION ${CMAKE_INSTALL_BINARY} CONFIGURATIONS Release RelWithDebInfo)
    install(FILES "${Qt_DIR}/bin/QtTestd4.dll" DESTINATION ${CMAKE_INSTALL_BINARY} CONFIGURATIONS Debug)
endif(WIN32)

#test
