#include "QueueSetting.hpp"

QueueSetting::QueueSetting(uint32_t queueFamilyIndex, std::vector<float> queuePriorities) : 
    m_queueFamilyIndex(queueFamilyIndex), m_queuePriorities { std::move(queuePriorities) }, Inherit() {}

uint32_t QueueSetting::getQueueFamilyIndex() const {
    return m_queueFamilyIndex;
}
std::vector<float> QueueSetting::getQueuePriorities() const {
    return m_queuePriorities;
}