cmake_minimum_required(VERSION 2.8)
set(PROJECT_NAME "FaceStuff")
set(CMAKE_BUILD_TYPE RELWITHDEBINFO)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
cmake_policy(SET CMP0079 NEW)

set(ARCH x86_64)
set(COMPILER 142)
if(CORE_CRL_MD)
  set(CRT MD)
else()
  set(CRT MT)
endif()

set(SOURCE_FILES
  "src/live2d/Allocator.cpp"
  "src/live2d/Definitions.cpp"
  "src/live2d/Displayer.cpp"
  "src/live2d/Model.cpp"
  "src/live2d/Sprite.cpp"
  "src/live2d/TextureManager.cpp"
  "src/live2d/Util.cpp"
  "src/ArgParse.cpp"
  "src/Detector.cpp"
  "src/WinBGInput.c"
  "src/Main.cpp"
)
set(HEADER_FILES
  "src/live2d/Allocator.hpp"
  "src/live2d/Definitions.hpp"
   "src/live2d/Displayer.hpp"
  "src/live2d/Model.hpp"
  "src/live2d/Sprite.hpp"
  "src/live2d/TextureManager.hpp"
  "src/live2d/Util.hpp"
  "src/ArgParse.hpp"
  "src/Detector.hpp"
  "src/WinBGInput.h"
  "src/Debug.h"
)

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
target_compile_options(${PROJECT_NAME} PRIVATE "/MP")

find_package(OpenMP REQUIRED)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

find_package(OpenCV REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE GLEW::GLEW)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
find_path(STB_INCLUDE_DIRS "stb.h")
target_include_directories(${PROJECT_NAME} PRIVATE ${STB_INCLUDE_DIRS})
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})

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

set(WinSDK_LIBS
  "psapi.lib"
  "Kernel32.lib"
  "User32.lib"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${WinSDK_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PRIVATE ${WinSDK_LIB_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE ${WinSDK_LIBS})

set(LIVE2D_SDK_ROOT_PATH "D:\\Documents\\Windows\\CubismSdkForNative-4-r.1")
set(LIVE2D_CORE_PATH "${LIVE2D_SDK_ROOT_PATH}\\Core")
set(LIVE2D_FRAMEWORK_PATH "${LIVE2D_SDK_ROOT_PATH}\\Framework")
set(LIVE2D_RES_PATH "${LIVE2D_SDK_ROOT_PATH}\\Samples\\Resources")

# Add Live2D as an external library manually
add_library(Live2DCubismCore STATIC IMPORTED)
set(LIVE2D_CORE_LIB_SUFFIX "${LIVE2D_CORE_PATH}\\lib\\windows\\${ARCH}\\${COMPILER}")
set_target_properties(Live2DCubismCore
  PROPERTIES
    IMPORTED_LOCATION_DEBUG "${LIVE2D_CORE_LIB_SUFFIX}\\Live2DCubismCore_${CRT}d.lib"
    IMPORTED_LOCATION_RELEASE "${LIVE2D_CORE_LIB_SUFFIX}\\Live2DCubismCore_${CRT}.lib"
    INTERFACE_INCLUDE_DIRECTORIES "${LIVE2D_CORE_PATH}\\include"
)

# Add Cubism Framework as external project
set(FRAMEWORK_SOURCE OpenGL)
add_subdirectory(${LIVE2D_FRAMEWORK_PATH} "${CMAKE_CURRENT_BINARY_DIR}\\Framework")
target_compile_definitions(Framework PUBLIC CSM_TARGET_WIN_GL)
target_link_libraries(Framework Live2DCubismCore GLEW::GLEW)

# Link Framework into the main executable
target_link_libraries(${PROJECT_NAME} PRIVATE Framework)

# Copy resource directory to build directory.
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND
    ${CMAKE_COMMAND} -E
      copy_directory ${LIVE2D_RES_PATH} $<TARGET_FILE_DIR:${PROJECT_NAME}>/Resources
)