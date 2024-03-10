#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Inherit.hpp"
#include "CommandBuffer.hpp"
#include "BaseItem.hpp"
#include "Visitor.hpp"

class CommandPool : public Inherit<CommandPool, BaseItem> {

    private:
        VkDevice m_device;
        VkCommandPool m_commandPool;

    public:
        CommandPool(VkDevice device, uint32_t familyIndex);
        [[nodiscard]] CommandBuffer::Pointer allocateCommandBuffer();
        void visit(Visitor& visitor) override;
        void reset();
        virtual ~CommandPool();
};

using CommandPools = std::vector<CommandPool::Pointer>;