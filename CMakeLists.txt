cmake_minimum_required(VERSION 2.8.3)
project(regado_firmware)

## Add support for C++11
add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE Release)

set(REGADO_SRC 
	src/common/common.cpp 
	src/main.cpp
	src/task_thread/task_thread.cpp 
	src/comm_fpga/task_readDDR.cpp 
	src/ransac/task_ransac.cpp
	src/ransac/matrix.cpp
	src/ransac/pose_compute.cpp
	src/dlg/task_dlg.cpp
)

add_executable(${PROJECT_NAME} ${REGADO_SRC})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} pthread)
