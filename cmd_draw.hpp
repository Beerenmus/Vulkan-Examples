#pragma once

#include "command.hpp"

class CmdDraw : public Command {

    private:
        uint32_t m_vertex_count;
        uint32_t m_instance_count;
        uint32_t m_first_vertex;
        uint32_t m_first_instance;

    public:
        CmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) : m_vertex_count(vertex_count), m_instance_count(instance_count), m_first_vertex(first_instance), m_first_instance(first_instance) {}
        void record(VkCommandBuffer commandbuffer);
};

std::shared_ptr<Command> make_cmd_draw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    return std::make_shared<CmdDraw>(vertex_count, instance_count, first_vertex, first_instance);
}

void CmdDraw::record(VkCommandBuffer commandbuffer) {
    vkCmdDraw(commandbuffer, m_vertex_count, m_instance_count, m_first_vertex, m_first_instance);
}