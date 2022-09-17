cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(mnist)
set(CMAKE_CXX_STANDARD 14)

find_package(Torch REQUIRED)

add_executable(mnist mnist.cpp net.cpp)
target_link_libraries(mnist ${TORCH_LIBRARIES})
