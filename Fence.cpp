#include "Fence.hpp"

Fence::Fence(vks::Device::Pointer device) {

    VkFenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    fenceCreateInfo.pNext = nullptr;
    fenceCreateInfo.flags = 0;

    if(VkResult result = vkCreateFence(device->getHandle(), &fenceCreateInfo, nullptr, &m_fence); result != VK_SUCCESS) {
        throw std::runtime_error("Error: Fence::create() Failed to create fence");
    }
}

[[nodiscard]] VkFence Fence::getHandle() const {
    return m_fence;
}