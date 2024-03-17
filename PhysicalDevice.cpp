#include "PhysicalDevice.hpp"

PhysicalDevice::PhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) : m_physicalDevice(physicalDevice), m_queueFamilies(physicalDevice), Inherit() {
    m_surfaceFormats = enumerateSurfaceFormats(surface);
    m_surfacePresentModes = enumerateSurfacePresentModes(surface);
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

void PhysicalDevice::visit(Visitor& visitor) {
    visitor.apply(*this);
}

const QueueFamilies& PhysicalDevice::getQueueFamilies() const {
    return m_queueFamilies;
}

const PhysicalDevice::SurfaceFormats& PhysicalDevice::getSurfaceFormats() const {
    return m_surfaceFormats;
}

const PhysicalDevice::SurfacePresentModes& PhysicalDevice::getSurfacePresentModes() const {
    return m_surfacePresentModes;
}

const VkPhysicalDevice PhysicalDevice::getHandle() const {
    return m_physicalDevice;
}