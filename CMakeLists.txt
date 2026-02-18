cmake_minimum_required(VERSION 3.20)

project(vulkan_examples LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/SDL)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

add_library(Base INTERFACE)
target_link_libraries(Base INTERFACE Vulkan::Vulkan SDL3::SDL3)

include(cmake/compile_shaders.cmake)

add_subdirectory(examples/hello_world)