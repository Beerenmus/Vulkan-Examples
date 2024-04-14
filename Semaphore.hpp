#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

#include "Device.hpp"
#include "BaseItem.hpp"
#include "Inherit.hpp"

class Semaphore : public Inherit<Semaphore, BaseItem> {

    private:
        VkSemaphore m_semaphore;

    public:
        Semaphore(vks::Device::Pointer device);
        [[nodiscard]] VkSemaphore getHandle() const;
};

using Semaphores = std::vector<Semaphore>;