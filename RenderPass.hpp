#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include <stdexcept>


#include "Inherit.hpp"
#include "BaseItem.hpp"

class RenderPass : public Inherit<RenderPass, BaseItem>
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
        void visit(Visitor& visitor);
        [[nodiscard]] VkRenderPass getHandle() const;
        virtual ~RenderPass();
};

[[nodiscard]] auto createRenderPass(VkDevice device, VkFormat format) -> RenderPass::Pointer;