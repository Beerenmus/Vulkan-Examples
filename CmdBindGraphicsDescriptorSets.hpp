#pragma once

#include "CmdBindDescriptorSets.hpp"

class CmdBindGraphicsDescriptorSets final : public CmdBindDescriptorSets {

    public:
        using CmdBindDescriptorSets::CmdBindDescriptorSets;

        VkPipelineBindPoint get_pipeline_bind_point() override;
};

VkPipelineBindPoint CmdBindGraphicsDescriptorSets::get_pipeline_bind_point() {
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}