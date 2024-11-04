cmake_minimum_required(VERSION 3.10)
project(OpenGL)

# Setting the C and C++ standard
set(CMAKE_C_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
add_definitions(-DGLEW_STATIC)

include_directories(${CMAKE_SOURCE_DIR}/Dependencies/GLFW/include)
link_directories(${CMAKE_SOURCE_DIR}/Dependencies/GLFW/lib-mingw-w64)
include_directories(${CMAKE_SOURCE_DIR}/Dependencies/GLEW/include)
link_directories(${CMAKE_SOURCE_DIR}/Dependencies/GLEW/lib/Release/x64)

# Adding the executable target
add_executable(OpenGL src/main.cpp)

target_link_libraries(OpenGL PRIVATE glew32s glfw3 opengl32)
