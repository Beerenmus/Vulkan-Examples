#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Inherit.hpp"
#include "CommandBuffer.hpp"

class ICommandPool {

};

class CommandPool : public Inherit<CommandPool, ICommandPool> {

    private:
        VkDevice m_device;
        VkCommandPool m_commandPool;

    public:
        CommandPool(VkDevice device, uint32_t familyIndex);
        [[nodiscard]] CommandBuffer::Pointer allocateCommandBuffer();
        void reset();
        virtual ~CommandPool();
};

using CommandPools = std::vector<CommandPool::Pointer>;

CommandPool::CommandPool(VkDevice device, uint32_t familyIndex) : m_device(device) {

    VkCommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    commandPoolCreateInfo.pNext = nullptr,
    commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    commandPoolCreateInfo.queueFamilyIndex = familyIndex;

    if (VkResult result = vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &m_commandPool); result != VK_SUCCESS) {
        throw std::runtime_error("Error: CommadPool::create(...) Failed to create CommandPool");
    }
}

[[nodiscard]] CommandBuffer::Pointer CommandPool::allocateCommandBuffer() {

    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.commandPool = m_commandPool;
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferAllocateInfo.commandBufferCount = 1;

    if (VkResult result = vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &commandBuffer); result != VK_SUCCESS) {
        throw std::runtime_error("Error: CommandPool::allocateCommandBuffer(...) Failed to allocate CommandBuffer");    
    }

    return CommandBuffer::create(commandBuffer);
}

void CommandPool::reset() {
    vkResetCommandPool(m_device, m_commandPool, 0);
}

CommandPool::~CommandPool() {
    
    if(m_commandPool) {
        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
    }
}