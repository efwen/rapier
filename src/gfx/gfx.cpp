#include "pch.hpp"
#include "gfx/gfx.hpp"
#include "util/version.hpp"

#include <vulkan/vulkan.h>

namespace rp::gfx {
    VkInstance instance = VK_NULL_HANDLE;
    void init() {
        log::rp_info("Initializing Renderer");

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello World";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "Rapier";
        Version engineVersion = getVersion();
        appInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledExtensionCount = 0;
        createInfo.ppEnabledExtensionNames = nullptr;
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = nullptr;
        

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan instance!");
        }

        log::rp_info("Created Vulkan Instance!");
    }

    void shutdown() {
        log::rp_info("Shutting Down Renderer");
        vkDestroyInstance(instance, nullptr);
        instance = VK_NULL_HANDLE;
    }

    void draw() {
    }
}