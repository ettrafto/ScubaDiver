cmake_minimum_required(VERSION 3.0)
project(graphics)

# Set which project you would like to build
set(B_TARGET "src")

# Checks for git
find_package(Git REQUIRED)

# Initialize the submodule if not already done so
if(NOT EXISTS lib/glfw/CMakeLists.txt)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive -- ${dir}
            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
endif()

# Do not build other non-important things
option(GLFW_BUILD_DOCS ON)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS ON)

# non-needed features of freetype
option(FT_DISABLE_ZLIB ON)
option(FT_DISABLE_BZIP2 ON)
option(FT_DISABLE_PNG ON)
option(FT_DISABLE_HARFBUZZ ON)
option(FT_DISABLE_BROTLI ON)
option(FT_DISABLE_GZIP ON)
option(FT_DISABLE_LZMA ON)

# Add subdirectories
add_subdirectory(lib/glfw)
add_subdirectory(lib/glm)
add_subdirectory(lib/freetype)

# Set compiler flags based on compiler
if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") # Check if using GCC
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -static-libstdc++ -static-libgcc")
    # -Wall -Wextra -Wpedantic
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang") # Check if using Clang
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
endif()

# Set include directories
include_directories(lib/glfw/include
                    lib/glad/include
                    lib/freetype/include
                    lib/glm)

# Set source files
file(GLOB VENDORS_SOURCES lib/glad/src/glad.c)
file(GLOB_RECURSE PROJECT_HEADERS ${B_TARGET}/*.h)
file(GLOB_RECURSE PROJECT_SOURCES ${B_TARGET}/*.cpp)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES}
        ../../../../Personal/Coding/M4OEP-ettrafto/src/framework/maze.h)

target_link_libraries(${PROJECT_NAME} glfw freetype)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
