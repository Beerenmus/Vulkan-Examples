#include "Swapchain.hpp"

vks::Swapchain::Swapchain(vks::Device::Pointer device, PhysicalDevice::Pointer physicalDevice, VkSurfaceKHR surface, VkImageUsageFlags usage) {

    m_device = device->getHandle();

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice->getHandle(), surface, &surfaceCapabilities);

    PhysicalDevice::SurfacePresentModes availablePresentModes = physicalDevice->getSurfacePresentModes();
    PhysicalDevice::SurfaceFormats availableSurfaceFormats = physicalDevice->getSurfaceFormats();

    VkPresentModeKHR presentMode = choosePresentMode(availablePresentModes);
    VkExtent2D extent = chooseExtent(surfaceCapabilities);
    VkSurfaceFormatKHR surfaceFormat = chooseSurfaceFormat(physicalDevice->getHandle(), availableSurfaceFormats, usage);
    
    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = {};
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 3;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = surfaceCapabilities.currentExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = usage;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_FALSE;
    swapchainCreateInfo.oldSwapchain = nullptr;

    if (VkResult result = vkCreateSwapchainKHR(device->getHandle(), &swapchainCreateInfo, nullptr, &m_swapchain); result != VK_SUCCESS) {
        throw std::runtime_error("Error: Swapchain::create() Failed to create Swapchain");
    }

    uint32_t amountOfSwapchainImages;
    vkGetSwapchainImagesKHR(device->getHandle(), m_swapchain, &amountOfSwapchainImages, nullptr);
    m_images.resize(amountOfSwapchainImages);
    vkGetSwapchainImagesKHR(device->getHandle(), m_swapchain, &amountOfSwapchainImages, &m_images[0]);

    m_imageViews.resize(amountOfSwapchainImages);
    for(std::size_t x=0; x<m_imageViews.size(); x++) {
        m_imageViews[x] = createImageView(device, m_images[x], VK_IMAGE_VIEW_TYPE_2D, surfaceFormat.format, VK_IMAGE_ASPECT_COLOR_BIT);
    }

    m_format = surfaceFormat.format;
    m_extent = extent;
}

VkPresentModeKHR vks::Swapchain::choosePresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
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

VkExtent2D vks::Swapchain::chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities)
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

VkSurfaceFormatKHR vks::Swapchain::chooseSurfaceFormat(const VkPhysicalDevice physicalDevice, const std::vector<VkSurfaceFormatKHR> &availableFormats, const VkImageUsageFlags usage)
{
    uint32_t index = 0;
    for (uint32_t x = 0; x < availableFormats.size(); x++)
    {

        VkImageFormatProperties formatProperties;

        VkResult result = vkGetPhysicalDeviceImageFormatProperties(physicalDevice, availableFormats[x].format, VK_IMAGE_TYPE_2D, VK_IMAGE_TILING_OPTIMAL, usage, 0, &formatProperties);

        if (result == VK_ERROR_FORMAT_NOT_SUPPORTED)
        {
            throw std::runtime_error("Swapchain format does not support requested usage flags");
        }
        else
        {
            index = x;
            break;
        }
    }

    return availableFormats[index];
}

std::tuple<VkResult, uint32_t> vks::Swapchain::acquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence) {
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &imageIndex);
    return std::make_tuple(result, imageIndex);
}

VkSwapchainKHR vks::Swapchain::getHandle() const {
    return m_swapchain;
}

vks::Swapchain::Images& vks::Swapchain::getImages() {
    return m_images;
}

const vks::Swapchain::Images& vks::Swapchain::getImages() const {
    return m_images;
}

vks::ImageViews& vks::Swapchain::getImageViews() {
    return m_imageViews;
}

const vks::ImageViews& vks::Swapchain::getImageViews() const {
    return m_imageViews;
}

VkFormat vks::Swapchain::getFormat() const {
    return m_format;
}

uint32_t vks::Swapchain::getWidth() const {
    return m_extent.width; 
}

uint32_t vks::Swapchain::getHeight() const {
    return m_extent.height;
}

uint32_t vks::Swapchain::getNumImages() const {
    return m_images.size();
}

vks::Swapchain::~Swapchain() {

    for(auto& imageView : m_imageViews) {
        imageView.reset();
    }

    if(m_swapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
    }
}