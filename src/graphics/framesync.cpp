#include "graphics/framesync.hpp"
#include "pch.hpp"

namespace rp::gfx {
        void FrameSynchronizer::Create(VkDevice device, uint32_t swapchainSize) {
            imagesInFlight.resize(swapchainSize, VK_NULL_HANDLE);
            VkSemaphoreCreateInfo semaphoreCreateInfo{};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            
            VkFenceCreateInfo fenceCreateInfo{};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                if (vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                    vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                    vkCreateFence(device, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                    throw std::runtime_error("Failed to create semaphores!");
                }
            }
        }

        void FrameSynchronizer::Cleanup(VkDevice device) {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
                vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
                vkDestroyFence(device, inFlightFences[i], nullptr);
            }
        }

        VkSemaphore FrameSynchronizer::GetNextWaitSemaphore() const {
            return imageAvailableSemaphores[currentFrame];
        }

        VkSemaphore FrameSynchronizer::GetNextSignalSemaphore() const {
            return renderFinishedSemaphores[currentFrame];
        }

        VkFence FrameSynchronizer::GetNextFrameFence() const {
            return inFlightFences[currentFrame];
        }

        void FrameSynchronizer::WaitFrameFence(VkDevice device) {
            vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
        }

        void FrameSynchronizer::ResetFrameFence(VkDevice device) {
            vkResetFences(device, 1, &inFlightFences[currentFrame]);
        }


        void FrameSynchronizer::WaitImageFence(VkDevice device, uint32_t imageIndex) {
            if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
                vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
            }
            imagesInFlight[imageIndex] = inFlightFences[currentFrame];
        }

        void FrameSynchronizer::IncrementFrame() {
            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }
}