#pragma once

#include <vulkan/vulkan.h>

namespace rp::gfx {
    struct FrameSynchronizer {
        static const size_t MAX_FRAMES_IN_FLIGHT = 2;
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores;
        std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores;
        std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;
    public:
        void Create(VkDevice device, uint32_t swapchainSize);
        void Cleanup(VkDevice device);
        VkSemaphore GetNextWaitSemaphore() const;
        VkSemaphore GetNextSignalSemaphore() const;
        VkFence GetNextFrameFence() const;

        void WaitFrameFence(VkDevice device);
        void ResetFrameFence(VkDevice device);
        void WaitImageFence(VkDevice device, uint32_t imageIndex);
        void IncrementFrame();
    };
}