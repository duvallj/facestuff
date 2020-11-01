cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "FaceStuff")
set(CMAKE_BUILD_TYPE RELWITHDEBINFO)

project(${PROJECT_NAME})

set(SOURCE_FILES
  "src/ArgParse.cpp"
  "src/Main.cpp"
)
set(HEADER_FILES
  "src/ArgParse.hpp"
)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(JSONCPP jsoncpp)
#include_directories(${JSONCPP_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
