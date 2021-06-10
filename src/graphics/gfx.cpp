#include "pch.hpp"
#include "graphics/gfx.hpp"
#include "graphics/vulkan_device.hpp"
#include "graphics/vulkan_queue.hpp"
#include "graphics/vulkan_validation.hpp"
#include "graphics/swapchain.hpp"
#include "graphics/shaders.hpp"
#include "graphics/framesync.hpp"
#include "util/version.hpp"
#include "util/file_io.hpp"
#include "core/window.hpp"

#pragma warning(push)
#pragma warning( disable : 26812 ) //Disable unscoped enum warning (VkError, etc.)
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>

namespace rp::gfx {
#ifdef NDEBUG
    const bool validationLayersEnabled = false;
#else
    const bool validationLayersEnabled = true;
#endif

    void VerifyExtensionSupport(const std::vector<const char*>& extensions) {
        uint32_t availableExtensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());

        log::rp_info("Verifying Extensions:");
        for (const auto& required : extensions) {
            log::rp_info("\t{}", required);
            if (std::ranges::none_of(availableExtensions, [&required](const auto& props) { return !strcmp(required, props.extensionName); }))
                throw std::runtime_error(fmt::format("Vulkan Extension \"{}\" not found!", required));
        }
    }

    VkInstance instance = VK_NULL_HANDLE;
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    VulkanDevice device;
    Swapchain swapchain;
    Window* pWindow = nullptr;

    //Pipeline and render pass for rendering the triangle
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkRenderPass renderPass;


    FrameSynchronizer frameSync;


    ShaderSet CreateTriangleShaderSet() {
        const std::string& shaderDir = "../../../../../shaders/";
        auto vertShaderData = util::loadBinaryFile(shaderDir + "vert.spv");
        auto fragShaderData = util::loadBinaryFile(shaderDir + "frag.spv");

        auto vertShader = CreateShaderModule(device.handle, vertShaderData);
        auto fragShader = CreateShaderModule(device.handle, fragShaderData);

        return {
            vertShader,
            fragShader
        };
    }

    void CreateInstance(const std::string& appName,
                        util::Version appVersion,
                        const std::string& engineName,
                        util::Version engineVersion,
                        const std::vector<const char*>& layers,
                        const std::vector<const char*>& extensions) {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = appName.c_str();
        appInfo.applicationVersion = VK_MAKE_VERSION(appVersion.major, appVersion.minor, appVersion.patch);
        appInfo.pEngineName = engineName.c_str();
        appInfo.engineVersion = VK_MAKE_VERSION(engineVersion.major, engineVersion.minor, engineVersion.patch);
        appInfo.apiVersion = VK_API_VERSION_1_1;

        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        createInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        createInfo.ppEnabledLayerNames = layers.data();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
        if (result != VK_SUCCESS) {
            log::rp_error("Failed to Create Vulkan Instance! result = {}", result);
            exit(-1);
        }
    }

    void CreateSurface(Window* window) {
        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = (HWND)window->getHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);

        if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface!");
        }
    }

    void CreateRenderPass(VkDevice device, VkFormat swapChainFormat) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = swapChainFormat;
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

        if (vkCreateRenderPass(device, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create render pass!");
        }
    }

    void CreateGraphicsPipeline(const ShaderSet& shaders, VkRenderPass renderPass) {

        auto shaderStageCreateInfos = shaders.GenerateStageCreateInfos();

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

        if (vkCreatePipelineLayout(device.handle, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }


        VkGraphicsPipelineCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        createInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
        createInfo.pStages = shaderStageCreateInfos.data();
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

        if (vkCreateGraphicsPipelines(device.handle, VK_NULL_HANDLE, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create graphics pipeline");
        }

    }

    void CreateCommandBuffers() {
        device.commandBuffers.resize(swapchain.frameBuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = device.commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)device.commandBuffers.size();

        if (vkAllocateCommandBuffers(device.handle, &allocInfo, device.commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers!");
        }

        for (size_t i = 0; i < device.commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(device.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
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

            vkCmdBeginRenderPass(device.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(device.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            vkCmdDraw(device.commandBuffers[i], 3, 1, 0, 0);
            vkCmdEndRenderPass(device.commandBuffers[i]);

            if (vkEndCommandBuffer(device.commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }
    }

    void CleanupSwapchain() {
        for (auto framebuffer : swapchain.frameBuffers) {
            vkDestroyFramebuffer(device.handle, framebuffer, nullptr);
        }

        vkFreeCommandBuffers(device.handle, device.commandPool, static_cast<uint32_t>(device.commandBuffers.size()), device.commandBuffers.data());

        vkDestroyPipeline(device.handle, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(device.handle, pipelineLayout, nullptr);
        vkDestroyRenderPass(device.handle, renderPass, nullptr);

        swapchain.Cleanup(device.handle);
    }

    void RecreateSwapchain() {
        vkDeviceWaitIdle(device.handle);

        CleanupSwapchain();

        auto windowProps = pWindow->GetProperties();
        SwapchainSupportDetails swapchainSupport = Swapchain::GetSupportDetails(device.physicalDevice, surface);
        QueueFamilyIndices queueFamilies = FindQueueFamilies(device.physicalDevice, surface);
        swapchain.Create(device.handle, surface, swapchainSupport, queueFamilies, windowProps.width, windowProps.height);
        CreateRenderPass(device.handle, swapchain.imageFormat);

        ShaderSet triangleShaderSet = CreateTriangleShaderSet();
        CreateGraphicsPipeline(triangleShaderSet, renderPass);
        swapchain.CreateFrameBuffers(device.handle, renderPass);
        CreateCommandBuffers();

        vkDestroyShaderModule(device.handle, triangleShaderSet.vertShader, nullptr);
        vkDestroyShaderModule(device.handle, triangleShaderSet.fragShader, nullptr);
    }

    void Initialize(Window* window, const std::string appName, util::Version appVersion, const std::string& engineName, util::Version engineVersion) {
        pWindow = window;
        log::rp_info(log::horiz_rule);
        log::rp_info("Initializing Renderer");

        std::vector<const char*> requiredLayers = {};
        std::vector<const char*> requiredExtensions = {
            VK_KHR_SURFACE_EXTENSION_NAME,
            "VK_KHR_win32_surface"
        };
        std::vector<const char*> requiredDeviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        if (validationLayersEnabled) {
            requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        VerifyLayerSupport(requiredLayers);
        VerifyExtensionSupport(requiredExtensions);
        CreateInstance(appName, appVersion, engineName, engineVersion, requiredLayers, requiredExtensions);
        if (validationLayersEnabled) {
            setupDebugCallback(instance);
        }

        CreateSurface(pWindow);
        device.Create(instance, surface, requiredLayers, requiredDeviceExtensions);

        auto windowProps = pWindow->GetProperties();
        SwapchainSupportDetails swapchainSupport = Swapchain::GetSupportDetails(device.physicalDevice, surface);
        QueueFamilyIndices queueFamilies = FindQueueFamilies(device.physicalDevice, surface);

        swapchain.Create(device.handle, surface, swapchainSupport, queueFamilies, windowProps.width, windowProps.height);

        CreateRenderPass(device.handle, swapchain.imageFormat);

        ShaderSet triangleShaderSet = CreateTriangleShaderSet();
        CreateGraphicsPipeline(triangleShaderSet, renderPass);
        swapchain.CreateFrameBuffers(device.handle, renderPass);
        CreateCommandBuffers();
        frameSync.Create(device.handle, swapchain.imageCount);

        vkDestroyShaderModule(device.handle, triangleShaderSet.vertShader, nullptr);
        vkDestroyShaderModule(device.handle, triangleShaderSet.fragShader, nullptr);

        log::rp_info(log::horiz_rule);
    }

    void Shutdown() {
        log::rp_info("Shutting Down Renderer");
        vkDeviceWaitIdle(device.handle);
        CleanupSwapchain();

        frameSync.Cleanup(device.handle);
        device.Cleanup();
        vkDestroySurfaceKHR(instance, surface, nullptr);
        if (validationLayersEnabled) {
            cleanupDebugCallback(instance);
        }
        vkDestroyInstance(instance, nullptr);
    }

    void DrawFrame() {
        if (pWindow->isMinimized()) return;
        frameSync.WaitFrameFence(device.handle);

        //Get the next available swapchain image
        uint32_t imageIndex = 0;
        VkResult result = vkAcquireNextImageKHR(device.handle, swapchain.handle, UINT64_MAX, frameSync.GetNextWaitSemaphore(), VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            log::rp_warn("Swapchain out of date, recreating!");
            RecreateSwapchain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swapchain image");
        }

        //Submit the command buffer to the graphics queue
        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { frameSync.GetNextWaitSemaphore() };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &device.commandBuffers[imageIndex];

        VkSemaphore signalSemaphores[] = { frameSync.GetNextSignalSemaphore() };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        frameSync.ResetFrameFence(device.handle);

        if (vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, frameSync.GetNextFrameFence()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit draw command buffer");
        }


        //present the image to the screen after waiting for the
        //graphics queue to finish processing the command buffer
        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapchains[] = { swapchain.handle };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.pResults = nullptr;

        result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            log::rp_warn("Swapchain out of date, recreating!");
            RecreateSwapchain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        frameSync.IncrementFrame();
    }
}

#pragma warning(pop)