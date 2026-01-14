#pragma once

#include <vulkan/vulkan.h>
#include <optional>
#include <fstream>
#include <glm/glm.hpp>
#include <utility>
#include <iostream>

namespace ke
{
    namespace util 
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> transferFamily;

            bool isComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
            }
        };

        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR surfaceCapabilities;
            std::vector<VkSurfaceFormatKHR> surfaceFormats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        static std::vector<char> readFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if(!file.is_open())
                throw std::runtime_error("Failed to open file for read!");

            size_t fileSize = (size_t) file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }

        struct UniformBufferObject
        {
            glm::mat4 model;
            glm::mat4 view;
            glm::mat4 proj;
        };

        struct Buffer
        {
            VkBuffer buffer;
            VkDeviceMemory bufferMemory;
            VkDevice device;

            Buffer() = default;
            Buffer(VkDevice _device) 
                : device(_device){}
            ~Buffer()
            {
                assert(device != VK_NULL_HANDLE);
                if(buffer == VK_NULL_HANDLE) return;
                
                vkDestroyBuffer(device, buffer, nullptr);
                vkFreeMemory(device, bufferMemory, nullptr);
            }

            Buffer(Buffer&& other)
                : buffer(std::exchange(other.buffer, VK_NULL_HANDLE)),
                  bufferMemory(std::exchange(other.bufferMemory, VK_NULL_HANDLE)){}
            Buffer& operator=(Buffer&& other)
            {
                if(this == &other) return *this;

                buffer = std::exchange(other.buffer, VK_NULL_HANDLE);
                bufferMemory = std::exchange(other.bufferMemory, VK_NULL_HANDLE);


                return *this;
            }

            Buffer(const Buffer& other) = delete;
            Buffer operator=(const Buffer& other) = delete;
        };
    }
}