#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>

using PhysicalDeviceList = std::vector<vk::PhysicalDevice>;

enum VulkanContextResult {
    Success,
    FailedCreateInstance,
    FailedCreateSurface,
    NoSuitablePhysicalDevice,
    FailedCreateDevice,
    FailedCreateSwapchain,
    FailedCreateSwapchainViewImages,
};

struct VulkanQueue {
    uint32_t familyIndex;
    vk::Queue queue;
};

using DeviceExtensionList = std::vector<const char*>;
using InstanceExtensionList = std::vector<const char*>;
using VulkanImageList = std::vector<vk::Image>;
using VulkanImageViewList = std::vector<vk::ImageView>;

struct VulkanContext {
    
    vk::Instance instance;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;
    
    VulkanQueue graphics;

    vk::Device device;

    vk::SwapchainKHR swapchain;

    vk::Format swapchainFormat;

    VulkanImageList images;
    VulkanImageViewList imageViews;
};


std::string vulkanErrorToString(VulkanContextResult result) {

    switch (result) {

        case VulkanContextResult::FailedCreateInstance:
            return std::string("Vulkan instance cannot be installed");
        case VulkanContextResult::FailedCreateSurface:
            return std::string("Vulkan Surface cannot be installed");
        case VulkanContextResult::NoSuitablePhysicalDevice:
            return std::string("Error: Failed to no suitable physical device");
        case VulkanContextResult::FailedCreateDevice:
            return std::string("Error: Failed to create vulkan logical device");
        case VulkanContextResult::FailedCreateSwapchain:
            return std::string("Error: Failed to create vulkan swapchain");
        case VulkanContextResult::FailedCreateSwapchainViewImages:
            return std::string("Error: Failed to create vulkan image views");
    }

    return std::string();
}

[[nodiscard]] VulkanContextResult createVulkanInstance(VulkanContext* context, InstanceExtensionList enabledExtensions) {

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
        return VulkanContextResult::FailedCreateInstance;
    }

    return VulkanContextResult::Success;
}

[[nodiscard]] InstanceExtensionList getRequiredInstanceExtensions() {

    uint32_t count;
    const char **extensions = glfwGetRequiredInstanceExtensions(&count);

    if (!extensions) {
        std::cerr << "Failed to retrieve required instance extensions." << std::endl;
        return std::vector<const char*>();
    }

    InstanceExtensionList extensionVector;
    for (uint32_t i = 0; i < count; ++i) {
        extensionVector.push_back(extensions[i]);
    }

    return extensionVector;
}

[[nodiscard]] VulkanContextResult createVulkanSurface(VulkanContext* context, GLFWwindow* window) {

    assert(context->instance);
    assert(window);

    VkSurfaceKHR surface;
    if(VkResult result = glfwCreateWindowSurface(context->instance, window, nullptr, &surface); result != VK_SUCCESS) {
        return VulkanContextResult::FailedCreateSurface;
    }

    context->surface = vk::SurfaceKHR(surface);

    return VulkanContextResult::Success;
}

[[nodiscard]] VulkanContextResult choosePhysicalDevice(VulkanContext* context)
{
    std::vector<vk::PhysicalDevice> physicalDevices = context->instance.enumeratePhysicalDevices();

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

    if(!bestDevice) return VulkanContextResult::NoSuitablePhysicalDevice;
    context->physicalDevice = bestDevice;

    return VulkanContextResult::Success;
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

[[nodiscard]] VulkanContextResult createDevice(VulkanContext* context, DeviceExtensionList extensions) {

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
        return VulkanContextResult::FailedCreateDevice;
    }

    context->graphics.familyIndex = familyIndex;
    context->graphics.queue = context->device.getQueue(familyIndex, 0);

    return VulkanContextResult::Success;
}

vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes) {

    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;
    
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
            return availablePresentMode;
        }
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
            bestMode = availablePresentMode;
        }
    }
    
    return bestMode;
}

vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR& capabilities)
{
    vk::Extent2D extent;

    if (capabilities.currentExtent.width == 0xFFFFFFFF) {
		extent.width = capabilities.minImageExtent.width;
	}

    else {
        extent.width = capabilities.currentExtent.width;
    }

    if (capabilities.currentExtent.height == 0xFFFFFFFF) {
		extent.height = capabilities.minImageExtent.height;
	}

    else {
        extent.height = capabilities.currentExtent.height;
    }

    return extent;
}

vk::SurfaceFormatKHR chooseSurfaceFormat(const vk::PhysicalDevice physicalDevice, const std::vector<vk::SurfaceFormatKHR>& availableFormats, const vk::ImageUsageFlags usage) {

    uint32_t index = 0;
    for (uint32_t x=0;x<availableFormats.size();x++) {
        
        vk::ImageFormatProperties formatProperties;
        vk::Result result = physicalDevice.getImageFormatProperties(availableFormats[x].format, vk::ImageType::e2D, vk::ImageTiling::eOptimal, usage, {}, &formatProperties);
		
		if(result == vk::Result::eErrorFormatNotSupported) {
			std::cout << "Swapchain format does not support requested usage flags" << std::endl;
		} else {
            index = x;
			break;
		}
    }

    return availableFormats[index];
}

VulkanContextResult createVulkanSwapchain(VulkanContext* context, vk::ImageUsageFlags usage) {

    vk::SurfaceKHR surface = context->surface;

    auto availableSurfaceFormats = context->physicalDevice.getSurfaceFormatsKHR(surface);
    auto surfaceCapabilities = context->physicalDevice.getSurfaceCapabilitiesKHR(surface);
    auto availablePresentModes = context->physicalDevice.getSurfacePresentModesKHR(surface);

    vk::PresentModeKHR presentMode = choosePresentMode(availablePresentModes);
    vk::Extent2D extent = chooseExtent(surfaceCapabilities);
	vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(context->physicalDevice, availableSurfaceFormats, usage);
    vk::Format format = surfaceFormat.format;

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.setSurface(surface);
	createInfo.setMinImageCount(3);
	createInfo.setImageFormat(format);
	createInfo.setImageColorSpace(surfaceFormat.colorSpace);
	createInfo.setImageExtent(surfaceCapabilities.currentExtent);
	createInfo.setImageArrayLayers(1);
	createInfo.setImageUsage(usage);
	createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	createInfo.setPreTransform(vk::SurfaceTransformFlagBitsKHR::eIdentity);
	createInfo.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque);
	createInfo.setPresentMode(presentMode);
	createInfo.setOldSwapchain(context->swapchain ? context->swapchain : vk::SwapchainKHR {});

    if(vk::Result result = context->device.createSwapchainKHR(&createInfo, nullptr, &context->swapchain); result != vk::Result::eSuccess) {
        return VulkanContextResult::FailedCreateSwapchain;
    }

    context->images = context->device.getSwapchainImagesKHR(context->swapchain);

    context->swapchainFormat = format;

    return VulkanContextResult::Success;
}

VulkanContextResult createSwapchainViewImages(VulkanContext* context) {

    if(!context->swapchain || context->images.empty()) return FailedCreateSwapchainViewImages;

    unsigned long amountOfImageViews = context->images.size();
    context->imageViews.resize(amountOfImageViews);

    vk::ImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.setViewType(vk::ImageViewType::e2D);
    imageViewCreateInfo.setFormat(context->swapchainFormat);
    imageViewCreateInfo.setSubresourceRange({ vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1 });

    for(unsigned long x = 0; x < amountOfImageViews; x++) {

        imageViewCreateInfo.setImage(context->images[x]);

        if(vk::Result result = context->device.createImageView(&imageViewCreateInfo, nullptr, &context->imageViews[x]); result != vk::Result::eSuccess) {
            return VulkanContextResult::FailedCreateSwapchainViewImages;
        }
    }

    return VulkanContextResult::Success;
}

[[nodiscard]] VulkanContextResult initVulkan(GLFWwindow* window, VulkanContext* context) {

    InstanceExtensionList extensions;
    DeviceExtensionList deviceExtensions { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    InstanceExtensionList requiredExtensions = getRequiredInstanceExtensions();
    extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

    if(createVulkanInstance(context, extensions) == VulkanContextResult::FailedCreateInstance) {
        return VulkanContextResult::FailedCreateInstance;
    }
    
    if(createVulkanSurface(context, window) == VulkanContextResult::FailedCreateSurface) {
        return VulkanContextResult::FailedCreateSurface;
    }
    
    if(choosePhysicalDevice(context) == VulkanContextResult::NoSuitablePhysicalDevice) {
        return VulkanContextResult::NoSuitablePhysicalDevice;
    }

    if(createDevice(context, deviceExtensions) == VulkanContextResult::FailedCreateDevice) {
        return VulkanContextResult::FailedCreateDevice;
    }

    if(createVulkanSwapchain(context, vk::ImageUsageFlagBits::eColorAttachment) == VulkanContextResult::FailedCreateSwapchain) {
        return VulkanContextResult::FailedCreateSwapchain;
    }

    if(createSwapchainViewImages(context) == VulkanContextResult::FailedCreateSwapchainViewImages) {
        return FailedCreateSwapchainViewImages;
    }

    return VulkanContextResult::Success;
}

void terminateVulkan(VulkanContext* context) {

    vk::Instance instance = context->instance;
    vk::Device device = context->device;

    if(device) {
        
        for(vk::ImageView view : context->imageViews) {
            device.destroyImageView(view);
        }

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
    if(VulkanContextResult result = initVulkan(window, &context); result != VulkanContextResult::Success) {
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
