cmake_minimum_required(VERSION 3.14)
project(TAMP_CPP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(cmake/CPM.cmake)
find_package(Boost COMPONENTS program_options REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${Boost_INCLUDE_DIRS}
        ${OpenCV_INCLUDE_DIRS}
        ${OMPLAPP_INCLUDE_DIR}
        )


CPMAddPackage(
        NAME yaml-cpp
        GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
        # 0.6.2 uses deprecated CMake syntax
        VERSION 0.6.3
        # 0.6.3 is not released yet, so use the most recent commit
        GIT_TAG 012269756149ae99745b6dafefd415843d7420bb
        OPTIONS "YAML_CPP_BUILD_TESTS Off" "YAML_CPP_BUILD_CONTRIB Off" "YAML_CPP_BUILD_TOOLS Off"
)
find_package(Matplot++ REQUIRED)
find_package(OMPL REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
add_executable(TAMP_CPP main.cpp include/Table.h src/solver.cpp src/Display.cpp include/Display.h src/PathPlanning.cpp include/PathPlanning.h)
target_link_libraries(${PROJECT_NAME}
        ${Boost_LIBRARIES}
        -pthread
        Matplot++::matplot
        ${OpenCV_LIBS}
        yaml-cpp
        ${OMPL_LIBRARY}
        ${OMPLAPP_LIBRARY}
        ${OMPLAPPBASE_LIBRARY}
        Eigen3::Eigen
        )
