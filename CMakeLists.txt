cmake_minimum_required(VERSION 3.14)
project(TAMP_CPP)

set(CMAKE_CXX_STANDARD 17)
find_package(Boost  COMPONENTS program_options filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
add_executable(TAMP_CPP main.cpp include/Table.h)
target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})