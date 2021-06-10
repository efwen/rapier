#pragma once

#include <vulkan/vulkan.hpp>

#include "graphics/vulkan_queue.hpp"

namespace rp::gfx {
    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities{};
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct Swapchain {
        VkSwapchainKHR handle = VK_NULL_HANDLE;
        VkFormat imageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D extent = {0, 0};
        uint32_t imageCount = 0;
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        std::vector<VkFramebuffer> frameBuffers;
    public:
        void Create(VkDevice device,
                    VkSurfaceKHR surface,
                    SwapchainSupportDetails swapchainSupportDetails,
                    QueueFamilyIndices familyIndices,
                    uint32_t width, uint32_t height);
        void Cleanup(VkDevice device);
        void CreateFrameBuffers(VkDevice device, VkRenderPass renderPass);
        static SwapchainSupportDetails GetSupportDetails(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    private:
        void CreateImageViews(VkDevice device);
        VkSurfaceFormatKHR ChooseSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) const;
        VkPresentModeKHR ChoosePresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) const;
        VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, uint32_t windowWidth, uint32_t windowHeight) const;
    };
}