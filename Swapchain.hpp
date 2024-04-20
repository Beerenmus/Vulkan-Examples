#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>
#include <tuple>

#include "BaseItem.hpp"
#include "Inherit.hpp"
#include "PhysicalDevice.hpp"
#include "Device.hpp"
#include "Semaphore.hpp"
#include "ImageView.hpp"

namespace vks {

    class Swapchain : public Inherit<Swapchain, BaseItem> {

        public:
            using Images = std::vector<VkImage>;

        private:
            VkSwapchainKHR m_swapchain;

            Images m_images;
            ImageViews m_imageViews;
            VkFormat m_format;
            VkExtent2D m_extent;
            VkDevice m_device;

        private:
            VkPresentModeKHR choosePresentMode(const PhysicalDevice::SurfacePresentModes &availablePresentModes);
            VkExtent2D chooseExtent(const VkSurfaceCapabilitiesKHR &capabilities);
            VkSurfaceFormatKHR chooseSurfaceFormat(const VkPhysicalDevice physicalDevice, const PhysicalDevice::SurfaceFormats &availableFormats, const VkImageUsageFlags usage);

        public:
            Swapchain(Device::Pointer device, PhysicalDevice::Pointer physicalDevice, VkSurfaceKHR surface, VkImageUsageFlags usage);
            virtual ~Swapchain();

        public:
            std::tuple<VkResult, uint32_t> acquireNextImage(uint64_t timeout, VkSemaphore semaphore, VkFence fence = VK_NULL_HANDLE);

        public:
            Images& getImages();
            const Images& getImages() const;

            ImageViews& getImageViews();
            const ImageViews& getImageViews() const;

            VkFormat getFormat() const;
            uint32_t getWidth() const;
            uint32_t getHeight() const;
            uint32_t getNumImages() const;
            VkSwapchainKHR getHandle() const;
    };
}