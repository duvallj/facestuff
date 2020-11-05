cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "FaceStuff")
set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

set(SOURCE_FILES
  "src/ArgParse.cpp"
  "src/Detector.cpp"
  "src/WinBGInput.c"
  "src/Main.cpp"
)
set(HEADER_FILES
  "src/ArgParse.hpp"
  "src/Detector.hpp"
  "src/WinBGInput.h"
  "src/Debug.h"
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})

set(WinSDK_BASE "D:/Program Files/Windows SDKs/10")
set(WinSDK_VERSION "10.0.19041.0")
set(WinSDK_INCLUDE_BASE "${WinSDK_BASE}/Include/${WinSDK_VERSION}")
set(WinSDK_INCLUDE_DIRS
  "${WinSDK_INCLUDE_BASE}/km"
  "${WinSDK_INCLUDE_BASE}/shared"
  "${WinSDK_INCLUDE_BASE}/ucrt"
  "${WinSDK_INCLUDE_BASE}/um"
  "${WinSDK_INCLUDE_BASE}/winrt"
)

set(WinSDK_LIB_BASE "${WinSDK_BASE}/Lib/${WinSDK_VERSION}")
set(WinSDK_LIB_DIRS
  "${WinSDK_LIB_BASE}/km/x64"
  "${WinSDK_LIB_BASE}/ucrt/x64"
  "${WinSDK_LIB_BASE}/um/x64"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${WinSDK_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${WinSDK_LIB_DIRS})

set(WinSDK_LIBS
  "psapi.lib"
  "Kernel32.lib"
  "User32.lib"
)

#find_package(PkgConfig REQUIRED)
#pkg_check_modules(JSONCPP jsoncpp)
#include_directories(${JSONCPP_INCLUDE_DIRS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${WinSDK_LIBS})
