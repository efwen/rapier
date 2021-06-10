#include "graphics/vulkan_device.hpp"
#include "graphics/swapchain.hpp"

namespace rp::gfx {
    bool verifyPhysicalDeviceExtensionSupport(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredDeviceExtensions) {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());

        auto isExtensionSupported = [&availableDeviceExtensions](const char* requiredDeviceExtension) {
            for (const auto& availableDeviceExtension : availableDeviceExtensions) {
                if (strcmp(requiredDeviceExtension, availableDeviceExtension.extensionName) == 0)
                    return true;
            }
            return false;
        };

        return std::ranges::all_of(requiredDeviceExtensions, isExtensionSupported);
    }

    void VulkanDevice::Create(  VkInstance instance, 
                                VkSurfaceKHR surface,
                                const std::vector<const char*>& enabledLayers,
                                const std::vector<const char*>& enabledDeviceExtensions) {
        PickPhysicalDevice(instance, surface, enabledDeviceExtensions);

        QueueFamilyIndices queueFamilyConfig = FindQueueFamilies(physicalDevice, surface);
        CreateLogicalDevice(queueFamilyConfig, enabledLayers, enabledDeviceExtensions);
        CreateCommandPool(queueFamilyConfig);
    }

    void VulkanDevice::Cleanup() {
        vkDestroyCommandPool(handle, commandPool, nullptr);
        vkDestroyDevice(handle, nullptr);
    }



    //Select a Physical Device in the user's computer to render to
    //For the time being, this is the first device that supports the required extensions
    void VulkanDevice::PickPhysicalDevice(VkInstance instance, VkSurfaceKHR surface, const std::vector<const char*>& requiredDeviceExtensions) {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr);
        if (physicalDeviceCount == 0) throw std::runtime_error("Unable to find Vulkan-supported GPU!");

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data());

        auto isPhysicalDeviceSuitable = [surface, requiredDeviceExtensions](VkPhysicalDevice physicalDevice) {
            QueueFamilyIndices indices = FindQueueFamilies(physicalDevice, surface);
            bool deviceExtensionsSupported = verifyPhysicalDeviceExtensionSupport(physicalDevice, requiredDeviceExtensions);
            bool swapchainSupport = false;

            if (deviceExtensionsSupported) {
                SwapchainSupportDetails details = Swapchain::GetSupportDetails(physicalDevice, surface);
                swapchainSupport = !details.formats.empty() && !details.presentModes.empty();

            }
            return indices.isValid() && deviceExtensionsSupported && swapchainSupport;
        };

        auto firstSuitableDeviceIt = std::ranges::find_if(physicalDevices, isPhysicalDeviceSuitable);

        if (firstSuitableDeviceIt == std::end(physicalDevices))
            throw std::runtime_error("Failed to find suitable physical device!");

        physicalDevice = *firstSuitableDeviceIt;
    }

    void VulkanDevice::CreateLogicalDevice(QueueFamilyIndices queueFamilyIndices, const std::vector<const char*>& enabledLayers, const std::vector<const char*>& enabledDeviceExtensions) {
        auto fillQueueCreateInfo = [](VkDeviceQueueCreateInfo& createInfo, uint32_t familyIndex, uint32_t queueCount, const std::vector<float>& priorities) {
            createInfo = {
                VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                nullptr,
                0,
                familyIndex,
                queueCount,
                priorities.data()
            };
        };

        //create 1 graphics queue and 1 present queue when creating the device
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

        //graphicsQueue
        std::vector<float> queuePriorities = { 1.0f };
        VkDeviceQueueCreateInfo queueCreateInfo;
        fillQueueCreateInfo(queueCreateInfo, queueFamilyIndices.graphicsFamily.value(), 1, queuePriorities);
        queueCreateInfos.push_back(queueCreateInfo);
        //presentQueue
        if (queueFamilyIndices.presentFamily.value() != queueFamilyIndices.graphicsFamily.value()) {
            fillQueueCreateInfo(queueCreateInfo, queueFamilyIndices.presentFamily.value(), 1, queuePriorities);
            queueCreateInfos.push_back(queueCreateInfo);
        }



        //The physical device features we want to be able to use
        VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &physicalDeviceFeatures;

        createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
        createInfo.ppEnabledLayerNames = enabledLayers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = enabledDeviceExtensions.data();

        if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &handle) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(handle, queueFamilyIndices.graphicsFamily.value(), 0, &graphicsQueue);
        vkGetDeviceQueue(handle, queueFamilyIndices.presentFamily.value(), 0, &presentQueue);
    }

    void VulkanDevice::CreateCommandPool(QueueFamilyIndices queueFamilyIndices) {
        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        createInfo.flags = 0;

        if (vkCreateCommandPool(handle, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }
}
