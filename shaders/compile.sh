#!/bin/sh
glslc -fshader-stage=vert VertexShader.glsl -o VertexShader.spv
glslc -fshader-stage=frag FragmentShader.glsl -o FragmentShader.spv
