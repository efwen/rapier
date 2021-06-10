#pragma once

#include "graphics/vulkan_queue.hpp"

#include <vulkan/vulkan.h>
#include <vector>


namespace rp::gfx {
    struct VulkanDevice {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice handle = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
    public:
        void Create(VkInstance instance,
                    VkSurfaceKHR surface,
                    const std::vector<const char*>& validationLayers,
                    const std::vector<const char*>& deviceExtensions);

        void Cleanup();
    private:
        void PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensions);
        void CreateLogicalDevice(QueueFamilyIndices queueFamilyConfig, const std::vector<const char*>& validationLayers, const std::vector<const char*>& deviceExtensions);
        void CreateCommandPool(QueueFamilyIndices queueFamilyConfig);
    };
}