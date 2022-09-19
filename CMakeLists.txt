cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(mnist)
set(CMAKE_CXX_STANDARD 14)

find_package(Torch REQUIRED)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

add_executable(mnist mnist.cpp net.cpp dataloaders.tpp)
target_link_libraries(mnist ${TORCH_LIBRARIES} ${OpenCV_LIBS} )
