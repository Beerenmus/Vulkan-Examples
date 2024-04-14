#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include <optional>

#include "BaseItem.hpp"
#include "Inherit.hpp"
#include "Visitor.hpp"

class PhysicalDevice : public Inherit<PhysicalDevice, BaseItem> {

    public:
        using Features = VkPhysicalDeviceFeatures;
        using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
        using SurfacePresentModes = std::vector<VkPresentModeKHR>;
        using QueueFamilyProperties = std::vector<VkQueueFamilyProperties>;

    private:
        VkPhysicalDevice m_physicalDevice;

        SurfaceFormats m_surfaceFormats;
        SurfacePresentModes m_surfacePresentModes;
        QueueFamilyProperties m_queueFamilyProperties;

    private:
        SurfaceFormats enumerateSurfaceFormats(VkSurfaceKHR surface);    
        SurfacePresentModes enumerateSurfacePresentModes(VkSurfaceKHR);
        QueueFamilyProperties enumerateQueueFamilyProperties();

    public:
        PhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        virtual ~PhysicalDevice() {}

    public:
        const std::optional<uint32_t> getQueueGraphicsFamily() const;
        const std::optional<uint32_t> getQueueTransferFamily() const;
    
    public:
        virtual void visit(Visitor& visitor) override;
        
    public:
        const SurfaceFormats& getSurfaceFormats() const;
        const SurfacePresentModes& getSurfacePresentModes() const;
        const QueueFamilyProperties& getQueueFamilyProperties() const;
        const VkPhysicalDevice getHandle() const;
};