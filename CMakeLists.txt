INCLUDE_DIRECTORIES(${OpenCV_INCLUDE_DIR})
add_library(Protocol protocol.h) #nur fuer die IDE wichtig...
target_link_libraries(${FILTER_NAME} ${OpenCV_LIBS})

add_subdirectory(src)