#pragma once
#include "pch.hpp"

#pragma warning(push)
#pragma warning( disable : 26812 ) //Disable unscoped enum warning (VkError, etc.)
#include <vulkan/vulkan.h>

namespace rp::gfx {

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    void VerifyLayerSupport(const std::vector<const char*>& requiredLayers) {
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        log::rp_info("Verifying Layers:");
        for (const auto& required : requiredLayers) {
            log::rp_info("\t{}", required);
            if (std::ranges::none_of(availableLayers, [&required](const auto& props) { return !strcmp(required, props.layerName); }))
                throw std::runtime_error(fmt::format("Vulkan Layer \"{}\" not found!", required));
        }
    }

    PFN_vkCreateDebugUtilsMessengerEXT pfnVkCreateDebugUtilsMessengerEXT = VK_NULL_HANDLE;
    PFN_vkDestroyDebugUtilsMessengerEXT pfnVkDestroyDebugUtilsMessengerEXT = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugUtilsMessenger = VK_NULL_HANDLE;

    VkResult CreateDebugUtilsMessengerEXT(  VkInstance                                  instance,
                                            const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
                                            const VkAllocationCallbacks*                pAllocator,
                                            VkDebugUtilsMessengerEXT*                   pMessenger) {
        return pfnVkCreateDebugUtilsMessengerEXT(instance, pCreateInfo, pAllocator, pMessenger);
    }

    void DestroyDebugUtilsMessengerEXT( VkInstance                      instance,
                                        VkDebugUtilsMessengerEXT        messenger,
                                        const VkAllocationCallbacks*    pAllocator) {
        pfnVkDestroyDebugUtilsMessengerEXT(instance, messenger, pAllocator);
    }

    VkBool32 logDebugMessage(   VkDebugUtilsMessageSeverityFlagBitsEXT          messageSeverity,
                                VkDebugUtilsMessageTypeFlagBitsEXT              messageType,
                                VkDebugUtilsMessengerCallbackDataEXT const *    pCallbackData,
                                void*                                           pUserData) {

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
            log::rp_error("DebugCallback: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            log::rp_warn("DebugCallback: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
            log::rp_info("DebugCallback: {}", pCallbackData->pMessage);
        }
        else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
            log::rp_trace("DebugCallback: {}", pCallbackData->pMessage);
        }

        return VK_FALSE;
    }

    void setupDebugCallback(VkInstance instance) {
        //Dynamically load the addresses for the functions that create and destroy
        //the debug utility messenger. This needs to be done because the messenger
        //functionality is contained within an extension, and not the core library.
        pfnVkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
        if (!pfnVkCreateDebugUtilsMessengerEXT) {
            log::rp_error("Failed to find pfnVkCreateDebugUtilsMessengerEXT function!");
            exit(1);
        }

        pfnVkDestroyDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (!pfnVkDestroyDebugUtilsMessengerEXT) {
            log::rp_error("Failed to find pfnVkDestroyDebugUtilsMessengerEXT function!");
            exit(1);
        }

        VkDebugUtilsMessageSeverityFlagsEXT messageSeverityFlags =  VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                                                                    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        VkDebugUtilsMessageTypeFlagsEXT messageTypeFlags =  VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = messageSeverityFlags;
        createInfo.messageType = messageTypeFlags;
        createInfo.pfnUserCallback = reinterpret_cast<PFN_vkDebugUtilsMessengerCallbackEXT>(logDebugMessage);

        VkResult result = CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugUtilsMessenger);
        if (result != VK_SUCCESS) {
            log::rp_error("Failed to create Debug Utils Messenger! VkResult = {}", result);
            exit(1);
        }
    }

    void cleanupDebugCallback(VkInstance instance) {
        DestroyDebugUtilsMessengerEXT(instance, debugUtilsMessenger, nullptr);
    }
}
#pragma warning(pop)