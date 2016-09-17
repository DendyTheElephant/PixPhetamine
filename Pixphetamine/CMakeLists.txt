cmake_minimum_required(VERSION 2.8.12)
project(PixPhetamine)

#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

# we want to use C++11
if(NOT DEFINED MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# add directories for CMake modules path
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/modules")

set(HEADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/code/hdr/)
set(SOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/code/src/)
set(SHADERS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)

file(GLOB_RECURSE HEADER_FILES code/hdr/*.h)
file(GLOB_RECURSE SOURCE_FILES code/src/*.cpp code/*.cpp)
file(GLOB_RECURSE SHADER_FILES shaders/*.vert shaders/*.frag)

#============== Link Dependancies ==============#
find_package(OpenGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    link_libraries(${OPENGL_LIBRARIES})
endif()

find_package(GLEW REQUIRED)
if(GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIR})
    link_libraries(${GLEW_LIBRARY})
endif()

find_package(GLM REQUIRED)
if(GLM_FOUND)
    include_directories(${GLM_INCLUDE_DIRS})
endif()

find_package(SDL2 REQUIRED)
if(SDL2_FOUND)
    include_directories(${SDL2_INCLUDE_DIR})
    link_libraries(${SDL2_LIBRARY})
endif()

find_package(SDL2_image REQUIRED)
if(SDL2_IMAGE_FOUND)
    include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
    link_libraries(${SDL2_IMAGE_LIBRARIES})
endif()

include_directories(${HEADERS_DIR})

add_executable(${PROJECT_NAME} ${HEADER_FILES} ${SOURCE_FILES} ${SHADER_FILES})
