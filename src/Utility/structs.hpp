#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>
#include <array>

namespace ke
{
    namespace util
    {
        namespace str
        {
            struct Vertex2P3C
            {
                glm::vec2 pos;
                glm::vec3 col;

                static std::array<VkVertexInputAttributeDescription, 2> getInputAttributeDescriptions()
                {
                    std::array<VkVertexInputAttributeDescription, 2> descs;
                    
                    descs[0].binding = 0;
                    descs[0].location = 0;
                    descs[0].format = VK_FORMAT_R32G32_SFLOAT;
                    descs[0].offset = offsetof(Vertex2P3C, pos);

                    descs[1].binding = 0;
                    descs[1].location = 1;
                    descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                    descs[1].offset = offsetof(Vertex2P3C, col);

                    return descs;
                }

                static VkVertexInputBindingDescription getInputBindingDescription()
                {
                    VkVertexInputBindingDescription desc{};
                    desc.binding = 0;
                    desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                    desc.stride = sizeof(Vertex2P3C);

                    return desc;
                }
            };
        }
        
    }
}