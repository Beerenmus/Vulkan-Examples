#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "BaseItem.hpp"
#include "Inherit.hpp"
#include "Visitor.hpp"
#include "QueueFamilies.hpp"

class PhysicalDevice : public Inherit<PhysicalDevice, BaseItem> {

    public:
        using SurfaceFormats = std::vector<VkSurfaceFormatKHR>;
        using SurfacePresentModes = std::vector<VkPresentModeKHR>;

    private:
        VkPhysicalDevice m_physicalDevice;

        QueueFamilies m_queueFamilies;
        SurfaceFormats m_surfaceFormats;
        SurfacePresentModes m_surfacePresentModes;

    private:
        QueueFamilies enumerateQueueFamilies();
        SurfaceFormats enumerateSurfaceFormats(VkSurfaceKHR surface);    
        SurfacePresentModes enumerateSurfacePresentModes(VkSurfaceKHR);

    public:
        PhysicalDevice(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
        virtual ~PhysicalDevice() {}
        
    public:
        virtual void visit(Visitor& visitor) override;
        
    public:
        const QueueFamilies& getQueueFamilies() const;
        const SurfaceFormats& getSurfaceFormats() const;
        const SurfacePresentModes& getSurfacePresentModes() const;

        const VkPhysicalDevice getHandle() const;
};