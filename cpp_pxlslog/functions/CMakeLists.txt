add_library(CanvasFunc CanvasFunc.cxx)

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
include_directories (${OpenCV_INCLUDE_DIRS})
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBAV REQUIRED IMPORTED_TARGET
    libavformat
    libavcodec
    libswresample
    libswscale
    libavutil
    libavfilter
    libavdevice
    libpostproc
)

target_link_libraries(CanvasFunc PkgConfig::LIBAV Eigen3::Eigen OpenSSL::SSL ${OpenCV_LIBS})
