#define GLFW_INCLUDE_VULKAN
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include <vulkan/vulkan.hpp>
#include <iostream>
#include <vector>
#include <optional>
#include <fstream>
#include "Window.hpp"

#define NODISCARD [[nodiscard]]

using PhysicalDeviceList = std::vector<vk::PhysicalDevice>;

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
    FailedCreateSemaphore
};

struct VulkanQueue
{
    uint32_t familyIndex;
    vk::Queue queue;
};

struct VulkanBuffer
{
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::BufferUsageFlagBits usage;
};

class SVulkanPipeline {
    
    public:
        vk::PipelineLayout layout;
        vk::Pipeline pipeline;

    public:
        constexpr SVulkanPipeline() = default;
        constexpr SVulkanPipeline(SVulkanPipeline& another) : layout(another.layout), pipeline(another.pipeline) {}

        constexpr SVulkanPipeline(SVulkanPipeline&& another) : layout(std::move(another.layout)), pipeline(std::move(another.pipeline)) {
            another.layout = vk::PipelineLayout();
            another.pipeline = vk::Pipeline();
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
using VulkanImageList = std::vector<vk::Image>;
using VulkanImageViewList = std::vector<vk::ImageView>;
using VulkanFramebufferList = std::vector<vk::Framebuffer>;
using VulkanCommandBufferList = std::vector<vk::CommandBuffer>;
using VulkanFenceList = std::vector<vk::Fence>;
using VulkanSemaphoreList = std::vector<vk::Semaphore>;

struct VulkanContext
{

    vk::Instance instance;
    vk::SurfaceKHR surface;
    vk::PhysicalDevice physicalDevice;

    VulkanQueue graphics;

    vk::Device device;

    vk::SwapchainKHR swapchain;

    vk::Format swapchainFormat;

    VulkanImageList images;
    VulkanImageViewList imageViews;

    vk::RenderPass renderPass;

    uint32_t width;
    uint32_t height;

    VulkanFramebufferList framebuffers;

    vk::CommandPool commandPool;
    VulkanCommandBufferList commandBuffers;

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
    }

    return std::string();
}

NODISCARD VulkanContextResult createVulkanInstance(VulkanContext *context, InstanceExtensionList enabledExtensions)
{

    std::vector<const char *> enabledLayers = {
        "VK_LAYER_KHRONOS_validation"};

    vk::ApplicationInfo applicationInfo{
        .sType = vk::StructureType::eApplicationInfo,
        .pNext = nullptr,
        .pApplicationName = "Vulkan Application",
        .applicationVersion = VK_MAKE_VERSION(0, 0, 1),
        .pEngineName = "Vulkan",
        .engineVersion = VK_MAKE_VERSION(0, 0, 1),
        .apiVersion = VK_API_VERSION_1_2};

    vk::InstanceCreateInfo instanceCreateInfo{
        .sType = vk::StructureType::eInstanceCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = static_cast<uint32_t>(enabledLayers.size()),
        .ppEnabledLayerNames = enabledLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size()),
        .ppEnabledExtensionNames = enabledExtensions.data()};

    if (vk::Result result = vk::createInstance(&instanceCreateInfo, nullptr, &context->instance); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedCreateInstance;
    }

    return VulkanContextResult::Success;
}

NODISCARD InstanceExtensionList getRequiredInstanceExtensions()
{

    uint32_t count;
    SDL_Vulkan_GetInstanceExtensions(&count, nullptr);
    std::vector<const char *> extensions(count);
    SDL_Vulkan_GetInstanceExtensions(&count, extensions.data());

    return extensions;
}

NODISCARD VulkanContextResult createVulkanSurface(VulkanContext *context, SDL_Window *window)
{

    assert(context->instance);
    assert(window);

    VkSurfaceKHR surface;
    if (SDL_Vulkan_CreateSurface(window, context->instance, &surface) != SDL_TRUE)
    {
        return VulkanContextResult::FailedCreateSurface;
    }

    context->surface = vk::SurfaceKHR(surface);
    if (!context->surface)
    {
        return VulkanContextResult::FailedCreateSurface;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult choosePhysicalDevice(VulkanContext *context)
{
    std::vector<vk::PhysicalDevice> physicalDevices = context->instance.enumeratePhysicalDevices();

    vk::PhysicalDevice bestDevice;
    int bestScore = 0;

    for (const auto &device : physicalDevices)
    {

        vk::PhysicalDeviceProperties properties = device.getProperties();

        int score = 0;

        switch (properties.deviceType)
        {

        case vk::PhysicalDeviceType::eDiscreteGpu:
            score += 1000;
            break;

        case vk::PhysicalDeviceType::eIntegratedGpu:
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
    context->physicalDevice = bestDevice;

    return VulkanContextResult::Success;
}

NODISCARD uint32_t findGraphicsQueueFamily(std::vector<vk::QueueFamilyProperties> &queueFamilies)
{

    uint32_t queueFamilyIndex = 0;

    for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilies.size()); i++)
    {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            queueFamilyIndex = i;
            break;
        }
    }

    return queueFamilyIndex;
}

NODISCARD VulkanContextResult createDevice(VulkanContext *context, DeviceExtensionList extensions)
{

    vk::PhysicalDevice physicalDevice = context->physicalDevice;
    if (!physicalDevice)
        return FailedCreateDevice;

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    uint32_t familyIndex = findGraphicsQueueFamily(queueFamilies);

    float priorities[] = {1.0f};

    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{
        .sType = vk::StructureType::eDeviceQueueCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .queueFamilyIndex = familyIndex,
        .queueCount = 1,
        .pQueuePriorities = priorities};

    vk::PhysicalDeviceFeatures enabledFeatures = {};

    vk::DeviceCreateInfo createInfo{
        .sType = vk::StructureType::eDeviceCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &deviceQueueCreateInfo,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = &enabledFeatures};

    if (vk::Result result = physicalDevice.createDevice(&createInfo, nullptr, &context->device); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedCreateDevice;
    }

    context->graphics.familyIndex = familyIndex;
    context->graphics.queue = context->device.getQueue(familyIndex, 0);

    return VulkanContextResult::Success;
}

NODISCARD vk::PresentModeKHR choosePresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes)
{

    vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

    for (const auto &availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == vk::PresentModeKHR::eMailbox)
        {
            return availablePresentMode;
        }
        else if (availablePresentMode == vk::PresentModeKHR::eImmediate)
        {
            bestMode = availablePresentMode;
        }
    }

    return bestMode;
}

NODISCARD vk::Extent2D chooseExtent(const vk::SurfaceCapabilitiesKHR &capabilities)
{
    vk::Extent2D extent;

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

NODISCARD vk::SurfaceFormatKHR chooseSurfaceFormat(const vk::PhysicalDevice physicalDevice, const std::vector<vk::SurfaceFormatKHR> &availableFormats, const vk::ImageUsageFlags usage)
{

    uint32_t index = 0;
    for (uint32_t x = 0; x < availableFormats.size(); x++)
    {

        vk::ImageFormatProperties formatProperties;
        vk::Result result = physicalDevice.getImageFormatProperties(availableFormats[x].format, vk::ImageType::e2D, vk::ImageTiling::eOptimal, usage, {}, &formatProperties);

        if (result == vk::Result::eErrorFormatNotSupported)
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

NODISCARD VulkanContextResult createVulkanSwapchain(VulkanContext *context, vk::ImageUsageFlags usage)
{

    vk::SurfaceKHR surface = context->surface;

    auto availableSurfaceFormats = context->physicalDevice.getSurfaceFormatsKHR(surface);
    auto surfaceCapabilities = context->physicalDevice.getSurfaceCapabilitiesKHR(surface);
    auto availablePresentModes = context->physicalDevice.getSurfacePresentModesKHR(surface);

    vk::PresentModeKHR presentMode = choosePresentMode(availablePresentModes);
    vk::Extent2D extent = chooseExtent(surfaceCapabilities);
    vk::SurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(context->physicalDevice, availableSurfaceFormats, usage);
    vk::Format format = surfaceFormat.format;

    vk::SwapchainCreateInfoKHR createInfo{
        .sType = vk::StructureType::eSwapchainCreateInfoKHR,
        .pNext = nullptr,
        .flags = {},
        .surface = surface,
        .minImageCount = 3,
        .imageFormat = format,
        .imageColorSpace = surfaceFormat.colorSpace,
        .imageExtent = surfaceCapabilities.currentExtent,
        .imageArrayLayers = 1,
        .imageUsage = usage,
        .imageSharingMode = vk::SharingMode::eExclusive,
        .preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = vk::PresentModeKHR::eFifo,
        .oldSwapchain = nullptr};

    if (vk::Result result = context->device.createSwapchainKHR(&createInfo, nullptr, &context->swapchain); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedCreateSwapchain;
    }

    context->images = context->device.getSwapchainImagesKHR(context->swapchain);

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

    vk::ImageViewCreateInfo imageViewCreateInfo{
        .sType = vk::StructureType::eImageViewCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .viewType = vk::ImageViewType::e2D,
        .format = context->swapchainFormat,
        .components = {
            .r = vk::ComponentSwizzle::eR,
            .g = vk::ComponentSwizzle::eG,
            .b = vk::ComponentSwizzle::eB,
            .a = vk::ComponentSwizzle::eA},
        .subresourceRange{.aspectMask = vk::ImageAspectFlagBits::eColor, .baseMipLevel = 0, .levelCount = 1, .baseArrayLayer = 0, .layerCount = 1}};

    for (unsigned long x = 0; x < context->amountOfFrames; x++)
    {

        imageViewCreateInfo.setImage(context->images[x]);

        if (vk::Result result = context->device.createImageView(&imageViewCreateInfo, nullptr, &context->imageViews[x]); result != vk::Result::eSuccess)
        {
            return VulkanContextResult::FailedCreateSwapchainViewImages;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createRenderPass(VulkanContext *context)
{

    vk::AttachmentDescription attachmentDescription{

        .format = context->swapchainFormat,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR};

    vk::AttachmentReference attachmentReference{

        .attachment = 0,
        .layout = vk::ImageLayout::eAttachmentOptimal};

    vk::SubpassDescription subpassDescription{

        .flags = {},
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &attachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr};
    /*
        vk::SubpassDependency subpassDependency;
        subpassDependency.setSrcSubpass(VK_SUBPASS_EXTERNAL);
        subpassDependency.setDstSubpass(0);
        subpassDependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        subpassDependency.setSrcAccessMask(vk::AccessFlagBits::eNone);
        subpassDependency.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
        subpassDependency.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentWrite);
    */

    vk::RenderPassCreateInfo renderPassCreateInfo{

        .attachmentCount = 1,
        .pAttachments = &attachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 0,
        .pDependencies = nullptr};

    if (vk::Result result = context->device.createRenderPass(&renderPassCreateInfo, 0, &context->renderPass); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedCreateRenderPass;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createSwapchainFramebuffers(VulkanContext *context)
{

    if (context->imageViews.empty())
        return VulkanContextResult::FailedCreateSwapchainFramebuffer;

    context->framebuffers.resize(context->amountOfFrames);

    vk::FramebufferCreateInfo framebufferCreateInfo{

        .renderPass = context->renderPass,
        .attachmentCount = 1,
        .width = context->width,
        .height = context->height,
        .layers = 1};

    for (unsigned long x = 0; x < context->amountOfFrames; x++)
    {

        framebufferCreateInfo.setAttachments(context->imageViews[x]);

        if (vk::Result result = context->device.createFramebuffer(&framebufferCreateInfo, nullptr, &context->framebuffers[x]); result != vk::Result::eSuccess)
        {
            return VulkanContextResult::FailedCreateSwapchainFramebuffer;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createCommandPool(VulkanContext *context)
{

    if (!context->device)
        return FailedCreateCommandPool;

    vk::CommandPoolCreateInfo commandPoolCreateInfo{
        .sType = vk::StructureType::eCommandPoolCreateInfo,
        .pNext = nullptr,
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = context->graphics.familyIndex};

    if (vk::Result result = context->device.createCommandPool(&commandPoolCreateInfo, nullptr, &context->commandPool); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedCreateCommandPool;
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult allocateCommandBuffer(VulkanContext *context)
{

    context->commandBuffers.resize(context->amountOfFrames);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{
        .sType = vk::StructureType::eCommandBufferAllocateInfo,
        .pNext = nullptr,
        .commandPool = context->commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1};

    for (uint32_t x = 0; x < context->amountOfFrames; x++)
    {

        if (vk::Result result = context->device.allocateCommandBuffers(&commandBufferAllocateInfo, &context->commandBuffers[x]); result != vk::Result::eSuccess)
        {
            return VulkanContextResult::FailedAllocateCommandBuffer;
        }
    }

    return VulkanContextResult::Success;
}

NODISCARD VulkanContextResult createFence(VulkanContext *context)
{

    context->fences.resize(context->amountOfFrames);

    vk::FenceCreateInfo fenceCreateInfo{
        .sType = vk::StructureType::eFenceCreateInfo,
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled};

    for (uint32_t x = 0; x < context->amountOfFrames; x++)
    {

        if (vk::Result result = context->device.createFence(&fenceCreateInfo, nullptr, &context->fences[x]); result != vk::Result::eSuccess)
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

    vk::SemaphoreCreateInfo semaphoreCreateInfo{
        .sType = vk::StructureType::eSemaphoreCreateInfo,
        .pNext = nullptr,
        .flags = {}};

    for (vk::Semaphore &semaphore : context->signalSemaphore)
    {

        if (vk::Result result = context->device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphore); result != vk::Result::eSuccess)
        {
            return VulkanContextResult::FailedCreateSemaphore;
        }
    }

    for (vk::Semaphore &semaphore : context->waitSemaphore)
    {

        if (vk::Result result = context->device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphore); result != vk::Result::eSuccess)
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

    InstanceExtensionList extensions;
    DeviceExtensionList deviceExtensions{VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    InstanceExtensionList requiredExtensions = getRequiredInstanceExtensions();
    extensions.insert(extensions.end(), requiredExtensions.begin(), requiredExtensions.end());

    if (createVulkanInstance(context, extensions) == VulkanContextResult::FailedCreateInstance)
    {
        return VulkanContextResult::FailedCreateInstance;
    }

    if (choosePhysicalDevice(context) == VulkanContextResult::NoSuitablePhysicalDevice)
    {
        return VulkanContextResult::NoSuitablePhysicalDevice;
    }

    if (createDevice(context, deviceExtensions) == VulkanContextResult::FailedCreateDevice)
    {
        return VulkanContextResult::FailedCreateDevice;
    }

    if (createVulkanSurface(context, window) == VulkanContextResult::FailedCreateSurface)
    {
        return VulkanContextResult::FailedCreateSurface;
    }

    if (createVulkanSwapchain(context, vk::ImageUsageFlagBits::eColorAttachment) == VulkanContextResult::FailedCreateSwapchain)
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
    if (createCommandPool(context) == VulkanContextResult::FailedCreateCommandPool)
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

NODISCARD VulkanContextResult render(VulkanContext *context, vk::Buffer buffer, SVulkanPipeline pipeline)
{

    uint32_t imageIndex;

    if (vk::Result result = context->device.waitForFences(1, &context->fences[context->frameIndex], VK_TRUE, UINT64_MAX); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedVulkanRendering;
    }

    if (vk::Result result = context->device.resetFences(1, &context->fences[context->frameIndex]); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedVulkanRendering;
    }

    if (vk::Result result = context->device.acquireNextImageKHR(context->swapchain, UINT64_MAX, context->waitSemaphore[context->frameIndex], {}, &imageIndex); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedVulkanRendering;
    }

    /* Commandbuffer recording */

    context->commandBuffers[imageIndex].reset();

    vk::CommandBufferBeginInfo commandBufferBeginInfo{
        .sType = vk::StructureType::eCommandBufferBeginInfo,
        .pNext = nullptr,
        .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit};

    vk::ClearValue clearValue;
    clearValue.setColor(std::array<float, 4>{0.1f, 0.1f, 0.1f, 1.0f});

    vk::RenderPassBeginInfo renderPassBeginInfo{

        .renderPass = context->renderPass,
        .framebuffer = context->framebuffers[imageIndex],

        .renderArea = {

            .offset{
                .x = 0,
                .y = 0},

            .extent{
                .width = context->width,
                .height = context->height}},

        .clearValueCount = 1,
        .pClearValues = &clearValue};

    context->commandBuffers[imageIndex].begin(commandBufferBeginInfo);
    context->commandBuffers[imageIndex].beginRenderPass(renderPassBeginInfo, vk::SubpassContents::eInline);

    context->commandBuffers[imageIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline.pipeline);
    
    vk::DeviceSize size = 0;
    context->commandBuffers[imageIndex].bindVertexBuffers(0, 1, &buffer, &size);
    
    context->commandBuffers[imageIndex].draw(3, 1, 0, 0);

    context->commandBuffers[imageIndex].endRenderPass();
    context->commandBuffers[imageIndex].end();

    vk::PipelineStageFlags pipelineStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

    /* Command Buffer submit */

    vk::SubmitInfo submitInfo{

        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &context->waitSemaphore[context->frameIndex],
        .pWaitDstStageMask = &pipelineStage,
        .commandBufferCount = 1,
        .pCommandBuffers = &context->commandBuffers[context->frameIndex],
        .signalSemaphoreCount = 1,
        .pSignalSemaphores = &context->signalSemaphore[context->frameIndex]};

    if (vk::Result result = context->graphics.queue.submit(1, &submitInfo, context->fences[context->frameIndex]); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedVulkanRendering;
    }

    vk::PresentInfoKHR presentInfoKHR{

        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &context->signalSemaphore[context->frameIndex],
        .swapchainCount = 1,
        .pSwapchains = &context->swapchain,
        .pImageIndices = &imageIndex};

    if (vk::Result result = context->graphics.queue.presentKHR(presentInfoKHR); result != vk::Result::eSuccess)
    {
        return VulkanContextResult::FailedVulkanRendering;
    }

    context->frameIndex = (context->frameIndex + 1) % context->amountOfFrames;

    return VulkanContextResult::Success;
}

void terminateVulkan(VulkanContext *context)
{
    vk::Instance instance = context->instance;
    vk::Device device = context->device;

    if (device)
    {

        for (vk::Semaphore &semaphore : context->waitSemaphore)
        {
            context->device.destroySemaphore(semaphore);
        }

        for (vk::Semaphore &semaphore : context->signalSemaphore)
        {
            context->device.destroySemaphore(semaphore);
        }

        for (vk::Fence &fence : context->fences)
        {
            device.destroyFence(fence);
        }

        device.destroyCommandPool(context->commandPool);

        for (vk::Framebuffer framebuffer : context->framebuffers)
        {
            device.destroyFramebuffer(framebuffer);
        }

        device.destroyRenderPass(context->renderPass);

        for (vk::ImageView view : context->imageViews)
        {
            device.destroyImageView(view);
        }

        device.destroy();
    }

    if (instance && context->surface)
    {
        instance.destroySurfaceKHR(context->surface);
    }

    if (instance)
    {
        instance.destroy();
    }
}

std::optional<uint32_t> findMemoryType(VulkanContext *context, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{

    vk::PhysicalDeviceMemoryProperties memoryProperties = context->physicalDevice.getMemoryProperties();

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

std::optional<VulkanBuffer> createBuffer(VulkanContext *context, vk::BufferUsageFlagBits usage, std::vector<float> vertices)
{

    vk::Buffer buffer;

    vk::BufferCreateInfo bufferCreateInfo{
        .sType = vk::StructureType::eBufferCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .size = sizeof(vertices[0]) * vertices.size(),
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr};

    if (vk::Result result = context->device.createBuffer(&bufferCreateInfo, nullptr, &buffer); result != vk::Result::eSuccess)
    {
        return std::nullopt;
    }

    vk::MemoryRequirements memoryRequirements;
    context->device.getBufferMemoryRequirements(buffer, &memoryRequirements);

    std::optional<uint32_t> memoryTypeIndex = findMemoryType(context, memoryRequirements.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    if (!memoryTypeIndex.has_value())
    {
        return std::nullopt;
    }

    vk::MemoryAllocateInfo memoryAllocateInfo{
        .pNext = nullptr,
        .allocationSize = memoryRequirements.size,
        .memoryTypeIndex = memoryTypeIndex.value()};

    vk::DeviceMemory memory;
    if (vk::Result result = context->device.allocateMemory(&memoryAllocateInfo, nullptr, &memory); result != vk::Result::eSuccess)
    {
        return std::nullopt;
    }

    context->device.bindBufferMemory(buffer, memory, 0);

    void* map = context->device.mapMemory(memory, 0, bufferCreateInfo.size);
    memcpy(map, vertices.data(), static_cast<size_t>(bufferCreateInfo.size));
    context->device.unmapMemory(memory);

    return std::optional<VulkanBuffer>({buffer, memory, usage});
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

NODISCARD std::optional<vk::ShaderModule> createShaderModule(VulkanContext *context, std::vector<uint8_t> &code)
{

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo{
        .sType = vk::StructureType::eShaderModuleCreateInfo,
        .pNext = nullptr,
        .flags = {},
        .codeSize = static_cast<uint32_t>(code.size()),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };

    vk::ShaderModule shaderModule;
    if (vk::Result result = context->device.createShaderModule(&shaderModuleCreateInfo, nullptr, &shaderModule); result != vk::Result::eSuccess)
    {
        return std::nullopt;
    }

    return std::make_optional<vk::ShaderModule>(shaderModule);
}

NODISCARD static std::pair<SVulkanPipeline, VulkanPipelineResult> createPipeline(VulkanContext *context, std::string vertexShaderFile, std::string fragmentShaderFile)
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

    std::array<vk::PipelineShaderStageCreateInfo, 2> shaderStages;

		shaderStages[0].setStage(vk::ShaderStageFlagBits::eVertex);
		shaderStages[0].setModule(vertexShaderModule.value());
		shaderStages[0].setPName("main");

		shaderStages[1].setStage(vk::ShaderStageFlagBits::eFragment);
		shaderStages[1].setModule(fragmentShaderModule.value());
		shaderStages[1].setPName("main");

		std::array<vk::VertexInputAttributeDescription, 2> vertexInputAttributeDescriptions;
		vertexInputAttributeDescriptions[0].setBinding(0);
        vertexInputAttributeDescriptions[0].setLocation(0);
        vertexInputAttributeDescriptions[0].setFormat(vk::Format::eR32G32Sfloat);
        vertexInputAttributeDescriptions[0].setOffset(0);

		vertexInputAttributeDescriptions[1].setBinding(0);
        vertexInputAttributeDescriptions[1].setLocation(1);
        vertexInputAttributeDescriptions[1].setFormat(vk::Format::eR32G32B32Sfloat);
        vertexInputAttributeDescriptions[1].setOffset(sizeof(float) * 2);

		std::array<vk::VertexInputBindingDescription, 1> vertexInputBindingDescriptions;
		vertexInputBindingDescriptions[0].setBinding(0);
        vertexInputBindingDescriptions[0].setStride(sizeof(float) * 5);
        vertexInputBindingDescriptions[0].setInputRate(vk::VertexInputRate::eVertex);

		vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
		vertexInputStateCreateInfo.setVertexAttributeDescriptions(vertexInputAttributeDescriptions);
		vertexInputStateCreateInfo.setVertexBindingDescriptions(vertexInputBindingDescriptions);

		vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState;
		inputAssemblyState.setTopology(vk::PrimitiveTopology::eTriangleList);

		std::array<vk::Viewport, 1> viewports;
		viewports[0].setWidth(static_cast<float>(context->width));
		viewports[0].setHeight(static_cast<float>(context->height));

		std::array<vk::Rect2D, 1> scissors;
		scissors[0].extent.setWidth(context->width);
		scissors[0].extent.setHeight(context->height);

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.setViewports(viewports);
		viewportState.setScissors(scissors);

		vk::PipelineRasterizationStateCreateInfo rasterizationState;
		rasterizationState.setLineWidth(1.0f);
		rasterizationState.setFrontFace(vk::FrontFace::eCounterClockwise);
		rasterizationState.setCullMode(vk::CullModeFlagBits::eFront);

		vk::PipelineMultisampleStateCreateInfo multisampleState;
		multisampleState.setRasterizationSamples(vk::SampleCountFlagBits::e1);

		std::array<vk::PipelineColorBlendAttachmentState, 1> colorBlendAttachments;
		colorBlendAttachments[0].setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
		colorBlendAttachments[0].setAlphaBlendOp(vk::BlendOp::eAdd);
		colorBlendAttachments[0].setBlendEnable(VK_FALSE);

		vk::PipelineColorBlendStateCreateInfo colorBlendState;
		colorBlendState.setAttachments(colorBlendAttachments);

		vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};

        pipeline.layout = context->device.createPipelineLayout(pipelineLayoutCreateInfo);

		if(!pipeline.layout) {
            return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedCreatePipelineLayout);
        }

		vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

		graphicsPipelineCreateInfo.setStages(shaderStages);
		graphicsPipelineCreateInfo.setPVertexInputState(&vertexInputStateCreateInfo);
		graphicsPipelineCreateInfo.setPInputAssemblyState(&inputAssemblyState);
		graphicsPipelineCreateInfo.setPViewportState(&viewportState);
		graphicsPipelineCreateInfo.setPRasterizationState(&rasterizationState);
		graphicsPipelineCreateInfo.setPMultisampleState(&multisampleState);
		graphicsPipelineCreateInfo.setPColorBlendState(&colorBlendState);
		graphicsPipelineCreateInfo.setLayout(pipeline.layout);
		graphicsPipelineCreateInfo.setRenderPass(context->renderPass);
		graphicsPipelineCreateInfo.setSubpass(0);

		if(vk::Result result = context->device.createGraphicsPipelines({}, 1, &graphicsPipelineCreateInfo, nullptr, &pipeline.pipeline); result != vk::Result::eSuccess) {
            return std::make_pair<SVulkanPipeline, VulkanPipelineResult>({}, VulkanPipelineResult::FailedCreatePipeline);
		} 

    context->device.destroyShaderModule(vertexShaderModule.value(), nullptr);
    context->device.destroyShaderModule(fragmentShaderModule.value(), nullptr);

    return std::make_pair<SVulkanPipeline, VulkanPipelineResult>(std::move(pipeline), VulkanPipelineResult::Succes);
}

void destroyPipeline(VulkanContext* context, SVulkanPipeline& pipeline) {

    context->device.destroyPipelineLayout(pipeline.layout, nullptr);
    context->device.destroyPipeline(pipeline.pipeline, nullptr);
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

    context->device.destroyBuffer(buffer.value().buffer);
    context->device.freeMemory(buffer.value().memory);
}

bool handleMessage()
{

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_EVENT_QUIT:
            return false;
        default:
            break;
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

    Window window = Window("Hello Vulkan", 1280, 720);

    VulkanContext context;
    if (VulkanContextResult result = initVulkan(window, &context); result != VulkanContextResult::Success)
    {
        terminateVulkan(&context);
        std::cout << vulkanErrorToString(result) << std::endl;
        return EXIT_FAILURE;
    }

    const std::vector<float> vertices {
        0.0, -0.5, 1.0, 0.0, 0.0,
        0.5, 0.5, 0.0, 1.0, 0.0,
        -0.5, 0.5, 0.0, 0.0, 1.0
    };

    std::optional<VulkanBuffer> buffer;
    if (buffer = createBuffer(&context, vk::BufferUsageFlagBits::eVertexBuffer, vertices); !buffer.has_value())
    {
        return EXIT_FAILURE;
    }

    std::pair<SVulkanPipeline, VulkanPipelineResult> pipeline = createPipeline(&context, "../shader/VertexShader.spv", "../shader/FragmentShader.spv");
    if (pipeline.second != VulkanPipelineResult::Succes) {
        std::cout << pipelineErrorToString(pipeline.second) << std::endl;
        destroyBuffer(&context, buffer);
        terminateVulkan(&context);
        return EXIT_FAILURE;
    }

    window.show();

    while (handleMessage())
    {

        if (render(&context, buffer.value().buffer, pipeline.first) != VulkanContextResult::Success)
        {
            terminateVulkan(&context);
            return EXIT_FAILURE;
        }
    }

    window.hide();
    
    vkDeviceWaitIdle(context.device);

    destroyPipeline(&context, pipeline.first);

    destroyBuffer(&context, buffer);

    terminateVulkan(&context);

    SDL_Quit();

    return EXIT_SUCCESS;
}
