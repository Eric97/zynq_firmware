cmake_minimum_required(VERSION 2.8.3)
project(regado_firmware)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}")

## Add support for C++11
add_definitions(-std=c++11)

# CMake build type
# Debug Release RelWithDebInfo MinSizeRel Coverage
set(CMAKE_BUILD_TYPE Release)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

# git
execute_process(
	COMMAND git describe --always --tags
	OUTPUT_VARIABLE git_tag
	OUTPUT_STRIP_TRAILING_WHITESPACE
	WORKING_DIRECTORY ${SRC_DIR}
	)
message(STATUS "firmware VERSION: ${git_tag}")

# print full c compiler version
execute_process(COMMAND ${CMAKE_C_COMPILER} --version
		OUTPUT_VARIABLE c_compiler_version
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
STRING(REGEX MATCH "[^\n]*" c_compiler_version_short ${c_compiler_version})
message(STATUS "C compiler: ${c_compiler_version_short}")

# print full c++ compiler version
execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
		OUTPUT_VARIABLE cxx_compiler_version
		OUTPUT_STRIP_TRAILING_WHITESPACE
		)
STRING(REGEX MATCH "[^\n]*" cxx_compiler_version_short ${cxx_compiler_version})
message(STATUS "C++ compiler: ${cxx_compiler_version_short}")

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
