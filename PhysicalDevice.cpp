#include "PhysicalDevice.hpp"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) : m_physicalDevice(physicalDevice), Inherit() {

    m_surfaceFormats = enumerateSurfaceFormats(surface);
    m_surfacePresentModes = enumerateSurfacePresentModes(surface);
    m_queueFamilyProperties = enumerateQueueFamilyProperties();
}

PhysicalDevice::SurfaceFormats PhysicalDevice::enumerateSurfaceFormats(VkSurfaceKHR surface) {

    uint32_t amountOfSurfaceFormats;
    if(VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &amountOfSurfaceFormats, nullptr); result != VK_SUCCESS) {
        throw std::runtime_error("Error: PhysicalDevice::getSurfaceFormats(...) surface formats could not be found");
    }

    SurfaceFormats availableSurfaceFormats(amountOfSurfaceFormats);
    if(VkResult result = vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, surface, &amountOfSurfaceFormats, &availableSurfaceFormats[0]); result != VK_SUCCESS) {
        throw std::runtime_error("Error: PhysicalDevice::getSurfaceFormats(...) surface formats could not be found");
    }

    return availableSurfaceFormats;
}

PhysicalDevice::SurfacePresentModes PhysicalDevice::enumerateSurfacePresentModes(VkSurfaceKHR surface) {

    uint32_t amountOfSurfacePresenModes;
    if(VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &amountOfSurfacePresenModes, nullptr); result != VK_SUCCESS) {
        throw std::runtime_error("Error: PhysicalDevice::getSurfacePresentModes(...) present modes could not be found");
    }
    
    SurfacePresentModes availablePresentModes(amountOfSurfacePresenModes);
    if(VkResult result = vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, surface, &amountOfSurfacePresenModes, &availablePresentModes[0]); result != VK_SUCCESS) {
        throw std::runtime_error("Error: PhysicalDevice::getSurfacePresentModes(...) present modes could not be found");
    }

    return availablePresentModes;
}

PhysicalDevice::QueueFamilyProperties PhysicalDevice::enumerateQueueFamilyProperties() {
    
    uint32_t amountOfDeviceQueueFamilyProperties;
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &amountOfDeviceQueueFamilyProperties, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(amountOfDeviceQueueFamilyProperties);
    vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &amountOfDeviceQueueFamilyProperties, &queueFamilyProperties[0]);

    return queueFamilyProperties;
}

const std::optional<uint32_t> PhysicalDevice::getQueueGraphicsFamily() const {

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++) {
        if ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) != VK_QUEUE_COMPUTE_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) != VK_QUEUE_TRANSFER_BIT) {
            return x;
        }
    }

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++) {
        if ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) != VK_QUEUE_TRANSFER_BIT) {
            return x;
        }
    }

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++) {
        if ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT &&
            (m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) {
            return x;
        }
    }

    return std::nullopt;
}

const std::optional<uint32_t> PhysicalDevice::getQueueTransferFamily() const {

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++)
    {
        if (((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) && 
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_QUEUE_GRAPHICS_BIT) &&
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) != VK_QUEUE_COMPUTE_BIT)) {
            return x;
        }
    }

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++)
    {
        if (((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) && 
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) != VK_QUEUE_GRAPHICS_BIT) &&
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)) {
            return x;
        }
    }

    for (uint32_t x = 0; x < static_cast<uint32_t>(m_queueFamilyProperties.size()); x++)
    {
        if (((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_TRANSFER_BIT) == VK_QUEUE_TRANSFER_BIT) && 
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) &&
            ((m_queueFamilyProperties[x].queueFlags & VK_QUEUE_COMPUTE_BIT) == VK_QUEUE_COMPUTE_BIT)) {
            return x;
        }
    }

    return std::nullopt;
}

void PhysicalDevice::visit(Visitor& visitor) {
    visitor.apply(*this);
}

const PhysicalDevice::SurfaceFormats& PhysicalDevice::getSurfaceFormats() const {
    return m_surfaceFormats;
}

const PhysicalDevice::SurfacePresentModes& PhysicalDevice::getSurfacePresentModes() const {
    return m_surfacePresentModes;
}

const PhysicalDevice::QueueFamilyProperties& PhysicalDevice::getQueueFamilyProperties() const {
    return m_queueFamilyProperties;
}

const VkPhysicalDevice PhysicalDevice::getHandle() const {
    return m_physicalDevice;
}