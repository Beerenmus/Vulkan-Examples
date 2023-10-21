#pragma once

#include "cmd_bind_pipeline.hpp"

class CmdBindGraphicsPipeline final : public CmdBindPipeline {

    public:
        using CmdBindPipeline::CmdBindPipeline;

        VkPipelineBindPoint get_pipeline_bind_point();
};

std::shared_ptr<CmdBindGraphicsPipeline> make_cmd_bind_graphics_pipeline(VkPipeline pipeline) {
    return std::make_shared<CmdBindGraphicsPipeline>(pipeline);
}

VkPipelineBindPoint CmdBindGraphicsPipeline::get_pipeline_bind_point() {
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}