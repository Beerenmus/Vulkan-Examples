#pragma once

#include "Command.hpp"
#include "CommandBuffer.hpp"
#include "Inherit.hpp"

class CmdDraw : public Inherit<CmdDraw, Command> {

    private:
        uint32_t m_vertexCount;
        uint32_t m_instanceCount;
        uint32_t m_firstVertex;
        uint32_t m_firstInstance;

    public:
        CmdDraw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance) : m_vertexCount(vertexCount), m_instanceCount(instanceCount), m_firstVertex(firstInstance), m_firstInstance(firstInstance) {}
        void record(CommandBuffer::Pointer commandbuffer) override;
};

void CmdDraw::record(CommandBuffer::Pointer commandbuffer) {
    commandbuffer->draw(m_vertexCount, m_instanceCount, m_firstVertex, m_firstInstance);
}