#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

struct VulkanContext {
    vk::Instance instance;
};

using ExtensionList = std::vector<const char*>;

void createVulkanInstance(VulkanContext* context, ExtensionList enabledExtensions) {

    std::vector<const char*> enabledLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    vk::ApplicationInfo applicationInfo;
    applicationInfo.setPApplicationName("Vulkan");
    applicationInfo.setApplicationVersion(VK_MAKE_VERSION(0, 0, 1));
    applicationInfo.setApiVersion(VK_API_VERSION_1_2);

    vk::InstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.setPApplicationInfo(&applicationInfo);
    instanceCreateInfo.setEnabledLayerCount(static_cast<uint32_t>(enabledLayers.size()));
    instanceCreateInfo.setPpEnabledLayerNames(enabledLayers.data());
    instanceCreateInfo.setEnabledExtensionCount(static_cast<uint32_t>(enabledExtensions.size()));
    instanceCreateInfo.setPpEnabledExtensionNames(enabledExtensions.data());
   
    vk::Result result;
    context->instance = vk::createInstance(instanceCreateInfo);
    if (!context->instance) {
        std::cout << "Vulkan instance cannot be installed" << std::endl;
    }
}

int main() {

    if(glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello Vulkan", 0, 0);

    VulkanContext context;

    ExtensionList extensions;
    createVulkanInstance(&context, extensions);

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }

    vkDestroyInstance(context.instance, nullptr);

    glfwTerminate();

    return EXIT_SUCCESS;
}
