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
    cmd_bind_descriptor_sets.hpp 
    cmd_bind_graphics_descriptor_sets.hpp 
    cmd_bind_graphics_pipeline.hpp 
    cmd_bind_pipeline.hpp 
    cmd_bind_vertex_buffers.hpp 
    cmd_draw.hpp 
    command.hpp
    command_buffer.hpp
)

if(UNIX)
add_custom_target(build_shaders ALL
    COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.sh"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
)
endif(UNIX)

add_dependencies(engine build_shaders)
target_include_directories(engine PUBLIC libs/SDL/include)
target_link_libraries(engine PUBLIC SDL3::SDL3)
target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(engine PUBLIC ${Vulkan_LIBRARIES})