#pragma ocne

#include <vulkan/vulkan.h>
#include <vector>
#include <stdexcept>

#include "RenderPass.hpp"
#include "Inherit.hpp"
#include "BaseItem.hpp"

class Framebuffer : public Inherit<Framebuffer, BaseItem> {

    private:
        VkDevice m_device;
        VkFramebuffer m_framebuffer;

    public:
        using Attachments = std::vector<VkImageView>;

    public:
        Framebuffer(VkDevice device, RenderPass::Pointer renderPass, Attachments &attachments, uint32_t width, uint32_t height);
        void visit(Visitor& visitor) override;
        [[nodiscard]] VkFramebuffer getHandle() const;
        ~Framebuffer();
};

using Framebuffers = std::vector<Framebuffer::Pointer>;