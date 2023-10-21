#pragma once

#include "command.hpp"

#include<vector>

class CmdBindDescriptorSets : public Command {

    private:
        VkPipelineLayout m_layout;
        uint32_t m_first_set;

        std::vector<VkDescriptorSet> m_descriptor_sets;
        std::vector<uint32_t> m_dynamic_offsets;

    public:
        CmdBindDescriptorSets(VkPipelineLayout const layout, uint32_t const first_set, std::vector<VkDescriptorSet> const descriptor_sets, std::vector<uint32_t> const dynamic_offsets = {}) noexcept : 
            m_layout(layout), m_first_set(first_set), m_descriptor_sets(std::move(descriptor_sets)), m_dynamic_offsets(std::move(dynamic_offsets)) {}

        virtual VkPipelineBindPoint get_pipeline_bind_point() = 0;
        void record(VkCommandBuffer commandbuffer);
};

void CmdBindDescriptorSets::record(VkCommandBuffer commandbuffer) {
    vkCmdBindDescriptorSets(commandbuffer, get_pipeline_bind_point(), m_layout, m_first_set, static_cast<uint32_t>(m_descriptor_sets.size()), m_descriptor_sets.data(), static_cast<uint32_t>(m_dynamic_offsets.size()), m_dynamic_offsets.data());
}