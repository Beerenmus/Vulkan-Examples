cmake_minimum_required(VERSION 3.14)

project("Hello-Vulkan")

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")

add_subdirectory(external/SDL)

find_package(Vulkan REQUIRED)

add_executable(engine 
    main.cpp
    Matrix.hpp
    Matrix.cpp
    Strukturen.hpp
    Strukturen.cpp
)

target_include_directories(engine PUBLIC libs/SDL/include)
target_link_libraries(engine PUBLIC SDL3::SDL3)
target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(engine PUBLIC ${Vulkan_LIBRARIES})