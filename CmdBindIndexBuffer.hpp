#pragma once

#include "Command.hpp"
#include "Inherit.hpp"

class CmdBindIndexBuffer : public Inherit<CmdBindIndexBuffer, Command> {

    private:
        VkBuffer m_buffer;
        VkIndexType m_type;

    public:
        CmdBindIndexBuffer(VkBuffer buffer, VkIndexType type = VK_INDEX_TYPE_UINT16) : m_buffer(buffer), m_type(type) {}
        void record(CommandBuffer::Pointer commandbuffer) override;
};

void CmdBindIndexBuffer::record(CommandBuffer::Pointer commandbuffer) {
    commandbuffer->bindIndexBuffer(m_buffer, m_type);
}