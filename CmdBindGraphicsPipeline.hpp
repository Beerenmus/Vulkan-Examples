#pragma once

#include "CmdBindPipeline.hpp"

class CmdBindGraphicsPipeline final : public CmdBindPipeline {

    public:
        using CmdBindPipeline::CmdBindPipeline;

        VkPipelineBindPoint get_pipeline_bind_point();
};

VkPipelineBindPoint CmdBindGraphicsPipeline::get_pipeline_bind_point() {
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}