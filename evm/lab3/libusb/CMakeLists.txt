cmake_minimum_required(VERSION 3.23)
project (libusb)

set(CMAKE_CXX_STANDARD 17)
list(APPEND CMAKE_MODULE_PATH "deps/tbb/cmake/")

find_package(TBB REQUIRED)
add_executable(libusb main.cpp)
target_link_libraries(libusb usb-1.0  TBB::tbb)
