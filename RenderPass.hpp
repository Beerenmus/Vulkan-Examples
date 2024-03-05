#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <stdexcept>


#include "Inherit.hpp"

class IRenderPass {

    public:
        virtual VkRenderPass getHandle() const = 0;
};

class RenderPass : public Inherit<RenderPass, IRenderPass>
{
    public:
        using Descriptions = std::vector<VkAttachmentDescription>;
        using Attachments = std::vector<VkAttachmentReference>;
        using Subpasses = std::vector<VkSubpassDescription>; 
        using Dependencys = std::vector<VkSubpassDependency>;

    private:
        VkDevice m_device;
        VkRenderPass m_renderPass;

    public:
        RenderPass(VkDevice device, Descriptions &descriptions, Attachments &attachments, Subpasses &subpasses, Dependencys &dependencys);

        [[nodiscard]] operator bool() const;

        [[nodiscard]] VkRenderPass getHandle() const override;

        virtual ~RenderPass();
};

RenderPass::RenderPass(VkDevice device, Descriptions &descriptions, Attachments &attachments, Subpasses &subpasses, Dependencys &dependencys) : m_device(device) {
    
    VkRenderPassCreateInfo renderPassCreateInfo;
    renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassCreateInfo.pNext = nullptr;
    renderPassCreateInfo.flags = 0;
    renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(descriptions.size());
    renderPassCreateInfo.pAttachments = descriptions.data();
    renderPassCreateInfo.subpassCount = static_cast<uint32_t>(subpasses.size());
    renderPassCreateInfo.pSubpasses = subpasses.data();
    renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(dependencys.size());
    renderPassCreateInfo.pDependencies = dependencys.data();

    if (VkResult result = vkCreateRenderPass(device, &renderPassCreateInfo, nullptr, &m_renderPass); result != VK_SUCCESS) {
        throw std::runtime_error("Error: Renderpass::create(...) Failed to create Renderpass" );
    }
}

[[nodiscard]] RenderPass::operator bool() const {
    return m_renderPass != VK_NULL_HANDLE;
}

[[nodiscard]] VkRenderPass RenderPass::getHandle() const {
    return m_renderPass;
}

RenderPass::~RenderPass() {
    
    if(m_renderPass) {
        vkDestroyRenderPass(m_device, m_renderPass, nullptr);
    }
}

[[nodiscard]] static auto createRenderPass(VkDevice device, VkFormat format) -> RenderPass::Pointer {

    VkAttachmentDescription attachmentDescription;
    attachmentDescription.format = format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    RenderPass::Descriptions descriptions { attachmentDescription };

    VkAttachmentReference attachment;
    attachment.attachment = 0;
    attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    RenderPass::Attachments attachments { attachment };

    VkSubpassDescription subpass;
    subpass.flags = 0;        
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.inputAttachmentCount = 0;
    subpass.pInputAttachments = nullptr;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &attachment;
    subpass.pResolveAttachments = nullptr;
    subpass.pDepthStencilAttachment = nullptr;
    subpass.preserveAttachmentCount = 0;
    subpass.pPreserveAttachments = nullptr;;

    RenderPass::Subpasses subpasses { subpass };

    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    
    RenderPass::Dependencys dependencys { dependency };

    return RenderPass::create(device, descriptions, attachments, subpasses, dependencys);
}