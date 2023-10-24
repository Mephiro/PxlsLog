cmake_minimum_required(VERSION 3.10)

project(PxlsLog VERSION 1.0)

add_executable(PxlsLog pxlslog.cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_subdirectory(functions)

target_link_libraries(PxlsLog PUBLIC CanvasFunc)

target_include_directories(PxlsLog PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/functions"
                           )
