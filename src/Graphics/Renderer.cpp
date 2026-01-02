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

    mLogger.info("Initialized renderer.");
}

void ke::Graphics::Renderer::terminate()
{
    for(auto imageView : mSwapchainImageViews)
        vkDestroyImageView(mDevice, imageView, nullptr);
    vkDestroySwapchainKHR(mDevice, mSwapchain, nullptr);
    vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
    vkDestroyDevice(mDevice, nullptr);
    DestroyDebugUtilsMessenger(mInstance, mDebugMessenger, nullptr);
    vkDestroyInstance(mInstance, nullptr);
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
}

void ke::Graphics::Renderer::createSwapchain(GLFWwindow *window)
{
    util::SwapchainSupportDetails support = querySwapchainSupport(mPhysicalDevice);

    mLogger.info(std::to_string(support.surfaceFormats.size()).c_str());
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

void ke::Graphics::Renderer::createGraphicsPipeline()
{
    auto vertexCode = ke::util::readFile("shader/bin/vert.spv");
    auto fragCode = ke::util::readFile("shader/bin/frag.spv");

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