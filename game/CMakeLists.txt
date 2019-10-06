cmake_minimum_required(VERSION 2.8)
cmake_policy(VERSION 2.8)

project(game)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
set(EXEC "game")

if(WIN32)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
endif(WIN32)

#link_directories(${CMAKE_SOURCE_DIR}/lib)

# find the required packages
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

if(UNIX AND NOT APPLE)
    set(LIBS ${GLFW3_LIBRARY} dl)
    set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -ldl")
elseif(APPLE)
    INCLUDE_DIRECTORIES(/System/Library/Frameworks)
    FIND_LIBRARY(COCOA_LIBRARY Cocoa)
    FIND_LIBRARY(OpenGL_LIBRARY OpenGL)
    FIND_LIBRARY(IOKit_LIBRARY IOKit)
    FIND_LIBRARY(CoreVideo_LIBRARY CoreVideo)
    MARK_AS_ADVANCED(COCOA_LIBRARY OpenGL_LIBRARY)
    SET(APPLE_LIBS ${COCOA_LIBRARY} ${IOKit_LIBRARY} ${OpenGL_LIBRARY} ${CoreVideo_LIBRARY})
    SET(APPLE_LIBS ${APPLE_LIBS} ${GLFW3_LIBRARY})
    set(LIBS ${LIBS} ${APPLE_LIBS})
else()
    set(LIBS )
endif(UNIX AND NOT APPLE)

set(CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_LINK_EXECUTABLE} -lm")

add_library(GLAD "src/glad.c")
set(LIBS ${LIBS} GLAD)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c99")

file(GLOB SRC "src/*.c" "src/*.h")
file(GLOB SHADERS "src/shaders/*.vs" "src/shaders/*.fs")
file(GLOB RESOURCES "resources/*.jpg" "resources/*.png")

add_executable(${EXEC} ${SRC})
target_link_libraries(${EXEC} ${LIBS})
include_directories(${CMAKE_SOURCE_DIR}/include)

foreach(SHADER ${SHADERS})
    file(COPY ${SHADER} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders")
endforeach(SHADER)

foreach(RESOURCE_FILE ${RESOURCES})
    file(COPY ${RESOURCE_FILE} DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/resources")
endforeach(RESOURCE_FILE)
