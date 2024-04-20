#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "Inherit.hpp"
#include "BaseItem.hpp"

#include "Device.hpp"

namespace vks {

    class ImageView : public Inherit<ImageView, BaseItem> {

        private:
            VkImageView m_imageView;
            Device::Pointer m_device;

        public:
            ImageView();
            ImageView(ImageView&) = delete;
            ImageView(ImageView&&) = delete;
            ~ImageView();

        public:
            void compile(Device::Pointer device, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlagBits aspectMask);

        public:
            VkImageView& getHandle();
            const VkImageView& getHandle() const;
    };

    using ImageViews = std::vector<ImageView::Pointer>;

    ImageView::Pointer createImageView(vks::Device::Pointer device, VkImage image, VkImageViewType viewType, VkFormat format, VkImageAspectFlagBits aspectMask);
}
