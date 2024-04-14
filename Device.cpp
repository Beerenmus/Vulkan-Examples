#include "Device.hpp"

vks::Device::Device(PhysicalDevice::Pointer physicalDevice, const QueueSettings& settings, Names names, VkPhysicalDeviceFeatures& enabledFeatures) : Inherit() {


    std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;

    float queuePriority = 1.0f;
    
    for(auto& setting : settings) {

        bool isUnique = true;
        for(auto& queueCreateInfo : deviceQueueCreateInfos) {
            if(setting->getQueueFamilyIndex() == static_cast<uint32_t>(queueCreateInfo.queueFamilyIndex)) {
                isUnique = false;
            }
        }

        if(!isUnique) continue;

        VkDeviceQueueCreateInfo deviceQueueCreateInfo;
        deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        deviceQueueCreateInfo.pNext = nullptr;
        deviceQueueCreateInfo.flags = 0;
        deviceQueueCreateInfo.queueFamilyIndex = setting->getQueueFamilyIndex();

        if(!setting->getQueuePriorities().empty()) {

            deviceQueueCreateInfo.queueCount = static_cast<uint32_t>(setting->getQueuePriorities().size());
            deviceQueueCreateInfo.pQueuePriorities = setting->getQueuePriorities().data();  

            uint32_t supportedQueueCount = physicalDevice->getQueueFamilyProperties()[setting->getQueueFamilyIndex()].queueCount;
            if(supportedQueueCount < deviceQueueCreateInfo.queueCount) {
                deviceQueueCreateInfo.queueCount = supportedQueueCount;
            }
        } 

        else {
            deviceQueueCreateInfo.queueCount = 1;
            deviceQueueCreateInfo.pQueuePriorities = &queuePriority;  
        }

        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    VkDeviceCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = {},
        .queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size()),
        .pQueueCreateInfos = deviceQueueCreateInfos.data(),
        .enabledExtensionCount = static_cast<uint32_t>(names.size()),
        .ppEnabledExtensionNames = names.data(),
        .pEnabledFeatures = &enabledFeatures
    };

    if (VkResult result = vkCreateDevice(physicalDevice->getHandle(), &createInfo, nullptr, &m_device); result != VK_SUCCESS) {
        throw std::runtime_error("Error: vks::Device::create(...) Failed to create device");
    }

    for(auto x = size_t { 0 }; x < deviceQueueCreateInfos.size(); x++) {

        for(auto index = size_t { 0 }; index < deviceQueueCreateInfos[x].queueCount; index++) {

            VkQueue queue;
            vkGetDeviceQueue(m_device, deviceQueueCreateInfos[x].queueFamilyIndex, index, &queue);

            m_queues.push_back(Queue::create(deviceQueueCreateInfos[x].queueFamilyIndex, index, queue));
        }
    }
}

template<typename T>
[[nodiscard]] bool vks::Device::getProcAddr(T& procAddress, std::string name) {

    T func = reinterpret_cast<T>(vkGetDeviceProcAddr(m_device, name.c_str()));

    if(!func) {
        return false;
    }

    return true;
}

[[nodiscard]] vks::Queue::Pointer vks::Device::getQueue(uint32_t familyIndex, uint32_t index) {
    
    auto iter = std::find_if(m_queues.begin(), m_queues.end(), [familyIndex, index](const Queue::Pointer& queue){
        return queue->getFamilyIndex() == familyIndex && queue->getIndex() == index;
    });

    if(iter == m_queues.end()) {
        throw std::runtime_error("Error: vks::Device::getQueue(...) not found queue");
    }

    return *iter;
}

[[nodiscard]] VkDevice vks::Device::getHandle() const {
    return m_device;
}