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
    CmdBindDescriptorSets.hpp
    CmdBindGraphicsPipeline.hpp 
    CmdBindVertexBuffers.hpp 
    CmdDraw.hpp 
    CmdBindIndexBuffer.hpp
    CmdDrawIndexed.hpp
    Command.hpp 
    CommandBuffer.hpp
    CommandBuffer.cpp
    Inherit.hpp
    RenderPass.hpp
    RenderPass.cpp
    CommandPool.hpp
    CommandPool.cpp
    Framebuffer.hpp
    Framebuffer.cpp
    BaseItem.hpp
    BaseItem.cpp
    Visitor.hpp
    Visitor.cpp
    PhysicalDevice.hpp
    PhysicalDevice.cpp
    Device.hpp
    Device.cpp
    QueueSetting.hpp
    QueueSetting.cpp
    Semaphore.cpp
    Semaphore.hpp
    Fence.cpp
    Fence.hpp
    Queue.cpp
    Queue.hpp
)

if(UNIX)
add_custom_target(build_shaders ALL
    COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.sh"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
)
endif(UNIX)

if(WIN32)
add_custom_target(build_shaders ALL
    COMMAND "${PROJECT_SOURCE_DIR}/shaders/compile.bat"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/shaders"
)
endif(WIN32)

add_dependencies(engine build_shaders)
target_include_directories(engine PUBLIC external/SDL/include)
target_link_libraries(engine PUBLIC SDL3::SDL3)
target_include_directories(engine PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(engine PUBLIC ${Vulkan_LIBRARIES})