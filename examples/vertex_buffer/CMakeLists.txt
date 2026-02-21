add_executable(vertex_buffer main.cpp)
target_link_libraries(vertex_buffer PRIVATE Base)
compile_shaders(vertex_buffer)
