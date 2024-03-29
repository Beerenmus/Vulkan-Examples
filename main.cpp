#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <optional>
#include <fstream>
#include <string.h>
#include <array>
#include <span>

#include <vulkan/vulkan.hpp>

#include<SDL.h>
#include<SDL_vulkan.h>

#include "Matrix.hpp"

#include "CommandBuffer.hpp"

#include "CommandBuffer.hpp"
#include "CmdBindDescriptorSets.hpp"
#include "CmdBindGraphicsPipeline.hpp"
#include "CmdBindIndexBuffer.hpp"
#include "CmdBindVertexBuffers.hpp"
#include "CmdDrawIndexed.hpp"
#include "RenderPass.hpp"
#include "CommandPool.hpp"
#include "Framebuffer.hpp"
#include "PhysicalDevice.hpp"

#define NODISCARD [[nodiscard]]

using PhysicalDeviceList = std::vector<VkPhysicalDevice>;

enum VulkanContextResult
{
    Success,
    FailedCreateInstance,
    FailedCreateSurface,
    NoSuitablePhysicalDevice,
    FailedCreateDevice,
    FailedCreateSwapchain,
    FailedCreateSwapchainViewImages,
    FailedCreateSwapchainFramebuffer,
    FailedCreateRenderPass,
    FailedCreateCommandPool,
    FailedAllocateCommandBuffer,
    FailedCreateFence,
    FailedRecordCommandBuffer,
    FailedVulkanRendering,
    FailedCreateSemaphore,
    FailedCreateBuffer,
    FailedCreateStagingBuffer
};

struct VulkanQueue
{
    uint32_t familyIndex;
    VkQueue queue;
};

struct VulkanBuffer
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    VkBufferUsageFlags usage;
};

struct UniformBuffer {
    Matrix modelMatrix;
    Matrix camera;
    Matrix projectionMatrix;
};

class SVulkanPipeline {
    
    public:
        VkPipelineLayout layout;
        VkPipeline pipeline;

    public:
        constexpr SVulkanPipeline() = default;
        constexpr SVulkanPipeline(SVulkanPipeline& another) : layout(another.layout), pipeline(another.pipeline) {}

        constexpr SVulkanPipeline(SVulkanPipeline&& another) : layout(std::move(another.layout)), pipeline(std::move(another.pipeline)) {
            another.layout = VK_NULL_HANDLE;
            another.pipeline = VK_NULL_HANDLE;
        }
        
        constexpr SVulkanPipeline operator = (SVulkanPipeline& another) {
            
            layout = another.layout;
            pipeline = another.pipeline;

            return *this;

        }
        
        constexpr SVulkanPipeline operator = (SVulkanPipeline&&) = delete;
        ~SVulkanPipeline() = default;
};

using DeviceExtensionList = std::vector<const char *>;
using InstanceExtensionList = std::vector<const char *>;
using VulkanImageList = std::vector<VkImage>;
using VulkanImageViewList = std::vector<VkImageView>;
using VulkanFenceList = std::vector<VkFence>;
using VulkanSemaphoreList = std::vector<VkSemaphore>;
using VulkanDescriptorSetLayoutBindingList = std::vector<VkDescriptorSetLayoutBinding>;
using DescriptorPoolSizeList = std::vector<VkDescriptorPoolSize>;
using DescriptorSetList = std::vector<VkDescriptorSet>;

struct VulkanContext
{
    VkInstance instance;
    VkSurfaceKHR surface;
    PhysicalDevice::Pointer physicalDevice;

    VulkanQueue graphics;
    VulkanQueue transfer;

    VkDevice device;

    VkSwapchainKHR swapchain;

    VkFormat swapchainFormat;

    VulkanImageList images;
    VulkanImageViewList imageViews;

    RenderPass::Pointer renderPass;

    uint32_t width;
    uint32_t height;

    Framebuffers framebuffers;

    CommandPools commandPools;
    VkCommandPool transferCommandPool;

    CommandBuffers commandBuffers;

    uint32_t amountOfFrames;

    VulkanFenceList fences;

    VulkanSemaphoreList waitSemaphore;
    VulkanSemaphoreList signalSemaphore;

    uint32_t frameIndex = 0;
};

NODISCARD std::string vulkanErrorToString(VulkanContextResult result)
{
    switch (result)
    {

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
    case VulkanContextResult::FailedCreateSwapchainFramebuffer:
        return std::string("Error: Failed to create vulkan framebuffer");
    case VulkanContextResult::FailedCreateRenderPass:
        return std::string("Error: Failed to create vulkan render pass");
    case VulkanContextResult::FailedCreateCommandPool:
        return std::string("Error: Failed to create vulkan command pool");
    case VulkanContextResult::FailedAllocateCommandBuffer:
        return std::string("Error: Failed to allocate vulkan command buffer");
    case VulkanContextResult::FailedCreateFence:
        return std::string("Error: Failed to create vulkan fence");
    case VulkanContextResult::FailedRecordCommandBuffer:
        return std::string("Error: Failed to record vulkan command buffer");
    case VulkanContextResult::FailedCreateSemaphore:
        return std::string("Error: Failed to create vulkan semaphore");
    case VulkanContextResult::FailedCreateStagingBuffer:
        return std::string("Error: Failed to create staging vulkan buffer");
    case VulkanContextResult::FailedCreateBuffer:
        return std::string("Error: Failed to create vulkan buffer");
    }

    return std::string();
}

NODISCARD VulkanContextResult createVulkanInstance(VulkanContext *context, InstanceExtensionList enabledExtensions)
{

    std::vector<const char *> enabledLayers = {
        "VK_LAYER_KHRONOS_validation"};

    VkApplicationInfo applicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "Vulkan",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_3
    };

    VkInstanceCreateInfo instanceCreateInfo {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
        .ppEnabledLayerNames = enabledLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
        .ppEnabledExtensionNames = enabledExtensions.data()};

    if (VkResult result = vkCreateInstance(&instanceCreateInfo, nullptr, &context->instance); result != VK_SUCCESS)
    {
        return VulkanContextResult::FailedCreateInstance;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createVulkanSurface(VulkanContext *context, SDL_Window *window)
{
    if(SDL_Vulkan_CreateSurface(window, context->instance, &context->surface) != SDL_TRUE) {
        return VulkanContextResult::FailedCreateSurface;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult choosePhysicalDevice(VulkanContext *context)
{
    uint32_t amountOfPhysicalDevice = 0;
    vkEnumeratePhysicalDevices(context->instance, &amountOfPhysicalDevice, nullptr);
    std::vector<VkPhysicalDevice> physicalDevices(amountOfPhysicalDevice);
    vkEnumeratePhysicalDevices(context->instance, &amountOfPhysicalDevice, &physicalDevices[0]);

    VkPhysicalDevice bestDevice;
    int bestScore = 0;

    for (const auto &device : physicalDevices)
    {

        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(device, &properties);

        int score = 0;

        switch (properties.deviceType)
        {

        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            score += 1000;
            break;

        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            score += 500;
            break;
        }

        score += static_cast<int>(properties.limits.maxMemoryAllocationCount) / 1024;

        if (score > bestScore)
        {
            bestDevice = device;
            bestScore = score;
        }
    }

    if (!bestDevice)
        return VulkanContextResult::NoSuitablePhysicalDevice;
    context->physicalDevice = PhysicalDevice::create(bestDevice, context->surface);
    
    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createDevice(VulkanContext *context, DeviceExtensionList extensions)
{
    VkPhysicalDevice physicalDevice = context->physicalDevice->getHandle();
    if (!physicalDevice)
        return FailedCreateDevice;

    const QueueFamilies& queueFamilies = context->physicalDevice->getQueueFamilies();

    uint32_t graphicsfamilyIndex = queueFamilies.getQueueGraphicsFamily();
    uint32_t transferFamilyIndex = queueFamilies.getQueueTransferFamily();

    float priorities[] = {1.0f, 1.0};

    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

    {
        auto iter = std::find_if(deviceQueueCreateInfos.begin(), deviceQueueCreateInfos.end(), [familyIndex = graphicsfamilyIndex](VkDeviceQueueCreateInfo &deviceQueueCreateInfo){
            return deviceQueueCreateInfo.queueFamilyIndex == familyIndex;
        });

        if(iter != deviceQueueCreateInfos.end()) {
            iter->queueCount += 1;
        }

        else {

            VkDeviceQueueCreateInfo deviceQueueCreateInfo;

            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.pNext = nullptr;
            deviceQueueCreateInfo.flags = {};
            deviceQueueCreateInfo.queueFamilyIndex = graphicsfamilyIndex;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = priorities;

            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }
    }

    {
        auto iter = std::find_if(deviceQueueCreateInfos.begin(), deviceQueueCreateInfos.end(), [familyIndex = transferFamilyIndex](VkDeviceQueueCreateInfo &deviceQueueCreateInfo){
            return deviceQueueCreateInfo.queueFamilyIndex == familyIndex;
        });

        if(iter != deviceQueueCreateInfos.end()) {
            iter->queueCount += 1;
        }

        else {

            VkDeviceQueueCreateInfo deviceQueueCreateInfo;

            deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            deviceQueueCreateInfo.pNext = nullptr;
            deviceQueueCreateInfo.flags = {};
            deviceQueueCreateInfo.queueFamilyIndex = transferFamilyIndex;
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = priorities;

            deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
        }
    }

    VkPhysicalDeviceFeatures enabledFeatures = {};

    VkDeviceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
        .pQueueCreateInfos = deviceQueueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &enabledFeatures
    };

    if (VkResult result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &context->device); result != VK_SUCCESS)
    {
        return VulkanContextResult::FailedCreateDevice;
    }

    context->graphics.familyIndex = graphicsfamilyIndex;
    vkGetDeviceQueue(context->device, graphicsfamilyIndex, 0, &context->graphics.queue);

    context->transfer.familyIndex = transferFamilyIndex;
    vkGetDeviceQueue(context->device, transferFamilyIndex, 0, &context->transfer.queue);

    return VulkanContextResult::Success;
}

NODISCARD VkPresentModeKHR choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

NODISCARD VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
{
    VkExtent2D extent;

    if (capabilities.currentExtent.width == 0xFFFFFFFF)
    {
        extent.width = capabilities.minImageExtent.width;
    }

    else
    {
        extent.width = capabilities.currentExtent.width;
    }

    if (capabilities.currentExtent.height == 0xFFFFFFFF)
    {
        extent.height = capabilities.minImageExtent.height;
    }

    else
    {
        extent.height = capabilities.currentExtent.height;
    }

    return extent;
}

NODISCARD VkSurfaceFormatKHR chooseSurfaceFormat(const VkPhysicalDevice physicalDevice, const std::vector<VkSurfaceFormatKHR> &availableFormats, const VkImageUsageFlags usage)
{
    uint32_t index = 0;
    for (uint32_t x = 0; x < availableFormats.size(); x++)
    {

        VkImageFormatProperties formatProperties;

        VkResult result = vkGetPhysicalDeviceImageFormatProperties(physicalDevice, availableFormats[x].format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usage, 0, &formatProperties);

        if (result == VK_ERROR_FORMAT_NOT_SUPPORTED)
        {
            std::cout << "Swapchain format does not support requested usage flags" << std::endl;
        }
        else
        {
            index = x;
            break;
        }
    }

    return availableFormats[index];
}

NODISCARD VulkanContextResult createVulkanSwapchain(VulkanContext *context, VkImageUsageFlags usage)
{
    const PhysicalDevice::Pointer physicalDevice = context->physicalDevice;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(context->physicalDevice->getHandle(), context->surface, &surfaceCapabilities);

    PhysicalDevice::SurfacePresentModes availablePresentModes = physicalDevice->getSurfacePresentModes();
    PhysicalDevice::SurfaceFormats availableSurfaceFormats = physicalDevice->getSurfaceFormats();

    VkPresentModeKHR presentMode = choosePresentMode(availablePresentModes);
    VkExtent2D extent = chooseExtent(surfaceCapabilities);
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(physicalDevice->getHandle(), availableSurfaceFormats, usage);
    VkFormat format = surfaceFormat.format;

    VkSwapchainCreateInfoKHR createInfo{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = {},
        .surface = context->surface,
        .minImageCount = 3,
        .imageFormat = format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = surfaceCapabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = usage,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = presentMode,
        .oldSwapchain = nullptr};

    if (VkResult result = vkCreateSwapchainKHR(context->device, &createInfo, nullptr, &context->swapchain); result != VK_SUCCESS)
    {
        return VulkanContextResult::FailedCreateSwapchain;
    }

    uint32_t amountOfSwapchainImages;
    vkGetSwapchainImagesKHR(context->device, context->swapchain, &amountOfSwapchainImages, nullptr);
    context->images.resize(amountOfSwapchainImages);
    vkGetSwapchainImagesKHR(context->device, context->swapchain, &amountOfSwapchainImages, &context->images[0]);

    context->swapchainFormat = format;

    context->width = extent.width;
    context->height = extent.height;

    context->amountOfFrames = context->images.size();

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createSwapchainViewImages(VulkanContext *context)
{
    if (!context->swapchain || context->images.empty())
        return VulkanContextResult::FailedCreateSwapchainViewImages;

    context->imageViews.resize(context->amountOfFrames);

    VkImageViewCreateInfo imageViewCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = context->swapchainFormat,
        .components = {
            .r = VK_COMPONENT_SWIZZLE_R,
            .g = VK_COMPONENT_SWIZZLE_G,
            .b = VK_COMPONENT_SWIZZLE_B,
            .a = VK_COMPONENT_SWIZZLE_A},
        .subresourceRange{.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    for (unsigned long x = 0; x < context->amountOfFrames; x++)
    {

        imageViewCreateInfo.image = context->images[x];

        if (VkResult result = vkCreateImageView(context->device, &imageViewCreateInfo, nullptr, &context->imageViews[x]); result != VK_SUCCESS)
        {
            return VulkanContextResult::FailedCreateSwapchainViewImages;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createRenderPass(VulkanContext *context) {

   context->renderPass = createRenderPass(context->device, context->swapchainFormat);

    if (!context->renderPass)
    {
        return VulkanContextResult::FailedCreateRenderPass;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createSwapchainFramebuffers(VulkanContext *context)
{
    context->framebuffers.resize(context->amountOfFrames);
    for (unsigned long x = 0; x < context->amountOfFrames; x++) {

        Framebuffer::Attachments attachments { context->imageViews[x] };
        context->framebuffers[x] = Framebuffer::create(context->device, context->renderPass, attachments, context->width, context->height);   
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createCommandPool(VulkanContext *context) {

    context->commandPools.resize(context->amountOfFrames);
    for (unsigned long x = 0; x < context->amountOfFrames; x++) {
        context->commandPools[x] = CommandPool::create(context->device, context->graphics.familyIndex);
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createTransferCommandPool(VulkanContext * context) {

    if (!context->device)
        return FailedCreateCommandPool;

    VkCommandPoolCreateInfo commandPoolCreateInfo {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
        .queueFamilyIndex = context->transfer.familyIndex 
    };


    if (VkResult result = vkCreateCommandPool(context->device, &commandPoolCreateInfo, nullptr, &context->transferCommandPool); result != VK_SUCCESS)
    {
        return VulkanContextResult::FailedCreateCommandPool;
    }
    
    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult allocateCommandBuffer(VulkanContext *context) {

    context->commandBuffers.resize(context->amountOfFrames);
    for (uint32_t x = 0; x < context->amountOfFrames; x++) {
        context->commandBuffers[x] = context->commandPools[x]->allocateCommandBuffer();
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createFence(VulkanContext *context)
{

    context->fences.resize(context->amountOfFrames);

    VkFenceCreateInfo fenceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_FENCE_CREATE_SIGNALED_BIT};

    for (uint32_t x = 0; x < context->amountOfFrames; x++)
    {

        if (VkResult result = vkCreateFence(context->device, &fenceCreateInfo, nullptr, &context->fences[x]); result != VK_SUCCESS)
        {
            return VulkanContextResult::FailedCreateFence;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createSemaphore(VulkanContext *context)
{

    context->waitSemaphore.resize(context->amountOfFrames);
    context->signalSemaphore.resize(context->amountOfFrames);

    VkSemaphoreCreateInfo semaphoreCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {}};

    for (VkSemaphore &semaphore : context->signalSemaphore)
    {

        if (VkResult result = vkCreateSemaphore(context->device, &semaphoreCreateInfo, nullptr, &semaphore); result != VK_SUCCESS)
        {
            return VulkanContextResult::FailedCreateSemaphore;
        }
    }

    for (VkSemaphore &semaphore : context->waitSemaphore)
    {

        if (VkResult result = vkCreateSemaphore(context->device, &semaphoreCreateInfo, nullptr, &semaphore); result != VK_SUCCESS)
        {
            return VulkanContextResult::FailedCreateSemaphore;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult recordCommandBuffer(VulkanContext *context)
{
    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult initVulkan(SDL_Window *window, VulkanContext *context)
{
    uint32_t count;
    SDL_Vulkan_GetInstanceExtensions(&count, nullptr);
    InstanceExtensionList extensions(count);
    SDL_Vulkan_GetInstanceExtensions(&count, extensions.data());

    DeviceExtensionList deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    if (createVulkanInstance(context, extensions) == VulkanContextResult::FailedCreateInstance)
    {
        return VulkanContextResult::FailedCreateInstance;
    }

    if (createVulkanSurface(context, window) == VulkanContextResult::FailedCreateSurface)
    {
        return VulkanContextResult::FailedCreateSurface;
    }

    if (choosePhysicalDevice(context) == VulkanContextResult::NoSuitablePhysicalDevice)
    {
        return VulkanContextResult::NoSuitablePhysicalDevice;
    }

    if (createDevice(context, deviceExtensions) == VulkanContextResult::FailedCreateDevice)
    {
        return VulkanContextResult::FailedCreateDevice;
    }

    if (createVulkanSwapchain(context, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) == VulkanContextResult::FailedCreateSwapchain)
    {
        return VulkanContextResult::FailedCreateSwapchain;
    }

    if (createSwapchainViewImages(context) == VulkanContextResult::FailedCreateSwapchainViewImages)
    {
        return FailedCreateSwapchainViewImages;
    }

    if (createRenderPass(context) == VulkanContextResult::FailedCreateRenderPass)
    {
        return VulkanContextResult::FailedCreateRenderPass;
    }

    if (createSwapchainFramebuffers(context) == VulkanContextResult::FailedCreateSwapchainFramebuffer)
    {
        return VulkanContextResult::FailedCreateSwapchainFramebuffer;
    }

    if(createCommandPool(context) == VulkanContextResult::FailedCreateCommandPool)
    {
        return VulkanContextResult::FailedCreateCommandPool;
    }

    if(createTransferCommandPool(context) == VulkanContextResult::FailedCreateCommandPool) 
    {
        return VulkanContextResult::FailedCreateCommandPool;
    }

    if (allocateCommandBuffer(context) == VulkanContextResult::FailedAllocateCommandBuffer)
    {
        return VulkanContextResult::FailedAllocateCommandBuffer;
    }

    if (recordCommandBuffer(context) == VulkanContextResult::FailedRecordCommandBuffer)
    {
        return FailedRecordCommandBuffer;
    }

    if (createFence(context) == VulkanContextResult::FailedCreateFence)
    {
        return VulkanContextResult::FailedCreateFence;
    }

    if (createSemaphore(context) == VulkanContextResult::FailedCreateSemaphore)
    {
        return VulkanContextResult::FailedCreateSemaphore;
    }

    return VulkanContextResult::Success;
}

VulkanContextResult render(VulkanContext *context, std::span<std::shared_ptr<Command>> commands)
{
        uint32_t frameIndex = context->frameIndex;
        VkFence fence = context->fences[frameIndex];

        VkSemaphore release = context->waitSemaphore[frameIndex];
        VkSemaphore acquire = context->signalSemaphore[frameIndex];

        VkResult result = vkWaitForFences(context->device, 1, &fence, VK_TRUE, UINT64_MAX);
        result = vkResetFences(context->device, 1, &fence);
        context->commandPools[frameIndex]->reset();

        uint32_t imageIndex;
        result = vkAcquireNextImageKHR(context->device, context->swapchain, UINT64_MAX, acquire, VK_NULL_HANDLE, &imageIndex);
        if (result != VK_SUCCESS) {
            std::cout << "Error: Acquire Next Image" << std::endl;
        }


        VkCommandBufferBeginInfo beginInfo = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        result = vkBeginCommandBuffer(context->commandBuffers[frameIndex]->getHandle(), &beginInfo);
        
    
        VkClearValue clearValue = { 0.1f, 0.1, 0.1f, 1.0f };
        VkRenderPassBeginInfo renderPassBeginInfo = { VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
        renderPassBeginInfo.renderPass = context->renderPass->getHandle();
        renderPassBeginInfo.framebuffer = context->framebuffers[frameIndex]->getHandle();
        renderPassBeginInfo.renderArea = { {0, 0}, {context->width, context->height} };
        renderPassBeginInfo.clearValueCount = 1;
        renderPassBeginInfo.pClearValues = &clearValue;
        vkCmdBeginRenderPass(context->commandBuffers[frameIndex]->getHandle(), &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        for(auto command : commands) {
            command->record(context->commandBuffers[frameIndex]);
        }

        vkCmdEndRenderPass(context->commandBuffers[frameIndex]->getHandle());
        vkEndCommandBuffer(context->commandBuffers[frameIndex]->getHandle());

        VkCommandBuffer commandBuffer = context->commandBuffers[frameIndex]->getHandle();

        VkSubmitInfo submitInfo = { VK_STRUCTURE_TYPE_SUBMIT_INFO };
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &acquire;
        VkPipelineStageFlags waitMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submitInfo.pWaitDstStageMask = &waitMask;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &release;
        result = vkQueueSubmit(context->graphics.queue, 1, &submitInfo, fence);

        VkSwapchainKHR swapchain = context->swapchain;

        VkPresentInfoKHR presentInfo = { VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &release;

        result = vkQueuePresentKHR(context->graphics.queue, &presentInfo);

        context->frameIndex++;
        context->frameIndex = context->frameIndex % context->amountOfFrames;

    return VulkanContextResult::Success;
}

void terminateVulkan(VulkanContext *context)
{
    VkInstance instance = context->instance;
    VkDevice device = context->device;

    if (device)
    {

        for (VkSemaphore &semaphore : context->waitSemaphore)
        {
            vkDestroySemaphore(context->device, semaphore, nullptr);
        }

        for (VkSemaphore &semaphore : context->signalSemaphore)
        {
            vkDestroySemaphore(context->device, semaphore, nullptr);
        }

        for (VkFence &fence : context->fences)
        {
            vkDestroyFence(context->device, fence, nullptr);
        }

        vkDestroyCommandPool(context->device, context->transferCommandPool, nullptr);

        for(auto& pool : context->commandPools) {
            pool.reset();
        }

        for (auto& framebuffer : context->framebuffers) {
            framebuffer.reset();
        }

        context->renderPass.reset();

        for (VkImageView view : context->imageViews)
        {
            vkDestroyImageView(context->device, view, nullptr);
        }

        if (context->swapchain) {
            vkDestroySwapchainKHR(context->device, context->swapchain, nullptr);
        }

        vkDestroyDevice(context->device, nullptr);
    }

    if (instance && context->surface)
    {
        vkDestroySurfaceKHR(context->instance, context->surface, nullptr);
    }

    if (instance)
    {
        vkDestroyInstance(context->instance, nullptr);
    }
}


std::optional<uint32_t> findMemoryType(VulkanContext *context, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{   
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(context->physicalDevice->getHandle(), &memoryProperties);

    for (uint32_t memoryTypeIndex = 0; memoryTypeIndex < memoryProperties.memoryTypeCount; ++memoryTypeIndex)
    {
        if ((typeFilter & (1 << memoryTypeIndex)) &&
            (memoryProperties.memoryTypes[memoryTypeIndex].propertyFlags & properties))
        {
            return std::optional<uint32_t>(memoryTypeIndex);
        }
    }

    return std::nullopt;
}

std::optional<VulkanBuffer> createBuffer(VulkanContext *context, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size) {

    VkBuffer buffer;

    VkBufferCreateInfo bufferCreateInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .size = size,
        .usage = usage,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr};

    if (VkResult result = vkCreateBuffer(context->device, &bufferCreateInfo, nullptr, &buffer); result != VK_SUCCESS)
    {
        return std::nullopt;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(context->device, buffer, &memoryRequirements);

    std::optional<uint32_t> memoryTypeIndex = findMemoryType(context, memoryRequirements.memoryTypeBits, properties);
    if (!memoryTypeIndex.has_value())
    {
        return std::nullopt;
    }

    VkMemoryAllocateInfo memoryAllocateInfo{
        .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex.value()};

    VkDeviceMemory memory;
    if (VkResult result = vkAllocateMemory(context->device, &memoryAllocateInfo, nullptr, &memory); result != VK_SUCCESS)
    {
        return std::nullopt;
    }

    vkBindBufferMemory(context->device, buffer, memory, 0);

    return std::optional<VulkanBuffer>({buffer, memory, usage});
}

template<typename T>
std::optional<VulkanBuffer> createBuffer(VulkanContext *context, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, std::vector<T> &vertices)
{
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    auto buffer = createBuffer(context, usage, properties, bufferSize);

    if(buffer.has_value()) {
        void* map;
        vkMapMemory(context->device, buffer.value().memory, 0, bufferSize, 0, &map);
        memcpy(map, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(context->device, buffer.value().memory);
    }

    return buffer;
}

void copyBuffer(VulkanContext* context, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;

    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = context->transferCommandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(context->device, &commandBufferAllocateInfo, &commandBuffer);
    if(result != VK_SUCCESS) {
        return;
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo;
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    if(result != VK_SUCCESS) {
        return;
    }

    VkBufferCopy bufferCopy;
    bufferCopy.srcOffset = 0;
    bufferCopy.dstOffset = 0;
    bufferCopy.size = size;

    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

    result = vkEndCommandBuffer(commandBuffer);
    if(result != VK_SUCCESS) {
        return;
    }

    VkPipelineStageFlags stageMask = VK_PIPELINE_STAGE_NONE;

    VkSubmitInfo submitInfo;
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.pWaitDstStageMask = &stageMask;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;

    vkQueueSubmit(context->transfer.queue, 1, &submitInfo, VK_NULL_HANDLE);

    result = vkQueueWaitIdle(context->transfer.queue);
    if(result != VK_SUCCESS) {
        return;
    }

    vkFreeCommandBuffers(context->device, context->transferCommandPool, 1, &commandBuffer);
}

template<typename T>
std::optional<VulkanBuffer> createVertexStagingBuffer(VulkanContext *context, std::vector<T> &vertices) {

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto stagingBuffer = createBuffer(context, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertices); 

    if(!stagingBuffer.has_value()) {
        return std::nullopt;
    }

    auto buffer = createBuffer(context, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

    copyBuffer(context, stagingBuffer.value().buffer, buffer.value().buffer, bufferSize);

    vkFreeMemory(context->device, stagingBuffer.value().memory, nullptr);
    vkDestroyBuffer(context->device, stagingBuffer.value().buffer, nullptr);

    return buffer;
}

template<typename T>
std::optional<VulkanBuffer> createIndexStagingBuffer(VulkanContext *context, std::vector<T> &vertices) {

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    auto stagingBuffer = createBuffer(context, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, vertices); 

    if(!stagingBuffer.has_value()) {
        return std::nullopt;
    }

    auto buffer = createBuffer(context, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, bufferSize);

    copyBuffer(context, stagingBuffer.value().buffer, buffer.value().buffer, bufferSize);

    vkFreeMemory(context->device, stagingBuffer.value().memory, nullptr);
    vkDestroyBuffer(context->device, stagingBuffer.value().buffer, nullptr);

    return buffer;
}

enum VulkanPipelineResult
{
    Succes,
    FailedReadVertexFile,
    FailedReadFragementFile,
    FailedVertexShaderModule,
    FailedFragmentShaderModule,
    FailedCreatePipelineLayout,
    FailedCreatePipeline
};

NODISCARD static std::optional<std::vector<uint8_t>> readFile(std::string filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        return std::nullopt;
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<uint8_t> buffer(fileSize);

    file.seekg(0);
    file.read(reinterpret_cast<char*>(buffer.data()), fileSize);
    file.close();

    return buffer;
}

NODISCARD std::optional<VkShaderModule> createShaderModule(VulkanContext *context, std::vector<uint8_t> &code)
{

    VkShaderModuleCreateInfo shaderModuleCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .codeSize = static_cast<uint32_t>(code.size()),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };

    VkShaderModule shaderModule;
    if (VkResult result = vkCreateShaderModule(context->device, &shaderModuleCreateInfo, nullptr, &shaderModule); result != VK_SUCCESS)
    {
        return std::nullopt;
    }

    return std::make_optional<VkShaderModule>(shaderModule);
}

NODISCARD std::optional<VkDescriptorPool> createDescriptorPool(VulkanContext* context, DescriptorPoolSizeList poolSize, uint32_t maxSets = 1000) {

    VkDescriptorPool descriptorPool;

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.pNext = nullptr;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = maxSets;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSize.size());
    descriptorPoolCreateInfo.pPoolSizes = poolSize.data();

    if(VkResult result = vkCreateDescriptorPool(context->device, &descriptorPoolCreateInfo, nullptr, &descriptorPool); result != VK_SUCCESS) {
        return std::nullopt;
    }

    return descriptorPool;
}

NODISCARD std::optional<VkDescriptorSet> createDescriptor(VulkanContext* context, VkDescriptorPool descriptorPool, VkDescriptorSetLayout layout) {

    VkDescriptorSet descriptorSet;

    VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
    descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocateInfo.pNext = nullptr;
    descriptorSetAllocateInfo.descriptorPool = descriptorPool;
    descriptorSetAllocateInfo.descriptorSetCount = 1;
    descriptorSetAllocateInfo.pSetLayouts = &layout;

    if(VkResult result = vkAllocateDescriptorSets(context->device, &descriptorSetAllocateInfo, &descriptorSet); result != VK_SUCCESS) {
        return std::nullopt;
    }

    return descriptorSet;
}

NODISCARD std::optional<VkDescriptorSetLayout> createDescriptorSetLayout(VulkanContext* context, VulkanDescriptorSetLayoutBindingList bindings) {

    VkDescriptorSetLayout descriptorSetLayout;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pNext = nullptr;
    descriptorSetLayoutCreateInfo.flags = 0;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();

    if(VkResult result = vkCreateDescriptorSetLayout(context->device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout); result != VK_SUCCESS) {
        return std::nullopt;
    }

    return descriptorSetLayout;
}

void updateDescriptorBuffer(VulkanContext* context, VkBuffer buffer, VkDeviceSize range, VkDescriptorSet descriptorSet, uint32_t binding, VkDescriptorType descriptorType) {

    VkDescriptorBufferInfo descriptorBufferInfo;
    descriptorBufferInfo.buffer = buffer;
    descriptorBufferInfo.offset = 0;
    descriptorBufferInfo.range = range;

    VkWriteDescriptorSet writeDescriptorSet;
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.pNext = nullptr;
    writeDescriptorSet.dstSet = descriptorSet;
    writeDescriptorSet.dstBinding = binding;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.descriptorType = descriptorType;
    writeDescriptorSet.pImageInfo = nullptr;
    writeDescriptorSet.pBufferInfo = &descriptorBufferInfo;
    writeDescriptorSet.pTexelBufferView = nullptr;

    vkUpdateDescriptorSets(context->device, 1, &writeDescriptorSet, 0, nullptr);
}

template<typename T>
void updateUniformBuffer(VulkanContext* context, VkDeviceMemory memory, VkDeviceSize size, const T* data) {

    void* map;
    vkMapMemory(context->device, memory, 0, size, 0, &map);
    memcpy(map, data, size);
    vkUnmapMemory(context->device, memory);
}

NODISCARD static std::pair<SVulkanPipeline, VulkanPipelineResult> createPipeline(VulkanContext *context, std::string vertexShaderFile, std::string fragmentShaderFile, std::span<const VkDescriptorSetLayout> descriptorSetLayout = {})
{

    SVulkanPipeline pipeline;

    auto vertexShaderCode = readFile(vertexShaderFile);
    if (!vertexShaderCode.has_value())
    {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedReadVertexFile);
    }

    auto fragmentShaderCode = readFile(fragmentShaderFile);
    if (!fragmentShaderCode.has_value())
    {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedReadFragementFile);
    }

    auto vertexShaderModule = createShaderModule(context, vertexShaderCode.value());
    if (!vertexShaderModule.has_value())
    {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedVertexShaderModule);
    }

    auto fragmentShaderModule = createShaderModule(context, fragmentShaderCode.value());
    if (!fragmentShaderModule.has_value())
    {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedFragmentShaderModule);
    }

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShaderModule.value();
	shaderStages[0].pName = "main";

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = fragmentShaderModule.value();
	shaderStages[1].pName = "main";

	std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributeDescriptions {};
	vertexInputAttributeDescriptions[0].binding = 0;
    vertexInputAttributeDescriptions[0].location = 0;
    vertexInputAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescriptions[0].offset = 0;

	vertexInputAttributeDescriptions[1].binding = 0;
    vertexInputAttributeDescriptions[1].location = 1;
    vertexInputAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescriptions[1].offset = sizeof(float) * 3;

	std::array<VkVertexInputBindingDescription, 1> vertexInputBindingDescriptions {};
	vertexInputBindingDescriptions[0].binding = 0;
    vertexInputBindingDescriptions[0].stride = sizeof(float) * 6;
    vertexInputBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {};
    vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo.pNext = nullptr;
    vertexInputStateCreateInfo.flags = 0;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexInputAttributeDescriptions.size());
	vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescriptions.data();
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexInputBindingDescriptions.size());
	vertexInputStateCreateInfo.pVertexBindingDescriptions = vertexInputBindingDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	std::array<VkViewport, 1> viewports {};
	viewports[0].width = static_cast<float>(context->width);
	viewports[0].height = static_cast<float>(context->height);

	std::array<VkRect2D, 1> scissors {};
	scissors[0].extent.width = context->width;
	scissors[0].extent.height = context->height;

	VkPipelineViewportStateCreateInfo viewportState {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.pNext = nullptr;
    viewportState.flags = 0;
    viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
    viewportState.pViewports = viewports.data();
    viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
    viewportState.pScissors = scissors.data();

	VkPipelineRasterizationStateCreateInfo rasterizationState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
	    .cullMode = VK_CULL_MODE_FRONT_BIT,
	    .frontFace = VK_FRONT_FACE_CLOCKWISE,
		.lineWidth = 1.0f
    };

	VkPipelineMultisampleStateCreateInfo multisampleState {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT
    };

	std::array<VkPipelineColorBlendAttachmentState, 1> colorBlendAttachments {};
	colorBlendAttachments[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachments[0].alphaBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachments[0].blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendState {};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlendState.pAttachments = colorBlendAttachments.data();

	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayout.size());
    pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayout.data();

    if(VkResult result = vkCreatePipelineLayout(context->device, &pipelineLayoutCreateInfo, nullptr, &pipeline.layout); result != VK_SUCCESS) {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedCreatePipelineLayout);
    }

	VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
	graphicsPipelineCreateInfo.pStages = shaderStages.data();
	graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
	graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	graphicsPipelineCreateInfo.pViewportState = &viewportState;
	graphicsPipelineCreateInfo.pRasterizationState = &rasterizationState;
	graphicsPipelineCreateInfo.pMultisampleState = &multisampleState;
	graphicsPipelineCreateInfo.pColorBlendState = &colorBlendState;
	graphicsPipelineCreateInfo.layout = pipeline.layout;
	graphicsPipelineCreateInfo.renderPass = context->renderPass->getHandle();
	graphicsPipelineCreateInfo.subpass = 0;

	if(VkResult result = vkCreateGraphicsPipelines(context->device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline.pipeline); result != VK_SUCCESS) {
        return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedCreatePipeline);
	} 

    vkDestroyShaderModule(context->device, vertexShaderModule.value(), nullptr);
    vkDestroyShaderModule(context->device, fragmentShaderModule.value(), nullptr);

    return std::make_pair<SVulkanPipeline, VulkanPipelineResult>(std::move(pipeline), VulkanPipelineResult::Succes);
}

std::array<float, 16> createProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) {

    std::array<float, 16> projectionMatrix;

    // Calculate the parameters of the projection matrix
    float f = 1.0f / std::tan(fov * 0.5f);
    float rangeInv = 1.0f / (nearPlane - farPlane);

    // Fill in the projection matrix elements
    projectionMatrix[0] = f / aspectRatio;  projectionMatrix[4] = 0.0f;     projectionMatrix[8] = 0.0f;                                 projectionMatrix[12] = 0.0f;
    projectionMatrix[1] = 0.0f;             projectionMatrix[5] = f;        projectionMatrix[9] = 0.0f;                                 projectionMatrix[13] = 0.0f;
    projectionMatrix[2] = 0.0f;             projectionMatrix[6] = 0.0f;     projectionMatrix[10] = (nearPlane + farPlane) * rangeInv;   projectionMatrix[14] = nearPlane * farPlane * rangeInv * 2.0f;
    projectionMatrix[3] = 0.0f;             projectionMatrix[7] = 0.0f;     projectionMatrix[11] = -1.0f;                               projectionMatrix[15] = 0.0f;

    return projectionMatrix;
}


void destroyPipeline(VulkanContext* context, SVulkanPipeline& pipeline) {

    vkDestroyPipelineLayout(context->device, pipeline.layout, nullptr);
    vkDestroyPipeline(context->device, pipeline.pipeline, nullptr);
}

NODISCARD static std::string pipelineErrorToString(VulkanPipelineResult result)
{
    switch (result)
    {
    case FailedReadVertexFile:
        return std::string("Error: Failed to read vertex file");
    case FailedReadFragementFile:
        return std::string("Error; Failed to read fragment file");
    case FailedVertexShaderModule:
        return std::string("Error: Failed to create vertex shader module");
    case FailedFragmentShaderModule:
        return std::string("Error: Failed to create fragment shader module");
    case FailedCreatePipelineLayout:
        return std::string("Error: Failed to create pipeline layout");
    case FailedCreatePipeline:
        return std::string("Error: Failed to create pipeline");
    }

    return std::string();
}

void destroyBuffer(VulkanContext *context, std::optional<VulkanBuffer> buffer)
{

    if (!buffer.has_value())
    {
        return;
    }

    vkDestroyBuffer(context->device, buffer.value().buffer, nullptr);
    vkFreeMemory(context->device, buffer.value().memory, nullptr);
}

localsystem camera {};
std::vector<UniformBuffer> uniformBuffer;

bool handleMessage() {

        float step = 0.001f;

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {

		    	case SDL_EVENT_QUIT:
			    	return false;

			default:
				break;
			}
		}

        const uint8_t* keystates = SDL_GetKeyboardState(nullptr);

        if(keystates[SDL_SCANCODE_DOWN]) {
            camera.pos = camera.pos + step * camera.sight;
        }

        else if(keystates[SDL_SCANCODE_UP]) {
            camera.pos = camera.pos + (-step) * camera.sight;
        }

        else if(keystates[SDL_SCANCODE_LEFT]) {
            camera.pos = camera.pos + (-step) * camera.right;
        }

        else if(keystates[SDL_SCANCODE_RIGHT]) {
            camera.pos = camera.pos + step * camera.right;
        }

        if(keystates[SDL_SCANCODE_D]) {

            Matrix matrix;
            
            for(uint32_t x=0;x<uniformBuffer.size();x++) {


                matrix.translate(-camera.pos.wx, -camera.pos.wy, -camera.pos.wz);
                matrix.rotate_y(-0.01);
                matrix.translate(camera.pos.wx, camera.pos.wy, camera.pos.wz);
            
                camera = matrix * camera;
            }
        }

        else if(keystates[SDL_SCANCODE_A]) {

            Matrix matrix;
            
            for(uint32_t x=0;x<uniformBuffer.size();x++) {

                matrix.translate(-camera.pos.wx, -camera.pos.wy, -camera.pos.wz);
                matrix.rotate_y(0.01);
                matrix.translate(camera.pos.wx, camera.pos.wy, camera.pos.wz);
            
                camera = matrix * camera;
            }
        }
        
		return true;
	}

int main()
{
    if (SDL_Init(SDL_INIT_VIDEO))
    {
        return EXIT_FAILURE;
    }

    SDL_Window* window = SDL_CreateWindow("Hello Vulkan", 1280, 720, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);

    VulkanContext context;
    if (VulkanContextResult result = initVulkan(window, &context); result != VulkanContextResult::Success)
    {
        terminateVulkan(&context);
        std::cout << vulkanErrorToString(result) << std::endl;
        return EXIT_FAILURE;
    }
    
    std::vector<float> vertices {
        -1.0, -1.0, 0.0, 1.0, 0.0, 0.0,
        -1.0, 1.0, 0.0, 0.0, 1.0, 0.0,
         1.0, 1.0, 0.0, 1.0, 0.0, 1.0,
        1.0, -1.0, 0.0, 0.0, 1.0, 0.0,
    };

    std::vector<uint16_t> indices {
        0, 1, 2, 2, 3, 0
    };

    std::optional<VulkanBuffer> buffer;
    if (buffer = createVertexStagingBuffer(&context, vertices); !buffer.has_value()) {
        return EXIT_FAILURE;
    }

    std::optional<VulkanBuffer> indicesBuffer;
    if (indicesBuffer = createIndexStagingBuffer(&context, indices); !indicesBuffer.has_value()) {
        return EXIT_FAILURE;
    }

    camera.pos.wz += 3;

    uniformBuffer.resize(context.amountOfFrames);

    for(uint32_t x=0;x<uniformBuffer.size();x++) {
        uniformBuffer[x].projectionMatrix = createProjectionMatrix(45.f, static_cast<float>(context.width) / context.height, 1.f, 1000.f);

        uniformBuffer[x].camera.translate(-camera.pos.wx, -camera.pos.wy, -camera.pos.wz);
        uniformBuffer[x].camera.rows(camera.right, camera.up, camera.sight);
    }

    std::vector<std::optional<VulkanBuffer>> matrixBuffer(context.amountOfFrames);
    for(uint32_t x=0;x<matrixBuffer.size();x++) {
        if(matrixBuffer[x] = createBuffer(&context, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, uniformBuffer); !matrixBuffer[x].has_value()) {
            return EXIT_FAILURE;
        }
    }

    std::optional<VkDescriptorPool> descriptorPool;
    DescriptorPoolSizeList descriptorPoolList {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 100 }
    };

    if(descriptorPool = createDescriptorPool(&context, descriptorPoolList); !descriptorPool.has_value()) {
        return EXIT_FAILURE;
    }

    std::optional<VkDescriptorSetLayout> descriptorSetLayout;
    VulkanDescriptorSetLayoutBindingList descriptorSetLayoutBindingList {
        { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr }
    };

    if(descriptorSetLayout = createDescriptorSetLayout(&context, descriptorSetLayoutBindingList); !descriptorSetLayout.has_value()) {
        return EXIT_FAILURE;
    }

    std::vector<std::optional<VkDescriptorSet>> descriptorSetList(context.amountOfFrames);
    for(uint32_t x=0;x<context.amountOfFrames;x++) {
        descriptorSetList[x] = createDescriptor(&context, descriptorPool.value(), descriptorSetLayout.value());
    }

    for(uint32_t x=0;x<context.amountOfFrames;x++) {
        updateDescriptorBuffer(&context, matrixBuffer[x].value().buffer, sizeof(UniformBuffer), descriptorSetList[x].value(), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
    }

    std::vector<VkDescriptorSetLayout> layouts { descriptorSetLayout.value() };
    std::pair<SVulkanPipeline, VulkanPipelineResult> pipeline = createPipeline(&context, "../shaders/VertexShader.spv", "../shaders/FragmentShader.spv", layouts);
    if (pipeline.second != VulkanPipelineResult::Succes) {
        std::cout << pipelineErrorToString(pipeline.second) << std::endl;
        destroyBuffer(&context, buffer);
        terminateVulkan(&context);
        return EXIT_FAILURE;
    }

    std::vector<std::vector<std::shared_ptr<Command>>> commands(context.amountOfFrames);
    for(uint32_t x=0; x<context.amountOfFrames; x++) {
        commands[x].push_back(CmdBindVertexBuffers::create(0, buffer->buffer));
        commands[x].push_back(CmdBindIndexBuffer::create(indicesBuffer.value().buffer, VK_INDEX_TYPE_UINT16));
        commands[x].push_back(CmdBindDescriptorSets::create(pipeline.first.layout, 0, std::vector<VkDescriptorSet> { descriptorSetList[x].value() }));
        commands[x].push_back(CmdBindGraphicsPipeline::create(pipeline.first.pipeline));
        commands[x].push_back(CmdDrawIndexed::create(6, 1, 0, 0, 0));
    }

    SDL_ShowWindow(window);

    bool running = true;
    while (handleMessage()) {
    
        for(uint32_t x=0;x<context.amountOfFrames;x++) {
            uniformBuffer[x].modelMatrix.rotate_z(0.001);
            updateUniformBuffer(&context, matrixBuffer[x].value().memory, sizeof(UniformBuffer), &uniformBuffer[x]);

            uniformBuffer[x].camera.clear();
            uniformBuffer[x].camera.translate(-camera.pos.wx, -camera.pos.wy, -camera.pos.wz);
            uniformBuffer[x].camera.rows(camera.right, camera.up, camera.sight);
        }

        render(&context, commands[context.frameIndex]);
    }

    SDL_HideWindow(window);
    vkDeviceWaitIdle(context.device);

    destroyPipeline(&context, pipeline.first);

    for(uint32_t x=0;x<descriptorSetList.size(); x++) {
        vkFreeDescriptorSets(context.device, descriptorPool.value(), 1, &descriptorSetList[x].value());
    }

    vkDestroyDescriptorSetLayout(context.device, descriptorSetLayout.value(), nullptr);

    vkDestroyDescriptorPool(context.device, descriptorPool.value(), nullptr);

    for(uint32_t x=0;x<matrixBuffer.size();x++) {
        destroyBuffer(&context, matrixBuffer[x]);
    }

    destroyBuffer(&context, indicesBuffer);
    destroyBuffer(&context, buffer);

    terminateVulkan(&context);

    return EXIT_SUCCESS;
}
