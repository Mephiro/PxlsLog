add_library(CanvasFunc CanvasFunc.cxx)

set(OpenCV_DIR "~/opencv-4.x/build")

find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
find_package(OpenSSL REQUIRED)

target_link_libraries(CanvasFunc Eigen3::Eigen OpenSSL::SSL ${OpenCV_LIBS})
