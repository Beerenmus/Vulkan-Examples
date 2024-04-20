#include "ImageView.hpp"

vks::ImageView::ImageView() : m_imageView(VK_NULL_HANDLE) {

}

vks::ImageView::~ImageView() {
    vkDestroyImageView(m_device->getHandle(), m_imageView, nullptr);
}

void vks::ImageView::compile(Device::Pointer device, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlagBits aspectMask) {

    if(m_imageView != VK_NULL_HANDLE) return;

    m_device = device;

    VkImageViewCreateInfo imageViewCreateInfo;
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = nullptr;
    imageViewCreateInfo.flags = {};
    imageViewCreateInfo.image = image;
    imageViewCreateInfo.viewType = viewType;
    imageViewCreateInfo.format = format;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
    imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    if(VkResult result = vkCreateImageView(m_device->getHandle(), &imageViewCreateInfo, nullptr, &m_imageView); result != VK_SUCCESS) {
        throw std::runtime_error("Error: ImageView::compile() Failed to create ImageView");
    }
}

VkImageView& vks::ImageView::getHandle() {
    return m_imageView;
}

const VkImageView& vks::ImageView::getHandle() const {
    return m_imageView;
}

vks::ImageView::Pointer vks::createImageView(vks::Device::Pointer device, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlagBits aspectMask) {
    auto imageView = vks::ImageView::create();
    imageView->compile(device, image, viewType, format, aspectMask);
    return imageView;
}