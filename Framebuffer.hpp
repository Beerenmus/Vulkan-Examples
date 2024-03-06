#pragma ocne

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include "RenderPass.hpp"
#include "Inherit.hpp"

class IFramebuffer {

};

class Framebuffer : public Inherit<Framebuffer, IFramebuffer> {

    private:
        VkDevice m_device;
        VkFramebuffer m_framebuffer;

    public:
        using Attachments = std::vector<VkImageView>;

    public:
        Framebuffer(VkDevice device, RenderPass::Pointer renderPass, Attachments &attachments, uint32_t width, uint32_t height);
        [[nodiscard]] VkFramebuffer getHandle() const;
        ~Framebuffer();
};

using Framebuffers = std::vector<Framebuffer::Pointer>;

Framebuffer::Framebuffer(VkDevice device, RenderPass::Pointer renderPass, Attachments &attachments, uint32_t width, uint32_t height) : m_device(device) {

    VkFramebufferCreateInfo framebufferCreateInfo;
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferCreateInfo.pNext = nullptr;
    framebufferCreateInfo.flags = 0;
    framebufferCreateInfo.renderPass = renderPass->getHandle();
    framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    framebufferCreateInfo.pAttachments = attachments.data();
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;

    if (VkResult result = vkCreateFramebuffer(m_device, &framebufferCreateInfo, nullptr, &m_framebuffer); result != VK_SUCCESS) {
        throw std::runtime_error("Error: Framebuffer::create(...) Failed to create framebuffer"); 
    }
}

[[nodiscard]] VkFramebuffer Framebuffer::getHandle() const {
    return m_framebuffer;
}

Framebuffer::~Framebuffer() {

    if(m_framebuffer) {
        vkDestroyFramebuffer(m_device, m_framebuffer, nullptr);
    }
}