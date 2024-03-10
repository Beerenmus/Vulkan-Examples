#pragma once

class PhysicalDevice;
class BaseItem;
class CommandBuffer;
class Framebuffer;
class RenderPass;
class CommandPool;

class Visitor {

    private:

    public:
        virtual void apply(BaseItem& baseItem);
        virtual void apply(PhysicalDevice& physicalDevice);
        virtual void apply(CommandBuffer& commandBuffer);
        virtual void apply(Framebuffer& framebuffer);
        virtual void apply(RenderPass& renderPass);
        virtual void apply(CommandPool& commandPool);
};