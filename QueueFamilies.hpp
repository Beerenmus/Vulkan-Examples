#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

struct QueueFamily {

    VkQueueFlags queueFlags;
    uint32_t queueCount;

    QueueFamily() : queueFlags(VK_QUEUE_FLAG_BITS_MAX_ENUM), queueCount(0) {}
    QueueFamily(VkQueueFlags flags, uint32_t count) : queueFlags(flags), queueCount(count) {}
};

class QueueFamilies {

    private:
        std::vector<QueueFamily> m_queueFamilies;
        
    public:
        QueueFamilies(VkPhysicalDevice physicalDevice);
        const uint32_t getQueueGraphicsFamily() const;
        const uint32_t getQueueTransferFamily() const;
};