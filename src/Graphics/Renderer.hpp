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
#include "../Utility/structs.hpp"

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

            void readyCanvas(GLFWwindow* window);
            void drawDemo();
            void finishDraw(GLFWwindow* window);

            void signalWindowResize();
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
            void recreateSwapchain(GLFWwindow* window);
            void cleanupSwapchain();

            void createGraphicsPipeline();
            VkShaderModule createShaderModule(const std::vector<char>& code);
            void createRenderPass();

            void createFramebuffers();
            void createCommandPool();
            void createCommandBuffer();
            void createSyncObjects();

            void beginRecording(VkCommandBuffer buffer);
            void endRecording(VkCommandBuffer buffer);

            void createVertexBuffer();
            void createIndexBuffer();
            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory );
            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            
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
            VkQueue mTransferQueue;
            
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
            std::vector<VkCommandBuffer> mCommandBuffers;

            std::vector<VkFramebuffer> mSwapchainFramebuffers;

            std::vector<VkSemaphore> mImageAvailableSemaphores;
            std::vector<VkSemaphore> mRenderFinishedSemaphores;
            std::vector<VkFence> mInFlightFences;
            std::vector<VkFence> mImagesInFlight;

            //DEBUG
            VkDebugUtilsMessengerEXT mDebugMessenger;

            const int MAXFRAMESINFLIGHT = 2;
            uint32_t currentFrameInFlight = 0;
            uint32_t currentImageIndex = 0;
            bool framebufferResized = false;

            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;

            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
        };

        const std::vector<util::str::Vertex2P3C> vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
            {{ 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{ 0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}}
        };

        const std::vector<uint16_t> indices =
        {
            0, 3, 2,
            2, 1, 0
        };
    }
}