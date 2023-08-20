#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>

using PhysicalDeviceList = std::vector<vk::PhysicalDevice>;

enum VulkanResult {
    Success,
    FailedCreateInstance,
    FailedCreateSurface,
    NoSuitablePhysicalDevice,
    FailedCreateDevice
};

struct VulkanQueue {
    uint32_t familyIndex;
    vk::Queue queue;
};

struct VulkanContext {
    
    vk::Instance instance;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    
    VulkanQueue graphics;

    vk::Device device;
};

using ExtensionList = std::vector<const char*>;
using DeviceExtensionList = std::vector<const char*>;

std::string vulkanErrorToString(VulkanResult result) {

    switch (result) {

        case VulkanResult::FailedCreateInstance:
            return std::string("Vulkan instance cannot be installed");
        case VulkanResult::FailedCreateSurface:
            return std::string("Vulkan Surface cannot be installed");
        case VulkanResult::NoSuitablePhysicalDevice:
            return std::string("Error: Failed to no suitable physical device");
        case VulkanResult::FailedCreateDevice:
            return std::string("Error: Failed to create vulkan logical device");
    }

    return std::string();
}

[[nodiscard]] VulkanResult createVulkanInstance(VulkanContext* context, ExtensionList enabledExtensions) {

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
   
    if (vk::Result result = vk::createInstance(&instanceCreateInfo, nullptr, &context->instance); result != vk::Result::eSuccess) {
        return VulkanResult::FailedCreateInstance;
    }

    return VulkanResult::Success;
}

[[nodiscard]] ExtensionList getRequiredInstanceExtensions() {

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

[[nodiscard]] VulkanResult createVulkanSurface(VulkanContext* context, GLFWwindow* window) {

    assert(context->instance);
    assert(window);

    VkSurfaceKHR surface;
    if(VkResult result = glfwCreateWindowSurface(context->instance, window, nullptr, &surface); result != VK_SUCCESS) {
        return VulkanResult::FailedCreateSurface;
    }

    context->surface = vk::SurfaceKHR(surface);

    return VulkanResult::Success;
}

[[nodiscard]] vk::PhysicalDevice choosePhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDevices)
{
    vk::PhysicalDevice bestDevice = VK_NULL_HANDLE;
    int bestScore = 0;

    for (const auto& device : physicalDevices) {

        vk::PhysicalDeviceProperties properties = device.getProperties();

        int score = 0;

        switch (properties.deviceType) {

            case vk::PhysicalDeviceType::eDiscreteGpu:
                score += 1000;
                break;

            case vk::PhysicalDeviceType::eIntegratedGpu:
                score += 500;
                break;
        }

        score += static_cast<int>(properties.limits.maxMemoryAllocationCount) / 1024;

        if (score > bestScore) {
            bestDevice = device;
            bestScore = score;
        }
    }

    return bestDevice;
}

[[nodiscard]] uint32_t findGraphicsQueueFamily(std::vector<vk::QueueFamilyProperties>& queueFamilies) {

    uint32_t queueFamilyIndex = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++) {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics) {
            queueFamilyIndex = i;
            break;
        }
    }

    return queueFamilyIndex;
}

[[nodiscard]] VulkanResult createDevice(VulkanContext* context, DeviceExtensionList extensions) {

    vk::PhysicalDevice physicalDevice = context->physicalDevice;
    if(!physicalDevice) return FailedCreateDevice;

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t familyIndex = findGraphicsQueueFamily(queueFamilies);

    float priorities[] = { 1.0f };

    vk::DeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.setQueueFamilyIndex(familyIndex);
    deviceQueueCreateInfo.setQueueCount(1);
    deviceQueueCreateInfo.setPQueuePriorities(priorities);

    vk::PhysicalDeviceFeatures enabledFeatures = {};

    vk::DeviceCreateInfo createInfo;
    createInfo.setQueueCreateInfoCount(1);
    createInfo.setPQueueCreateInfos(&deviceQueueCreateInfo);
    createInfo.setEnabledExtensionCount(static_cast<uint32_t>(extensions.size()));
    createInfo.setPpEnabledExtensionNames(extensions.data());
    createInfo.setPEnabledFeatures(&enabledFeatures);

    if (vk::Result result = physicalDevice.createDevice(&createInfo, nullptr, &context->device); result !=vk::Result::eSuccess) {
        return VulkanResult::FailedCreateDevice;
    }

    context->graphics.familyIndex = familyIndex;
    context->graphics.queue = context->device.getQueue(familyIndex, 0);

    return VulkanResult::Success;
}

[[nodiscard]] VulkanResult initVulkan(GLFWwindow* window, VulkanContext* context) {

    ExtensionList extensions;

    ExtensionList requiredExtensions = getRequiredInstanceExtensions();
    extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

    if(createVulkanInstance(context, extensions) == VulkanResult::FailedCreateInstance) {
        return VulkanResult::FailedCreateInstance;
    }
    
    if(createVulkanSurface(context, window) == VulkanResult::FailedCreateSurface) {
        return VulkanResult::FailedCreateSurface;
    }
    
    PhysicalDeviceList physicalDevices = context->instance.enumeratePhysicalDevices();
    if(physicalDevices.empty()) {
        std::cerr << "No physical devices found" << std::endl;
        return VulkanResult::NoSuitablePhysicalDevice;
    }

    context->physicalDevice = choosePhysicalDevice(physicalDevices);
    if(!context->physicalDevice) {
        //std::cerr << "Failed to find suitable physical device" << std::endl;
        return VulkanResult::NoSuitablePhysicalDevice;
    }

    DeviceExtensionList deviceExtensions;
    if(createDevice(context, deviceExtensions) == VulkanResult::FailedCreateDevice) {
        return VulkanResult::FailedCreateDevice;
    }

    return VulkanResult::Success;
}

void terminateVulkan(VulkanContext* context) {

    vk::Instance instance = context->instance;
    vk::Device device = context->device;

    if(device) {
        device.destroy();
    }

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
    if(VulkanResult result = initVulkan(window, &context); result != VulkanResult::Success) {
        terminateVulkan(&context);
        std::cout << vulkanErrorToString(result) << std::endl;
        return EXIT_FAILURE;
    }

    while(!glfwWindowShouldClose(window)) {

        glfwPollEvents();
    }

    terminateVulkan(&context);

    glfwTerminate();

    return EXIT_SUCCESS;
}
