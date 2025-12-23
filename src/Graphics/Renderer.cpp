#include "Renderer.hpp"
#include <iostream>

const std::vector<const char*> gValidationLayers = 
{"VK_LAYER_KHRONOS_validation"};

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

void ke::Graphics::Renderer::init()
{
    mLogger.trace("Initializing renderer.");
    createVulkanInstance();
    setupDebugMessenger();
    pickPhysicalDevice();

    mLogger.info("Initialized renderer.");
}

void ke::Graphics::Renderer::terminate()
{
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

    if(!deviceFeatures.geometryShader) return 0;
    
    util::QueueFamilyIndices indices = findQueueFamilyIndices(device);
    if(!indices.isComplete()) return 0;

    return score;
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

        if(indices.isComplete())
            break;
        i++;
    }

    return indices;
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
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
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