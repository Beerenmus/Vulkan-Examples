#include <iostream>
#include <GLFW/glfw3.h>

int main() {

    if(glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello Vulkan", 0, 0);

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }

    glfwTerminate();

    return EXIT_SUCCESS;
}
