cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

project(game)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(EXEC "game")

find_package(GLFW3 REQUIRED)
message(STATUS "GLFW3 inclued at ${GLFW3_INCLUDE_DIR}")

add_library(GLAD "src/glad.c")

if (APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
    FIND_LIBRARY(IOKit_Library IOKit)
    FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
    MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
    SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_Library} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    SET(LIBS ${LIBS} ${APPLE_LIBS} ${GLFW3_LIBRARY})
elseif(UNIX)
    set(LIBS ${LIBS} GLAD)
    set(LIBS ${GLFW3_LIBRARY} dl)
else()
    set(LIBS )
endif(APPLE)

file(GLOB SRC "src/*.c" "src/*.h")
list(APPEND CMAKE_C_FLAGS "-Wall -std=c99")
set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -ldl")


add_executable(${EXEC} ${SRC})
include_directories(${CMAKE_SOURCE_DIR}/include)
target_link_libraries(${EXEC} ${LIBS})
