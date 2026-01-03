#pragma once

#define VK_USE_PLATFORM_XCB_KHR
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

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
            
            void init(GLFWwindow* window);
            void terminate();
        private:
            Renderer() = default;

            void createVulkanInstance();
            void pickPhysicalDevice();
            int ratePhysicalDeviceSuitability(VkPhysicalDevice device);
            void createLogicalDevice();
            void createWindowSurface(GLFWwindow* window);
            bool checkDeviceExtensionSupport(VkPhysicalDevice device);
            util::QueueFamilyIndices findQueueFamilyIndices(VkPhysicalDevice device);
            util::SwapchainSupportDetails querySwapchainSupport(VkPhysicalDevice device);
            
            VkSurfaceFormatKHR chooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available);
            VkPresentModeKHR chooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& available);
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& cap, GLFWwindow* window);
            void createSwapchain(GLFWwindow* window);
            void createSwapchainImageViews();

            void createGraphicsPipeline();
            VkShaderModule createShaderModule(const std::vector<char>& code);
            void createRenderPass();

            void createFramebuffers();
            void createCommandPool();
            void createCommandBuffer();
            void recordCommandBuffer(VkCommandBuffer buffer, uint32_t imageIndex);    

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
            VkDevice mDevice;

            VkQueue mGraphicsQueue;
            VkQueue mPresentQueue;

            VkSurfaceKHR mSurface;
            VkSwapchainKHR mSwapchain;  
            std::vector<VkImage> mSwapchainImages;
            VkFormat mSwapchainFormat;
            VkExtent2D mSwapchainExtent;
            std::vector<VkImageView> mSwapchainImageViews;

            VkPipelineLayout mPipelineLayout;
            VkPipeline mPipeline;
            VkRenderPass mRenderPass;

            VkCommandPool mCommandPool;
            VkCommandBuffer mCommandBuffer;

            std::vector<VkFramebuffer> mSwapchainFramebuffers;

            //DEBUG
            VkDebugUtilsMessengerEXT mDebugMessenger;
        };
    }
}