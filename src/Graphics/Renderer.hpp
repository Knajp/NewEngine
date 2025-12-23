#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "../Utility/Logger.hpp"
#include "../Utility/RenderUtil.hpp"

namespace ke
{
    namespace Graphics
    {
        class Renderer
        {
        public:
            static Renderer& getInstance();
            
            void init();
            void terminate();
        private:
            Renderer() = default;

            void createVulkanInstance();
            void pickPhysicalDevice();
            int ratePhysicalDeviceSuitability(VkPhysicalDevice device);


            util::QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);

            //DEBUG
            bool checkValidationLayerSupport();
            void setupDebugMessenger();
            void DestroyDebugUtilsMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
            VkResult CreateDebugUtilsMessenger(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
            std::vector<const char*> getRequiredExtensions();
        private:
            util::Logger mLogger = util::Logger("Render Logger");

            VkInstance mInstance;
            
            VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
            //DEBUG
            VkDebugUtilsMessengerEXT mDebugMessenger;
        };
    }
}