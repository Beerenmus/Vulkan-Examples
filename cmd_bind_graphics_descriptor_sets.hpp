#pragma once

#include "cmd_bind_descriptor_sets.hpp"

class CmdBindGraphicsDescriptorSets final : public CmdBindDescriptorSets {

    public:
        using CmdBindDescriptorSets::CmdBindDescriptorSets;

        VkPipelineBindPoint get_pipeline_bind_point() override;
};

std::shared_ptr<Command> make_cmd_bind_graphics_descriptor_sets(VkPipelineLayout const layout, uint32_t const first_set, std::vector<VkDescriptorSet> const descriptor_sets, std::vector<uint32_t> offsets = {}) {
    return std::make_shared<CmdBindGraphicsDescriptorSets>(layout, first_set, descriptor_sets, offsets);
}

VkPipelineBindPoint CmdBindGraphicsDescriptorSets::get_pipeline_bind_point() {
    return VK_PIPELINE_BIND_POINT_GRAPHICS;
}