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
            void updateUIUniforms(float aspectRatio);
            void updateSceneUniforms(float aspectRatio);
            void finishDraw(GLFWwindow* window);

            void createTextureImage(const std::string& filepath, util::Image& image);
            void createTextureImageView(util::Image& image);
            uint32_t addTextureToDescriptor(const util::Image& image);

            void signalWindowResize();
            void createVertexBuffer(const std::vector<util::str::Vertex2P3C2T>& vertices, VkBuffer& targetBuffer, VkDeviceMemory& targetMemory);
            void createIndexBuffer(const std::vector<uint16_t>& indices, VkBuffer& targetBuffer, VkDeviceMemory& targetMemory);

            void bindUIPipeline(VkCommandBuffer buffer);
            void bindScenePipeline(VkCommandBuffer buffer, const VkViewport& viewport, const VkRect2D& scissor);

            void pickTextureIndex(int32_t index) const;
            void drawBuffersIndexed(const util::Buffer& vertexBuffer, const util::Buffer& indexBuffer, uint16_t indexCount) const;

            VkDevice getDevice() const;

            VkCommandBuffer getCurrentCommandBuffer();
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

            void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout srcLayout, VkImageLayout dstLayout, uint32_t mipLevel);
            void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

            VkCommandBuffer beginSingleTimeCommands();
            void endSingleTimeCommands(VkCommandBuffer commandBuffer);

            void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,  VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory );
            void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& memory);
            VkImageView createImageView(VkImage image, VkFormat format, uint32_t mipLevels);
            void generateMipmaps(VkImage image, VkFormat format, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);

            uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

            void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
            
            void createDescriptorSetLayout();
            void createUniformBuffers();
            void createDescriptorPool();
            void createDescriptorSets();


            void createTextureSampler();
            
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

            VkPipeline mDisplayPipeline;

            VkCommandPool mCommandPool;
            std::vector<VkCommandBuffer> mCommandBuffers;

            std::vector<VkFramebuffer> mSwapchainFramebuffers;

            std::vector<VkSemaphore> mImageAvailableSemaphores;
            std::vector<VkSemaphore> mRenderFinishedSemaphores;
            std::vector<VkFence> mInFlightFences;
            std::vector<VkFence> mImagesInFlight;

            VkDescriptorSetLayout mDescriptorSetLayout;
            VkDescriptorSetLayout mTextureSetLayout;

            std::vector<VkBuffer> uniformBuffers;
            std::vector<VkDeviceMemory> uniformBuffersMemory;
            std::vector<void*> uniformBuffersMapped;

            std::vector<VkBuffer> sceneUniformBuffers;
            std::vector<VkDeviceMemory> sceneUniformBuffersMemory;
            std::vector<void*> sceneUniformBuffersMapped;

            VkDescriptorPool mDescriptorPool;
            std::vector<VkDescriptorSet> mUIDescriptorSets;
            std::vector<VkDescriptorSet> mSceneDescriptorSets;
            VkDescriptorSet mTextureDescriptorSet;


            bool  USE_BINDLESS_TXT = false;
            uint32_t MAX_TEXTURES = 0;
            uint32_t mMipLevels;
            //DEBUG
            VkDebugUtilsMessengerEXT mDebugMessenger;

            VkSampler mTextureSampler;

            const int MAXFRAMESINFLIGHT = 2;
            uint32_t currentFrameInFlight = 0;
            uint32_t currentImageIndex = 0;
            bool framebufferResized = false;
        };
    }
}