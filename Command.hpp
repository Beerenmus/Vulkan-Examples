#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>

class Command {

    public:
        virtual void record(VkCommandBuffer commandbuffer) = 0;
};

using CommandList = std::vector<std::shared_ptr<Command>>;