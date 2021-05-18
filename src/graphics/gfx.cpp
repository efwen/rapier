#include "pch.hpp"
#include "graphics/gfx.hpp"
#include "graphics/validation.hpp"
#include "util/version.hpp"
#include "util/file_io.hpp"
#include "core/window.hpp"

#include <vulkan/vulkan_win32.h>

namespace rp::gfx {

    struct VulkanContext {
        VkInstance instance = VK_NULL_HANDLE;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device = VK_NULL_HANDLE;
        VkQueue graphicsQueue = VK_NULL_HANDLE;
        VkQueue presentQueue = VK_NULL_HANDLE;
        VkSurfaceKHR surface = VK_NULL_HANDLE;
        VkCommandPool commandPool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
    };

    struct Swapchain {
        VkSwapchainKHR handle = VK_NULL_HANDLE;
        VkFormat imageFormat = VK_FORMAT_UNDEFINED;
        VkExtent2D extent = {0, 0};
        std::vector<VkImage> images;
        std::vector<VkImageView> imageViews;
        std::vector<VkFramebuffer> frameBuffers;
    };

    struct SwapchainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isValid() {
            return graphicsFamily.has_value() &&
                   presentFamily.has_value();
        }
    };

#ifdef NDEBUG
    const bool validationLayersEnabled = false;
#else
    const bool validationLayersEnabled = true;
#endif

    std::vector<const char*> requiredExtensions = {
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_win32_surface"
    };

    std::vector<const char*> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    constexpr VkSurfaceFormatKHR g_preferredSurfaceFormat = {
        VK_FORMAT_B8G8R8A8_SRGB,
        VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
    };

    VulkanContext context;
    Swapchain swapchain;
    Window* pWindow = nullptr;

    //Pipeline and render pass for rendering the triangle
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;

    //Frame Synchronization objects
    const size_t MAX_FRAMES_IN_FLIGHT = 2;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> imageAvailableSemaphores;
    std::array<VkSemaphore, MAX_FRAMES_IN_FLIGHT> renderFinishedSemaphores;
    std::array<VkFence, MAX_FRAMES_IN_FLIGHT> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;


    void createInstance(const std::string& appName, util::Version appVersion, const std::string& engineName, util::Version engineVersion) {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        if (validationLayersEnabled) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();

        if (vkCreateInstance(&createInfo, nullptr, &context.instance) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create vulkan instance!");
        }
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice dev) {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(dev, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& props : queueFamilies) {
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily = i;

            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(dev, i, context.surface, &presentSupport);
            if (presentSupport) indices.presentFamily = i;

            i++;
        }

        return indices;
    }

    void verifyRequiredExtensions() {
        uint32_t availableExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

        log::rp_info("Verifying Extensions:");
        for (const auto& required : requiredExtensions) {
            log::rp_info("\t{}", required);
            if (std::ranges::none_of(availableExtensions, [&required](const auto& props) { return !strcmp(required, props.extensionName); }))
                throw std::runtime_error(fmt::format("Vulkan Extension \"{}\" not found!", required));
        }
    }

    void verifyRequiredLayers() {
        uint32_t availableLayerCount = 0;
        vkEnumerateInstanceLayerProperties(&availableLayerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(availableLayerCount);
        vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

        log::rp_info("Verifying Layers:");
        for (const auto& required : validationLayers) {
            log::rp_info("\t{}", required);
            if (std::ranges::none_of(availableLayers, [&required](const auto& props) { return !strcmp(required, props.layerName); }))
                throw std::runtime_error(fmt::format("Vulkan Layer \"{}\" not found!", required));
        }
    }

    bool verifyDeviceExtensionSupport(VkPhysicalDevice testPhysicalDevice) {
        uint32_t extensionCount = 0;
        vkEnumerateDeviceExtensionProperties(testPhysicalDevice, nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableDeviceExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(testPhysicalDevice, nullptr, &extensionCount, availableDeviceExtensions.data());

        auto isExtensionSupported = [availableDeviceExtensions](const char* requiredDeviceExtension) {
            for (const auto& availableDeviceExtension : availableDeviceExtensions) {
                if (strcmp(requiredDeviceExtension, availableDeviceExtension.extensionName) == 0)
                    return true;
            }
            return false;
        };

        return std::ranges::all_of(requiredDeviceExtensions, isExtensionSupported);
    }

    SwapchainSupportDetails getSwapchainSupportDetails(VkPhysicalDevice physical_device) {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, context.surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, context.surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, context.surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, context.surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, context.surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    void pickSuitableDevice() {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, nullptr);
        if (physicalDeviceCount == 0) throw std::runtime_error("Unable to find Vulkan-supported GPU!");
        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
        vkEnumeratePhysicalDevices(context.instance, &physicalDeviceCount, physicalDevices.data());

        auto isPhysicalDeviceSuitable = [](VkPhysicalDevice physical_device) {
            QueueFamilyIndices indices = findQueueFamilies(physical_device);
            bool deviceExtensionsSupported = verifyDeviceExtensionSupport(physical_device);
            bool swapchainSupport = false;

            if (deviceExtensionsSupported) {
                SwapchainSupportDetails details = getSwapchainSupportDetails(physical_device);
                swapchainSupport = !details.formats.empty() && !details.presentModes.empty();

            }
            return indices.isValid() && deviceExtensionsSupported && swapchainSupport;
        };

        auto first_suitable = std::ranges::find_if(physicalDevices, isPhysicalDeviceSuitable);

        if (first_suitable == std::end(physicalDevices))
            throw std::runtime_error("Unable to find suitable GPU!");

        context.physicalDevice = *first_suitable;
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(context.physicalDevice);

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
        fillQueueCreateInfo(queueCreateInfo, indices.graphicsFamily.value(), 1, queuePriorities);
        queueCreateInfos.push_back(queueCreateInfo);
        //presentQueue
        if (indices.presentFamily.value() != indices.graphicsFamily.value()) {
            fillQueueCreateInfo(queueCreateInfo, indices.presentFamily.value(), 1, queuePriorities);
            queueCreateInfos.push_back(queueCreateInfo);
        }



        //The physical device features we want to be able to use
        VkPhysicalDeviceFeatures physicalDeviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();
        createInfo.pEnabledFeatures = &physicalDeviceFeatures;

        createInfo.enabledExtensionCount = 0;
        if (validationLayersEnabled) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        }
        else {
            createInfo.enabledLayerCount = 0;
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtensions.size());
        createInfo.ppEnabledExtensionNames = requiredDeviceExtensions.data();

        if (vkCreateDevice(context.physicalDevice, &createInfo, nullptr, &context.device) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create logical device!");
        }

        vkGetDeviceQueue(context.device, indices.graphicsFamily.value(), 0, &context.graphicsQueue);
        vkGetDeviceQueue(context.device, indices.presentFamily.value(), 0, &context.presentQueue);
    }

    void createSurface() {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = (HWND)pWindow->getHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(context.instance, &createInfo, nullptr, &context.surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void createSwapchain() {
        uint32_t width = pWindow->getProperties().width;
        uint32_t height = pWindow->getProperties().height;
        log::rp_info("Creating swapchain, window size: {}x{}", width, height);

        auto chooseSwapSurfaceFormat = [](const std::vector<VkSurfaceFormatKHR>&availableFormats) {
            for (const auto& availableFormat : availableFormats) {
                if (availableFormat.format == g_preferredSurfaceFormat.format &&
                    availableFormat.colorSpace == g_preferredSurfaceFormat.colorSpace) {
                    return availableFormat;
                }
            }

            return availableFormats[0];
        };


        auto choosePresentMode = [](const std::vector <VkPresentModeKHR>& availablePresentModes) {
            for (const auto& availablePresentMode : availablePresentModes) {
                if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                    return availablePresentMode;
                }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        };

        auto chooseSwapExtent = [width, height](const VkSurfaceCapabilitiesKHR& capabilities) {
            if (capabilities.currentExtent.width != UINT32_MAX) {
                return capabilities.currentExtent;
            }
            else {
                VkExtent2D extents = {

                    width,
                    height
                };

                extents.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, extents.width));
                extents.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, extents.height));

                return extents;
            }
        };

        auto swapchainSupport = getSwapchainSupportDetails(context.physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapchainSupport.formats);
        VkPresentModeKHR presentMode = choosePresentMode(swapchainSupport.presentModes);
        VkExtent2D extents = chooseSwapExtent(swapchainSupport.capabilities);
        uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
        if (swapchainSupport.capabilities.maxImageCount > 0 && imageCount > swapchainSupport.capabilities.maxImageCount) {
            imageCount = swapchainSupport.capabilities.maxImageCount;
        }


        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = context.surface;
        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extents;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


        QueueFamilyIndices indices = findQueueFamilies(context.physicalDevice);
        uint32_t queueFamilyIndices[2] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.queueFamilyIndexCount = 0;
            createInfo.pQueueFamilyIndices = nullptr;
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(context.device, &createInfo, nullptr, &swapchain.handle) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create swapchain!");
        }

        vkGetSwapchainImagesKHR(context.device, swapchain.handle, &imageCount, nullptr);
        swapchain.images.resize(imageCount);
        vkGetSwapchainImagesKHR(context.device, swapchain.handle, &imageCount, swapchain.images.data());

        swapchain.imageFormat = surfaceFormat.format;
        swapchain.extent = extents;
    }

    void createSwapchainImageViews() {
        swapchain.imageViews.resize(swapchain.images.size());
        for (size_t i = 0; i < swapchain.images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = swapchain.images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = swapchain.imageFormat;
            createInfo.components = {
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY,
                VK_COMPONENT_SWIZZLE_IDENTITY
            };
            createInfo.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0,
                1,
                0,
                1
            };

            if (vkCreateImageView(context.device, &createInfo, nullptr, &swapchain.imageViews[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create image views!");
            }
        }
    }


    VkShaderModule createShaderModule(const std::vector<char>& code) {
        VkShaderModule shaderModule;

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        if (vkCreateShaderModule(context.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module!");
        }

        return shaderModule;
    }

    void createRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapchain.imageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = &colorAttachment;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpass;
        createInfo.dependencyCount = 1;
        createInfo.pDependencies = &dependency;

        if (vkCreateRenderPass(context.device, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void createGraphicsPipeline() {
        const std::string& shaderDir = "../../../../../shaders/";
        auto vertShader = util::loadBinaryFile(shaderDir + "vert.spv");
        auto fragShader = util::loadBinaryFile(shaderDir + "frag.spv");

        VkShaderModule vertShaderModule = createShaderModule(vertShader);
        VkShaderModule fragShaderModule = createShaderModule(fragShader);
      
        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };


        //Vertex Input Stage
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;

        //Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        //Viewport
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapchain.extent.width;
        viewport.height = (float)swapchain.extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = { 0, 0 };
        scissor.extent = swapchain.extent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        //Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        rasterizer.depthBiasConstantFactor = 0.0f;
        rasterizer.depthBiasClamp = 0.0f;
        rasterizer.depthBiasSlopeFactor = 0.0f;


        //Multisampling
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisampling.minSampleShading = 1.0f;
        multisampling.pSampleMask = nullptr;
        multisampling.alphaToCoverageEnable = VK_FALSE;
        multisampling.alphaToOneEnable = VK_FALSE;

        //Depth and Stencil Testing
        //not using, pass nullptr

        //Color Blending
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                            VK_COLOR_COMPONENT_G_BIT |
                                            VK_COLOR_COMPONENT_B_BIT |
                                            VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        //Dynamic State
        //not using for now

        //Pipeline Layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(context.device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }


        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = 2;
        createInfo.pStages = shaderStages;
        createInfo.pVertexInputState = &vertexInputInfo;
        createInfo.pInputAssemblyState = &inputAssembly;
        createInfo.pViewportState = &viewportState;
        createInfo.pRasterizationState = &rasterizer;
        createInfo.pMultisampleState = &multisampling;
        createInfo.pDepthStencilState = nullptr;
        createInfo.pColorBlendState = &colorBlending;
        createInfo.pDynamicState = nullptr;
        createInfo.layout = pipelineLayout;
        createInfo.renderPass = renderPass;
        createInfo.subpass = 0;
        createInfo.basePipelineHandle = VK_NULL_HANDLE;
        createInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(context.device, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline");
        }

        vkDestroyShaderModule(context.device, fragShaderModule, nullptr);
        vkDestroyShaderModule(context.device, vertShaderModule, nullptr);
    }


    void createSwapchainFramebuffers() {
        swapchain.frameBuffers.resize(swapchain.imageViews.size());

        //Create a framebuffer for each image view in the swapchain
        for (size_t i = 0; i < swapchain.imageViews.size(); i++) {
            VkImageView attachments[] = {
                swapchain.imageViews[i]
            };

            VkFramebufferCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.renderPass = renderPass;
            createInfo.attachmentCount = 1;
            createInfo.pAttachments = attachments;
            createInfo.width = swapchain.extent.width;
            createInfo.height = swapchain.extent.height;
            createInfo.layers = 1;

            if (vkCreateFramebuffer(context.device, &createInfo, nullptr, &swapchain.frameBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create framebuffer");
            }
        }
    }


    void createCommandPool() {
        QueueFamilyIndices queueFamilyIndices = findQueueFamilies(context.physicalDevice);

        VkCommandPoolCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
        createInfo.flags = 0;

        if (vkCreateCommandPool(context.device, &createInfo, nullptr, &context.commandPool) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create command pool!");
        }
    }

    void createCommandBuffers() {
        context.commandBuffers.resize(swapchain.frameBuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = context.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)context.commandBuffers.size();

        if (vkAllocateCommandBuffers(context.device, &allocInfo, context.commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }

        for (size_t i = 0; i < context.commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(context.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin command recording");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapchain.frameBuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapchain.extent;

            VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
            renderPassInfo.clearValueCount = 1;
            renderPassInfo.pClearValues = &clearColor;

            vkCmdBeginRenderPass(context.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(context.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDraw(context.commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(context.commandBuffers[i]);

            if (vkEndCommandBuffer(context.commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }
    }

    void createSyncObjects() {
        imagesInFlight.resize(swapchain.images.size(), VK_NULL_HANDLE);
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(context.device, &semaphoreCreateInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(context.device, &semaphoreCreateInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(context.device, &fenceCreateInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to create semaphores!");
            }
        }
    }

    void cleanupSwapchain() {
        for (auto framebuffer : swapchain.frameBuffers) {
            vkDestroyFramebuffer(context.device, framebuffer, nullptr);
        }

        vkFreeCommandBuffers(context.device, context.commandPool, static_cast<uint32_t>(context.commandBuffers.size()), context.commandBuffers.data());

        vkDestroyPipeline(context.device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(context.device, pipelineLayout, nullptr);
        vkDestroyRenderPass(context.device, renderPass, nullptr);

        for (auto imageView : swapchain.imageViews) {
            vkDestroyImageView(context.device, imageView, nullptr);
        }
        vkDestroySwapchainKHR(context.device, swapchain.handle, nullptr);
    }

    void recreateSwapchain() {
        vkDeviceWaitIdle(context.device);

        cleanupSwapchain();

        createSwapchain();
        createSwapchainImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createSwapchainFramebuffers();
        createCommandBuffers();
    }

    void init(Window* window, const std::string appName, util::Version appVersion, const std::string& engineName, util::Version engineVersion) {
        pWindow = window;
        log::rp_info(log::horiz_rule);
        log::rp_info("Initializing Renderer");


        if (validationLayersEnabled) {
            verifyRequiredLayers();
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        verifyRequiredExtensions();

        createInstance(appName, appVersion, engineName, engineVersion);

        if (validationLayersEnabled) {
            setupDebugCallback(context.instance);
        }

        createSurface();
        pickSuitableDevice();
        createLogicalDevice();
        createSwapchain();
        createSwapchainImageViews();
        createRenderPass();
        createGraphicsPipeline();
        createSwapchainFramebuffers();
        createCommandPool();
        createCommandBuffers();
        createSyncObjects();

        log::rp_info(log::horiz_rule);
    }

    void shutdown() {
        log::rp_info("Shutting Down Renderer");
        vkDeviceWaitIdle(context.device);

        cleanupSwapchain();

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(context.device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(context.device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(context.device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(context.device, context.commandPool, nullptr);

        vkDestroyDevice(context.device, nullptr);
        vkDestroySurfaceKHR(context.instance, context.surface, nullptr);
        if (validationLayersEnabled) {
            DestroyDebugUtilsMessengerEXT(context.instance, debugMessenger, nullptr);
        }
        vkDestroyInstance(context.instance, nullptr);
    }

    void draw() {
        if (pWindow->isMinimized()) return;
        vkWaitForFences(context.device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(context.device, swapchain.handle, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            log::rp_warn("Swapchain out of date, recreating!");
            recreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swapchain image");
        }

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(context.device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &context.commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(context.device, 1, &inFlightFences[currentFrame]);

        if (vkQueueSubmit(context.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer");
        }


        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = { swapchain.handle };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR(context.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            log::rp_warn("Swapchain out of date, recreating!");
            recreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
}