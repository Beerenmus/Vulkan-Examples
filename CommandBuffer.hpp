#pragma once

#include<vector>
#include<memory>
#include<utility>

#include "Command.hpp"
#include "CmdBindVertexBuffers.hpp"
#include "CmdBindGraphicsDescriptorSets.hpp"
#include "CmdBindGraphicsPipeline.hpp"
#include "CmdDraw.hpp"
#include "CmdBindIndexBuffer.hpp"
#include "CmdDrawIndexed.hpp"

class CommandBuffer {

    private:
        using CommandList = std::vector<std::unique_ptr<Command>>;

        CommandList m_commands;

    public:
        CommandBuffer();
        CommandBuffer(CommandBuffer&) = delete;
        CommandBuffer(CommandBuffer&& commandBuffer);

    public:
        CommandBuffer& operator = (CommandBuffer&) = delete;
        CommandBuffer& operator = (CommandBuffer&& commandBuffer);
        
    public:
        void addBindGraphicsDescriptorSets(VkPipelineLayout const layout, uint32_t const first_set, std::vector<VkDescriptorSet> const descriptor_sets, std::vector<uint32_t> const dynamic_offsets = {});
        void addBindGraphicsPipeline(VkPipeline pipeline);
        void addBindVertexBuffers(uint32_t first_binding, VkBuffer buffer);
        void addBindIndexBuffer(VkBuffer buffer, VkIndexType type = VK_INDEX_TYPE_UINT16);
        void addCmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance);
        void addCmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

    public:
        void record(VkCommandBuffer commandBuffer);
};

CommandBuffer::CommandBuffer() {
    m_commands.clear();
}

CommandBuffer::CommandBuffer(CommandBuffer&& commandbuffer) : m_commands(std::exchange(commandbuffer.m_commands, {})) {}

CommandBuffer& CommandBuffer::operator = (CommandBuffer&& commandBuffer) {
    m_commands = std::exchange(commandBuffer.m_commands, {});
    return *this;
}

void CommandBuffer::addBindGraphicsDescriptorSets(VkPipelineLayout const layout, uint32_t const first_set, std::vector<VkDescriptorSet> const descriptor_sets, std::vector<uint32_t> const dynamic_offsets) {
    m_commands.push_back(std::make_unique<CmdBindGraphicsDescriptorSets>(layout, first_set, descriptor_sets, dynamic_offsets));
}

void CommandBuffer::addBindGraphicsPipeline(VkPipeline pipeline) {
    m_commands.push_back(std::make_unique<CmdBindGraphicsPipeline>(pipeline));
}

void CommandBuffer::addBindVertexBuffers(uint32_t first_binding, VkBuffer buffer) {
    m_commands.push_back(std::make_unique<CmdBindVertexBuffers>(first_binding, buffer));    
}

void CommandBuffer::addBindIndexBuffer(VkBuffer buffer, VkIndexType type) {
    m_commands.push_back(std::make_unique<CmdBindIndexBuffer>(buffer, type));
}

void CommandBuffer::addCmdDraw(uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance) {
    m_commands.push_back(std::make_unique<CmdDraw>(vertex_count, instance_count, first_vertex, first_instance));
}

void CommandBuffer::addCmdDrawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) {
    m_commands.push_back(std::make_unique<CmdDrawIndexed>(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance));
}

void CommandBuffer::record(VkCommandBuffer const commandBuffer) {

    for(auto &cmd : m_commands) {
        cmd->record(commandBuffer);
    }
}