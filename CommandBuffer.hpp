#pragma once

#include<vulkan/vulkan.h>
#include<memory>
#include<vector>

#include "Inherit.hpp"
#include "BaseItem.hpp"

class CommandBuffer : public Inherit<CommandBuffer, BaseItem> {

    private:
        VkCommandBuffer m_commandBuffer;

    public:
        CommandBuffer(VkCommandBuffer commandBuffer);
        
        void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
        void bindIndexBuffer(VkBuffer buffer, VkIndexType type);
        void bindVertexBuffers(uint32_t firstBinding, uint32_t bindingCount, const VkBuffer *pBuffers, const VkDeviceSize *pOffsets);
        void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);
        void bindDescriptorSets(VkPipelineBindPoint pipelineBindPoint, VkPipelineLayout layout, uint32_t firstSet, uint32_t descriptorSetCount, const VkDescriptorSet *pDescriptorSets, uint32_t dynamicOffsetCount, const uint32_t *pDynamicOffsets);
        void bindPipeline(VkPipelineBindPoint pipelineBindPoint, VkPipeline pipeline);

        virtual void visit(Visitor& visitor);

        VkCommandBuffer getHandle() const;
};

using CommandBuffers = std::vector<CommandBuffer::Pointer>;