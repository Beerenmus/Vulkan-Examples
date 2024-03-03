#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "CommandBuffer.hpp"

class Command {

    public:
        virtual void record(CommandBuffer::Pointer commandbuffer) = 0;
};

using CommandList = std::vector<std::shared_ptr<Command>>;