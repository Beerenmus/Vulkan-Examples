#pragma once

#include "Command.hpp"

class CmdDrawIndexed : public Command {

    private:
        uint32_t m_indexCount; 
        uint32_t m_instanceCount; 
        uint32_t m_firstIndex;
        int32_t m_vertexOffset;
        uint32_t m_firstInstance;

    public:
        CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) : m_indexCount(indexCount), m_instanceCount(instanceCount), m_firstIndex(firstIndex), m_vertexOffset(vertexOffset), m_firstInstance(firstInstance) {}
        void record(VkCommandBuffer commandbuffer);
};

void CmdDrawIndexed::record(VkCommandBuffer commandbuffer) {
    vkCmdDrawIndexed(commandbuffer, m_indexCount, m_instanceCount, m_firstIndex, m_vertexOffset, m_firstInstance);
}