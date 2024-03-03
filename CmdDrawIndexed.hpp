#pragma once

#include "Command.hpp"
#include "Inherit.hpp"
#include "CommandBuffer.hpp"

class CmdDrawIndexed : public Inherit<CmdDrawIndexed, Command> {

    private:
        uint32_t m_indexCount; 
        uint32_t m_instanceCount; 
        uint32_t m_firstIndex;
        int32_t m_vertexOffset;
        uint32_t m_firstInstance;

    public:
        CmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) : m_indexCount(indexCount), m_instanceCount(instanceCount), m_firstIndex(firstIndex), m_vertexOffset(vertexOffset), m_firstInstance(firstInstance) {}
        void record(CommandBuffer::Pointer commandbuffer) override;
};

void CmdDrawIndexed::record(CommandBuffer::Pointer commandbuffer) {
    commandbuffer->drawIndexed(m_indexCount, m_instanceCount, m_firstIndex, m_vertexOffset, m_firstInstance);
}