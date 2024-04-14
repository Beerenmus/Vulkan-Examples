#pragma once

#include <vulkan/vulkan.h>

#include "BaseItem.hpp"
#include "Inherit.hpp"

#include <vulkan/vulkan.h>
#include <vector>

class QueueSetting : public Inherit<QueueSetting, BaseItem> {

    private:
        uint32_t m_queueFamilyIndex;
        std::vector<float> m_queuePriorities;

    public:
        QueueSetting(uint32_t queueFamilyIndex, std::vector<float> queuePriorities = { 1.0f });

        uint32_t getQueueFamilyIndex() const;
        std::vector<float> getQueuePriorities() const;
};

using QueueSettings = std::vector<QueueSetting::Pointer>;