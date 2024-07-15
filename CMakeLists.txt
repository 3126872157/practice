cmake_minimum_required(VERSION 3.28)
project(tv_training)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(tv_training main.cpp)

# Link your application with OpenCV libraries
target_link_libraries(tv_training PRIVATE ${OpenCV_LIBS})
