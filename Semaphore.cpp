#include "Semaphore.hpp"

Semaphore::Semaphore(vks::Device::Pointer device) {

    VkSemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    if(VkResult result = vkCreateSemaphore(device->getHandle(), &semaphoreCreateInfo, nullptr, &m_semaphore); result != VK_SUCCESS) {
        throw std::runtime_error("Error: Semaphore::create() Failed to create semaphore");
    }
}

[[nodiscard]] VkSemaphore Semaphore::getHandle() const {
    return m_semaphore;
}