#include "Visitor.hpp"

#include "PhysicalDevice.hpp"
#include "CommandBuffer.hpp"
#include "Framebuffer.hpp"
#include "RenderPass.hpp"
#include "CommandPool.hpp"

void Visitor::apply(BaseItem& baseItem) {}

void Visitor::apply(PhysicalDevice& physicalDevice) {
    apply(static_cast<BaseItem&>(physicalDevice));
}

void Visitor::apply(CommandBuffer& commandBuffer) {
    apply(static_cast<BaseItem&>(commandBuffer));
}

void Visitor::apply(Framebuffer& framebuffer) {
    apply(static_cast<BaseItem&>(framebuffer));
}

void Visitor::apply(RenderPass& renderPass) {
    apply(static_cast<BaseItem&>(renderPass));
}

void Visitor::apply(CommandPool& commandPool) {
    apply(static_cast<BaseItem&>(commandPool));
}
