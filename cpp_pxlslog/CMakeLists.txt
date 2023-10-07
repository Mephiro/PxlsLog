cmake_minimum_required(VERSION 3.10)

project(PxlsLog VERSION 1.0)

add_executable(PxlsLog pxlslog.cxx)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(OpenCV_DIR "~/opencv-4.x/build")
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)

add_subdirectory(functions)

target_link_libraries(PxlsLog PUBLIC CanvasFunc)

target_include_directories(PxlsLog PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           "${PROJECT_SOURCE_DIR}/functions"
                           )
