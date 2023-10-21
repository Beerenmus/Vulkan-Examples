#pragma once

#include "command.hpp"

class CmdBindPipeline : public Command {

    private:
        VkPipeline m_pipeline;

    public:
        CmdBindPipeline(VkPipeline pipeline) : m_pipeline(pipeline) {}
        virtual VkPipelineBindPoint get_pipeline_bind_point() = 0;
        void record(VkCommandBuffer commandbuffer);
};

void CmdBindPipeline::record(VkCommandBuffer commandbuffer) {

    vkCmdBindPipeline(commandbuffer, get_pipeline_bind_point(), m_pipeline);
}