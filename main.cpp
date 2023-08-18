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
        std::cerr << "Vulkan instance cannot be installed" << std::endl;
    }
}

ExtensionList getRequiredInstanceExtensions() {

    uint32_t count;
    const char **extensions = glfwGetRequiredInstanceExtensions(&count);

    if (!extensions) {
        std::cerr << "Failed to retrieve required instance extensions." << std::endl;
        return std::vector<const char*>();
    }

    ExtensionList extensionVector;
    for (uint32_t i = 0; i < count; ++i) {
        extensionVector.push_back(extensions[i]);
    }

    return extensionVector;
}

int main() {

    if(glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello Vulkan", 0, 0);

    VulkanContext context;

    ExtensionList extensions;

    ExtensionList requiredExtensions = getRequiredInstanceExtensions();
    extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

    createVulkanInstance(&context, extensions);

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }

    vkDestroyInstance(context.instance, nullptr);

    glfwTerminate();

    return EXIT_SUCCESS;
}
