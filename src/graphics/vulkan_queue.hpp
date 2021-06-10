#pragma once

#include <vulkan/vulkan.h>

namespace rp::gfx {
    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isValid() {
            return graphicsFamily.has_value() &&
                presentFamily.has_value();
        }
    };

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
}
