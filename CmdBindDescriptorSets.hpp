#pragma once

#include "Command.hpp"
#include "CommandBuffer.hpp"
#include "Inherit.hpp"

#include<vector>

class CmdBindDescriptorSets final : public Inherit<CmdBindDescriptorSets, Command> {

    private:
        VkPipelineLayout m_layout;
        uint32_t m_firstSet;

        std::vector<VkDescriptorSet> m_descriptorSets;
        std::vector<uint32_t> m_dynamicOffsets;

    public:
        CmdBindDescriptorSets(VkPipelineLayout const layout, uint32_t const firstSet, std::vector<VkDescriptorSet> const descriptorSets, std::vector<uint32_t> const dynamicOffsets = {}) noexcept : 
            m_layout(layout), m_firstSet(firstSet), m_descriptorSets(std::move(descriptorSets)), m_dynamicOffsets(std::move(dynamicOffsets)) {}

        void record(CommandBuffer::Pointer commandBuffer) override;
};

void CmdBindDescriptorSets::record(CommandBuffer::Pointer commandBuffer) {
    commandBuffer->bindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS, m_layout, m_firstSet, static_cast<uint32_t>(m_descriptorSets.size()), m_descriptorSets.data(), static_cast<uint32_t>(m_dynamicOffsets.size()), m_dynamicOffsets.data());
}