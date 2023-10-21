#pragma once

#include "command.hpp"

class CmdBindVertexBuffers final : public Command {

    private:
        uint32_t m_first_binding;
        VkBuffer m_buffer;

    public:
        CmdBindVertexBuffers(uint32_t first_binding, VkBuffer buffer) : m_first_binding(first_binding), m_buffer(buffer) {}
        void record(VkCommandBuffer commandbuffer);
};

std::shared_ptr<Command> make_cmd_bind_vertex_buffers(uint32_t first_binding, VkBuffer buffer) {
    return std::make_shared<CmdBindVertexBuffers>(first_binding, buffer);
}

void CmdBindVertexBuffers::record(VkCommandBuffer commandbuffer) {
    VkDeviceSize offset = 0;
    vkCmdBindVertexBuffers(commandbuffer, m_first_binding, 1, &m_buffer, &offset);
}