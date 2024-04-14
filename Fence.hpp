#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

#include "Device.hpp"
#include "BaseItem.hpp"
#include "Inherit.hpp"

class Fence : public Inherit<Fence, BaseItem> {

    private:
        VkFence m_fence;

    public:
        Fence(vks::Device::Pointer device);
        [[nodiscard]] VkFence getHandle() const;
};

using Fences = std::vector<Fence>;