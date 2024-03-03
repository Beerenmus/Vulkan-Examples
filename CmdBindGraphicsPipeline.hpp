#pragma once

#include "Command.hpp"
#include "Inherit.hpp"

class CmdBindGraphicsPipeline final : public Inherit<CmdBindGraphicsPipeline, Command> {

    private:
        VkPipeline m_pipeline;

    public:
        CmdBindGraphicsPipeline(VkPipeline pipeline);
        void record(CommandBuffer::Pointer commandBuffer) override;
};

CmdBindGraphicsPipeline::CmdBindGraphicsPipeline(VkPipeline pipeline) : m_pipeline(pipeline) {}

void CmdBindGraphicsPipeline::record(CommandBuffer::Pointer commandBuffer) {
    commandBuffer->bindPipeline(VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
}