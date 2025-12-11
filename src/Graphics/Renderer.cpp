#include "Renderer.hpp"

ke::Graphics::Renderer &ke::Graphics::Renderer::getInstance()
{
    static Renderer instance;
    return instance;
}

void ke::Graphics::Renderer::init()
{
    createVulkanInstance();
}

void ke::Graphics::Renderer::terminate()
{
    vkDestroyInstance(mInstance, nullptr);
}

void ke::Graphics::Renderer::createVulkanInstance()
{
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_VERSION_1_0;
    appInfo.pApplicationName = "Knaj's Engine";
    appInfo.pEngineName = "No Engine";

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 0;
    createInfo.enabledLayerCount = 0;

    if(vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS)
        mLogger.critical("Failed to create a Vulkan instance!");
}
