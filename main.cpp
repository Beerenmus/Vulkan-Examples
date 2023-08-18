#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>

struct VulkanContext {
    vk::Instance instance;
    vk::SurfaceKHR surface;
};

using ExtensionList = std::vector<const char*>;

bool createVulkanInstance(VulkanContext* context, ExtensionList enabledExtensions) {

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
        return false;
    }

    return true;
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

bool createVulkanSurface(VulkanContext* context, GLFWwindow* window) {

    assert(context->instance);
    assert(window);

    VkSurfaceKHR surface;
    VkResult result = glfwCreateWindowSurface(context->instance, window, nullptr, &surface);

    if(result != VK_SUCCESS) {
        std::cerr << "Vulkan Surface cannot be installed" << std::endl;
        return false;
    }

    context->surface = vk::SurfaceKHR(surface);

    return true;
}

bool initVulkan(GLFWwindow* window, VulkanContext* context) {

    ExtensionList extensions;

    ExtensionList requiredExtensions = getRequiredInstanceExtensions();
    extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

    if(!createVulkanInstance(context, extensions)) return false;
    if(!createVulkanSurface(context, window)) return false;

    return true;
}

void terminateVulkan(VulkanContext* context) {

    vk::Instance instance = context->instance;

    if(instance && context->surface) {
        instance.destroySurfaceKHR(context->surface);    
    }

    if(instance) {
        instance.destroy();
    }
}

int main() {

    if(glfwInit() == GLFW_FALSE) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Hello Vulkan", 0, 0);

    VulkanContext context;
    if(!initVulkan(window, &context)) {
        terminateVulkan(&context);
        return EXIT_FAILURE;
    }

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }

    terminateVulkan(&context);

    glfwTerminate();

    return EXIT_SUCCESS;
}
