#pragma once

#include <vulkan/vulkan.h>

namespace rp::gfx {
    VkShaderModule CreateShaderModule(VkDevice device, const std::vector<char>& code);

    struct ShaderSet {
        VkShaderModule vertShader = VK_NULL_HANDLE;
        VkShaderModule fragShader = VK_NULL_HANDLE;

        std::vector<VkPipelineShaderStageCreateInfo> GenerateStageCreateInfos() const;
    };
}