add_executable(push_constants main.cpp
        ../../common/Matrix.h)
target_link_libraries(push_constants PRIVATE Base)
compile_shaders(push_constants)
