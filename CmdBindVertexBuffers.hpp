#pragma once

#include "Command.hpp"
#include "Inherit.hpp"
#include "CommandBuffer.hpp"

class CmdBindVertexBuffers final : public Inherit<CmdBindVertexBuffers, Command> {

    private:
        uint32_t m_firstBinding;
        VkBuffer m_buffer;

    public:
        CmdBindVertexBuffers(uint32_t firstBinding, VkBuffer buffer) : m_firstBinding(firstBinding), m_buffer(buffer) {}
        void record(CommandBuffer::Pointer commandbuffer);
};

void CmdBindVertexBuffers::record(CommandBuffer::Pointer commandbuffer) {
    VkDeviceSize offset = 0;
    commandbuffer->bindVertexBuffers(m_firstBinding, 1, &m_buffer, &offset);
}