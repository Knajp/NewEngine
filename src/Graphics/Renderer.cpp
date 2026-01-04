#include "Renderer.hpp"
#include <iostream>
#include <set>
const std::vector<const char*> gValidationLayers = 
{"VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> gDeviceExtensions = 
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

#ifdef DEBUG
bool enableValidationLayers = true;
#else
bool enableValidationLayers = false;
#endif

ke::Graphics::Renderer &ke::Graphics::Renderer::getInstance()
{
    static Renderer instance;
    return instance;
}

void ke::Graphics::Renderer::init(GLFWwindow* window)
{
    mLogger.trace("Initializing renderer.");
    createVulkanInstance();
    setupDebugMessenger();
    createWindowSurface(window);
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapchain(window);
    createSwapchainImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createCommandBuffer();
    createSyncObjects();

    mLogger.info("Initialized renderer.");
}

void ke::Graphics::Renderer::terminate()
{
    vkDeviceWaitIdle(mDevice);


    for(size_t i = 0; i < mSwapchainImages.size(); i++)
        vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
    for(int i = 0; i < MAXFRAMESINFLIGHT; i++)
    {
        vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
        vkDestroyFence(mDevice, mInFlightFences[i], nullptr);    
    }
    
    cleanupSwapchain();

    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroyPipeline(mDevice, mPipeline, nullptr);
    vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
    vkDestroyRenderPass(mDevice, mRenderPass, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyDevice(mDevice, nullptr);
    DestroyDebugUtilsMessenger(mInstance, mDebugMessenger, nullptr);
    vkDestroyInstance(mInstance, nullptr);
}

void ke::Graphics::Renderer::draw(GLFWwindow* window)
{
    vkWaitForFences(mDevice, 1, &mInFlightFences[currentFrameInFlight], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult status = vkAcquireNextImageKHR(mDevice, mSwapchain, UINT64_MAX, mImageAvailableSemaphores[currentFrameInFlight], VK_NULL_HANDLE, &imageIndex);

    if(status == VK_ERROR_OUT_OF_DATE_KHR)
    {
        recreateSwapchain(window);

        currentFrameInFlight = (currentFrameInFlight + 1) % MAXFRAMESINFLIGHT;

        return;
    }
    else if(status != VK_SUCCESS && status != VK_SUBOPTIMAL_KHR)
        mLogger.error("Failed to acquire swapchain image!");

    if(mImagesInFlight[imageIndex] != VK_NULL_HANDLE)
        vkWaitForFences(mDevice, 1, &mImagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);

    mImagesInFlight[imageIndex] = mInFlightFences[currentFrameInFlight];

    vkResetFences(mDevice, 1, &mInFlightFences[currentFrameInFlight]);

    vkResetCommandBuffer(mCommandBuffers[currentFrameInFlight], 0);
    recordCommandBuffer(mCommandBuffers[currentFrameInFlight], imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {mImageAvailableSemaphores[currentFrameInFlight]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &mCommandBuffers[currentFrameInFlight];
    
    VkSemaphore signalSemaphores[] = {mRenderFinishedSemaphores[imageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if(vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, mInFlightFences[currentFrameInFlight]) != VK_SUCCESS)
        mLogger.error("Failed to submit to graphics queue!");

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapchains[] = {mSwapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(mPresentQueue, &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
    {
        recreateSwapchain(window);
        framebufferResized = false;
    }
    currentFrameInFlight = (currentFrameInFlight + 1) % MAXFRAMESINFLIGHT;
}

void ke::Graphics::Renderer::signalWindowResize()
{
    framebufferResized = true;
}

void ke::Graphics::Renderer::createVulkanInstance()
{
    if(enableValidationLayers && !checkValidationLayerSupport())
        mLogger.critical("Failed to supply requested validation layers!");
    
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    appInfo.pApplicationName = "Knaj's Engine";
    appInfo.pEngineName = "No Engine";

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(gValidationLayers.size());
        createInfo.ppEnabledLayerNames = gValidationLayers.data();
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    if(vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
        mLogger.critical("Failed to create a Vulkan instance!");
}

void ke::Graphics::Renderer::pickPhysicalDevice()
{
    uint32_t deviceCount;
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
    if(deviceCount == 0)
    {
        mLogger.critical("Failed to find a Graphics Processing Unit with Vulkan support");
        return;
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

    std::map<int, VkPhysicalDevice> candidates;
    for(const auto& device : devices)
    {
        candidates.insert(std::make_pair(ratePhysicalDeviceSuitability(device), device));
    }
    

    if(candidates.rbegin()->first > 0)
        mPhysicalDevice = candidates.rbegin()->second;
    else
        mLogger.critical("Failed to find a suitable GPU!");
}

int ke::Graphics::Renderer::ratePhysicalDeviceSuitability(VkPhysicalDevice device)
{
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    int score = 0;
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    score += deviceProperties.limits.maxImageDimension2D;

    if(!deviceFeatures.geometryShader) return 0;
    if(!checkDeviceExtensionSupport(device)) return 0;

    util::QueueFamilyIndices indices = findQueueFamilyIndices(device);
    if(!indices.isComplete()) return 0;

    return score;
}

void ke::Graphics::Renderer::createLogicalDevice()
{
    float queuePriorities = 1.0f;
    util::QueueFamilyIndices indices = findQueueFamilyIndices(mPhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueInfos;
    std::set<uint32_t> uniqueQueueIndices = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    for(auto index : uniqueQueueIndices)
    {
        VkDeviceQueueCreateInfo createInfo{};
        createInfo.pQueuePriorities = &queuePriorities;
        createInfo.queueCount = 1;
        createInfo.queueFamilyIndex = index;
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfos.push_back(createInfo);
    }


    VkPhysicalDeviceFeatures deviceFeatures{};


    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
    createInfo.pQueueCreateInfos = queueInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(gDeviceExtensions.size());
    createInfo.ppEnabledExtensionNames = gDeviceExtensions.data();
    
    if(enableValidationLayers)
    {
        createInfo.enabledLayerCount = 1;
        createInfo.ppEnabledLayerNames = gValidationLayers.data();
    }
    else createInfo.enabledLayerCount = 0;

    if(vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS)
        mLogger.critical("Failed to create logical device!");

    vkGetDeviceQueue(mDevice, indices.graphicsFamily.value(), 0, &mGraphicsQueue);
    vkGetDeviceQueue(mDevice, indices.presentFamily.value(), 0, &mPresentQueue);
}

void ke::Graphics::Renderer::createWindowSurface(GLFWwindow* window)
{
    if(glfwCreateWindowSurface(mInstance, window, nullptr, &mSurface) != VK_SUCCESS)
        mLogger.error("Failed to create a window surface!");
}

bool ke::Graphics::Renderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

    std::set<std::string> reqExtensions(gDeviceExtensions.begin(), gDeviceExtensions.end());

    for(const auto& avEx : extensions)
    {
        reqExtensions.erase(avEx.extensionName);
    }

    return reqExtensions.empty();
}

ke::util::QueueFamilyIndices ke::Graphics::Renderer::findQueueFamilyIndices(VkPhysicalDevice device)
{
    util::QueueFamilyIndices indices;

    uint32_t familyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, nullptr);
    std::vector<VkQueueFamilyProperties> families(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &familyCount, families.data());

    int i = 0;
    for(const auto& prop : families)
    {
        if(prop.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        VkBool32 presentSupport = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

        if(presentSupport) indices.presentFamily = i;

        if(indices.isComplete())
            break;
        i++;
    }

    return indices;
}

ke::util::SwapchainSupportDetails ke::Graphics::Renderer::querySwapchainSupport(VkPhysicalDevice device)
{
   util::SwapchainSupportDetails swapchainSupport;
   
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &swapchainSupport.surfaceCapabilities);

    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);
    
    if(formatCount > 0)
    {
        swapchainSupport.surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, swapchainSupport.surfaceFormats.data());   
    }
        
    uint32_t modeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &modeCount, nullptr);

    if(modeCount > 0)
    {
        swapchainSupport.presentModes.resize(modeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &modeCount, swapchainSupport.presentModes.data());
    }

   return swapchainSupport;
}

VkSurfaceFormatKHR ke::Graphics::Renderer::chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &available)
{
    for(const auto& format : available)
    {
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    mLogger.warn("No suitable surface format found!");
    return available[0];

}

VkPresentModeKHR ke::Graphics::Renderer::chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR> &available)
{
    for(const auto& mode : available)
    {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }

    mLogger.warn("Desired present mode not supported, defaulting to FIFO.");

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D ke::Graphics::Renderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &cap, GLFWwindow* window)
{
    if(cap.currentExtent.width != std::numeric_limits<uint32_t>::max())
        return cap.currentExtent;
    
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    VkExtent2D actualExtent =
    {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, cap.minImageExtent.width, cap.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, cap.minImageExtent.height, cap.maxImageExtent.height);

    return actualExtent;
}

void ke::Graphics::Renderer::createSwapchain(GLFWwindow *window)
{
    util::SwapchainSupportDetails support = querySwapchainSupport(mPhysicalDevice);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapchainSurfaceFormat(support.surfaceFormats);
    VkPresentModeKHR presentMode = chooseSwapchainPresentMode(support.presentModes);
    VkExtent2D extent = chooseSwapExtent(support.surfaceCapabilities, window);

    uint32_t imageCount = support.surfaceCapabilities.minImageCount + 1;

    if(support.surfaceCapabilities.maxImageCount > 0 && imageCount > support.surfaceCapabilities.maxImageCount)
        imageCount = support.surfaceCapabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = mSurface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.presentMode = presentMode;
    createInfo.imageExtent = extent;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    util::QueueFamilyIndices indices = findQueueFamilyIndices(mPhysicalDevice);

    uint32_t familyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if(indices.graphicsFamily.value() == indices.presentFamily.value())
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = familyIndices;
    }

    createInfo.preTransform = support.surfaceCapabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapchain) != VK_SUCCESS)
        mLogger.critical("Failed to create a swapchain!");


    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, nullptr);
    mSwapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(mDevice, mSwapchain, &imageCount, mSwapchainImages.data());

    mSwapchainExtent = extent;
    mSwapchainFormat = surfaceFormat.format;
    
}

void ke::Graphics::Renderer::createSwapchainImageViews()
{
    mSwapchainImageViews.resize(mSwapchainImages.size());

        

    for(size_t i = 0; i < mSwapchainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.format = mSwapchainFormat;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.image = mSwapchainImages[i];

        if(vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapchainImageViews[i]) != VK_SUCCESS)
            mLogger.error("Failed to create an image view!");
    }
}

void ke::Graphics::Renderer::recreateSwapchain(GLFWwindow* window)
{
    vkDeviceWaitIdle(mDevice);
    int width, height;
    do
    {
        glfwGetFramebufferSize(window, &width, &height);
        glfwWaitEvents();
    } while (width == 0 || height == 0); 
    

    cleanupSwapchain();

    createSwapchain(window);
    createSwapchainImageViews();
    createFramebuffers();
}

void ke::Graphics::Renderer::cleanupSwapchain()
{
    for(auto fb : mSwapchainFramebuffers)
        vkDestroyFramebuffer(mDevice, fb, nullptr);
    
    for(auto view : mSwapchainImageViews)
        vkDestroyImageView(mDevice, view, nullptr);
    
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
}

void ke::Graphics::Renderer::createGraphicsPipeline()
{
    auto vertexCode = ke::util::readFile("./shader/bin/vert.spv");
    auto fragCode = ke::util::readFile("./shader/bin/frag.spv");

    VkShaderModule vertexModule = createShaderModule(vertexCode);
    VkShaderModule fragmentModule = createShaderModule(fragCode);

    VkPipelineShaderStageCreateInfo vertStageInfo{};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertexModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo{};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragmentModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo stageInfos[] = {vertStageInfo, fragStageInfo};

    std::vector<VkDynamicState> dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates  = dynamicStates.data();

    VkPipelineVertexInputStateCreateInfo vertexInput{};
    vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInput.vertexAttributeDescriptionCount = 0;
    vertexInput.vertexBindingDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) mSwapchainExtent.width;
    viewport.height = (float) mSwapchainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.extent = mSwapchainExtent;
    scissor.offset = {0,0};

    VkPipelineViewportStateCreateInfo viewportInfo{};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.scissorCount = 1;
    viewportInfo.viewportCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    
    VkPipelineMultisampleStateCreateInfo multisample{};
    multisample.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample.sampleShadingEnable = VK_FALSE;
    multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineColorBlendAttachmentState colorAtt{};
    colorAtt.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorAtt.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlend{};
    colorBlend.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlend.logicOpEnable = VK_FALSE;
    colorBlend.attachmentCount = 1;
    colorBlend.pAttachments = &colorAtt;

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if(vkCreatePipelineLayout(mDevice, &layoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
        mLogger.error("Failed to create a pipeline layout!");

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = stageInfos;
    pipelineInfo.pVertexInputState = &vertexInput;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisample;
    pipelineInfo.pColorBlendState = &colorBlend;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = mPipelineLayout;
    pipelineInfo.renderPass = mRenderPass;
    pipelineInfo.subpass = 0;

    if(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline) != VK_SUCCESS)
        mLogger.critical("Failed to create a graphics pipeline!");

    vkDestroyShaderModule(mDevice, vertexModule, nullptr);
    vkDestroyShaderModule(mDevice, fragmentModule, nullptr);
}

VkShaderModule ke::Graphics::Renderer::createShaderModule(const std::vector<char> &code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule;
    if(vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
        mLogger.error("Failed to create shader module!");

    return shaderModule;
}

void ke::Graphics::Renderer::createRenderPass()
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = mSwapchainFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;

    VkSubpassDependency dep{};
    dep.srcSubpass = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass = 0;
    dep.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.srcAccessMask = 0;
    dep.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;


    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dep;

    if(vkCreateRenderPass(mDevice, &createInfo, nullptr, &mRenderPass) != VK_SUCCESS)
        mLogger.error("Failed to create render pass!");

}

void ke::Graphics::Renderer::createFramebuffers()
{
    mSwapchainFramebuffers.resize(mSwapchainImageViews.size());

    for(size_t i = 0; i < mSwapchainImageViews.size(); i++)
    {
        VkImageView attachments[] = {mSwapchainImageViews[i]};

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = mRenderPass;
        createInfo.attachmentCount = 1;
        createInfo.pAttachments = attachments;
        createInfo.height = mSwapchainExtent.height;
        createInfo.width = mSwapchainExtent.width;
        createInfo.layers = 1;

        if(vkCreateFramebuffer(mDevice, &createInfo, nullptr, &mSwapchainFramebuffers[i]) != VK_SUCCESS)
            mLogger.error("Failed to create framebuffer!");
    }
}

void ke::Graphics::Renderer::createCommandPool()
{
    util::QueueFamilyIndices indices = findQueueFamilyIndices(mPhysicalDevice);

    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = indices.graphicsFamily.value();

    if(vkCreateCommandPool(mDevice, &createInfo, nullptr, &mCommandPool) != VK_SUCCESS)
        mLogger.critical("Failed to create a command pool!");
}

void ke::Graphics::Renderer::createCommandBuffer()
{
    mCommandBuffers.resize(MAXFRAMESINFLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandBufferCount = (uint32_t) mCommandBuffers.size();
    allocInfo.commandPool = mCommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    if(vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS)
        mLogger.critical("Failed to allocate command buffers!");
}

void ke::Graphics::Renderer::recordCommandBuffer(VkCommandBuffer buffer, uint32_t imageIndex)
{
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if(vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS)
        mLogger.error("Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderBegin{};
    renderBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderBegin.renderPass = mRenderPass;
    renderBegin.framebuffer = mSwapchainFramebuffers[imageIndex];
    renderBegin.renderArea.offset = {0,0};
    renderBegin.renderArea.extent = mSwapchainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderBegin.clearValueCount = 1;
    renderBegin.pClearValues = &clearColor;

    vkCmdBeginRenderPass(buffer, &renderBegin, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

    VkViewport viewport{};
    viewport.height = mSwapchainExtent.height;
    viewport.width = mSwapchainExtent.width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = mSwapchainExtent;
    scissor.offset = {0, 0};
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    vkCmdDraw(buffer, 6, 1, 0, 0);

    vkCmdEndRenderPass(buffer);

    if(vkEndCommandBuffer(buffer) != VK_SUCCESS)
        mLogger.error("Failed to record command buffer!");
}

void ke::Graphics::Renderer::createSyncObjects()
{
    mImageAvailableSemaphores.resize(MAXFRAMESINFLIGHT);
    mRenderFinishedSemaphores.resize(mSwapchainImages.size());
    mInFlightFences.resize(MAXFRAMESINFLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for(int i = 0; i < MAXFRAMESINFLIGHT; i++)
    {
        if(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
        mLogger.error("Failed to create at least one sync object!");
    }
    for(size_t i = 0; i < mSwapchainImages.size(); i++)
    {
        if(vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS)
            mLogger.error("Failed to create render finished semaphore!");
    }
    mImagesInFlight = std::vector<VkFence>(mSwapchainImages.size(), VK_NULL_HANDLE);
}

bool ke::Graphics::Renderer::checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> layers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

    for(const char* layerName : gValidationLayers)
    {
        bool layerFound = false;

        for(const auto& lprop : layers)
        {
            if(strcmp(layerName, lprop.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound) return false;
    }

    return true;
}

void ke::Graphics::Renderer::setupDebugMessenger()
{
    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;

    if(CreateDebugUtilsMessenger(mInstance, &createInfo, nullptr, &mDebugMessenger) != VK_SUCCESS)
        mLogger.error("Failed to set up debug utils messenger!");
}

void ke::Graphics::Renderer::DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks *pAllocator)
{  
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

VkResult ke::Graphics::Renderer::CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo, const VkAllocationCallbacks *pAllocator, VkDebugUtilsMessengerEXT *pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

std::vector<const char *> ke::Graphics::Renderer::getRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if(enableValidationLayers)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    return extensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    if(messageSeverity <= VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) return VK_FALSE;
    std::cerr << "Validation Layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}