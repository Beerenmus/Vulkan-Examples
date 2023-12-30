#pragma once

#include "Command.hpp"

class CmdBindIndexBuffer : public Command {

    private:
        VkBuffer m_buffer;
        VkIndexType m_type;

    public:
        CmdBindIndexBuffer(VkBuffer buffer, VkIndexType type = VK_INDEX_TYPE_UINT16) : m_buffer(buffer), m_type(type) {}
        void record(VkCommandBuffer commandbuffer);
};

void CmdBindIndexBuffer::record(VkCommandBuffer commandbuffer) {
    vkCmdBindIndexBuffer(commandbuffer, m_buffer, 0, m_type);
}