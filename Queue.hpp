#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include "BaseItem.hpp"
#include "Inherit.hpp"

namespace vks {

    class Queue : public Inherit<Queue, BaseItem> {

        public:
            using SubmitInfos = std::vector<VkSubmitInfo>;
            using PresentInfos = std::vector<VkPresentInfoKHR>;

        private:
            uint32_t m_queueFamilyIndex; 
            uint32_t m_queueIndex;
            VkQueue m_queue;

        public:
            Queue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue queue);
            
        public:
            void submit(const VkSubmitInfo& info, VkFence fence = VK_NULL_HANDLE);
            void submit(const SubmitInfos& infos, VkFence fence = VK_NULL_HANDLE);
            void wait();
            VkResult preset(const VkPresentInfoKHR &presentInfo);

        public:
            [[nodiscard]] uint32_t getFamilyIndex() const;
            [[nodiscard]] uint32_t getIndex() const;
    };

    using Queues = std::vector<Queue::Pointer>;
};