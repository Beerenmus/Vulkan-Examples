#include "Queue.hpp"

vks::Queue::Queue(uint32_t queueFamilyIndex, uint32_t queueIndex, VkQueue queue) : m_queueFamilyIndex { queueFamilyIndex }, m_queueIndex { queueIndex }, m_queue { queue } {}

void vks::Queue::submit(const SubmitInfos& infos, VkFence fence) {
    
    if(VkResult result = vkQueueSubmit(m_queue, static_cast<uint32_t>(infos.size()), infos.data(), fence ? fence : VK_NULL_HANDLE); result != VK_SUCCESS) {
        throw std::runtime_error("Error: vks::Queue::submit() Submit failed");
    }
}

void vks::Queue::submit(const VkSubmitInfo& submitInfo, VkFence fence) {
    
    if(VkResult result = vkQueueSubmit(m_queue, 1, &submitInfo, fence ? fence : VK_NULL_HANDLE); result != VK_SUCCESS) {
        throw std::runtime_error("Error: vks::Queue::submit() Submit failed");
    }
}

void vks::Queue::wait() {
    vkQueueWaitIdle(m_queue);
}

VkResult vks::Queue::preset(const VkPresentInfoKHR &presentInfo) {
    return vkQueuePresentKHR(m_queue, &presentInfo);
}

[[nodiscard]] uint32_t vks::Queue::getFamilyIndex() const {
    return m_queueFamilyIndex;
}

[[nodiscard]] uint32_t vks::Queue::getIndex() const {
    return m_queueIndex;
}