#include "QueueFamilies.hpp"

QueueFamilies::QueueFamilies(VkPhysicalDevice physicalDevice) {

    uint32_t amountOfDeviceQueueFamilyProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfDeviceQueueFamilyProperties, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(amountOfDeviceQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &amountOfDeviceQueueFamilyProperties, &queueFamilies[0]);

    m_queueFamilies.resize(amountOfDeviceQueueFamilyProperties);

    for(uint32_t x=0; x<m_queueFamilies.size(); x++) {
        m_queueFamilies[x].queueFlags = queueFamilies[x].queueFlags; 
        m_queueFamilies[x].queueCount = queueFamilies[x].queueCount;
    }
}

const uint32_t QueueFamilies::getQueueGraphicsFamily() const {

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilies.size()); x++)
    {
        if (m_queueFamilies[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            return x;
        }
    }

    throw std::runtime_error("Error: QueueFamilies::getQueueGraphicsFamily() no queue with graphics support found");
}

const uint32_t QueueFamilies::getQueueTransferFamily() const {

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilies.size()); x++)
    {
        if (m_queueFamilies[x].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            return x;
        }
    }

    throw std::runtime_error("Error: QueueFamilies::getQueueGraphicsFamily() no queue with transfer support found");
}