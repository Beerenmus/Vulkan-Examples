#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>
#include <algorithm>
#include <vector>

#include "Inherit.hpp"
#include "BaseItem.hpp"
#include "PhysicalDevice.hpp"
#include "QueueSetting.hpp"
#include "Queue.hpp"

namespace vks {

    class Device : public Inherit<Device, BaseItem> {

        public:
            using Names = std::vector<const char*>;

        private:
            VkDevice m_device;

            Queues m_queues;

        public:
            Device(PhysicalDevice::Pointer physicalDevice, const QueueSettings& settings, Names names, VkPhysicalDeviceFeatures& enabledFeatures);
            
        public:
            template<typename T>
            [[nodiscard]] bool getProcAddr(T& procAddress, std::string name);
            
        public:
            [[nodiscard]] Queue::Pointer getQueue(uint32_t familyIndex, uint32_t index);
            [[nodiscard]] VkDevice getHandle() const;
    };
}