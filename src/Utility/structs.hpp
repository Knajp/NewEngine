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
            struct Vertex2P3C2T
            {
                glm::vec2 pos;
                glm::vec3 col;
                glm::vec2 uv;

                static std::array<VkVertexInputAttributeDescription, 3> getInputAttributeDescriptions()
                {
                    std::array<VkVertexInputAttributeDescription, 3> descs;
                    
                    descs[0].binding = 0;
                    descs[0].location = 0;
                    descs[0].format = VK_FORMAT_R32G32_SFLOAT;
                    descs[0].offset = offsetof(Vertex2P3C2T, pos);

                    descs[1].binding = 0;
                    descs[1].location = 1;
                    descs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                    descs[1].offset = offsetof(Vertex2P3C2T, col);

                    descs[2].binding = 0;
                    descs[2].location = 2;
                    descs[2].format = VK_FORMAT_R32G32_SFLOAT;
                    descs[2].offset = offsetof(Vertex2P3C2T, uv);

                    return descs;
                }

                static VkVertexInputBindingDescription getInputBindingDescription()
                {
                    VkVertexInputBindingDescription desc{};
                    desc.binding = 0;
                    desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
                    desc.stride = sizeof(Vertex2P3C2T);

                    return desc;
                }
            };
        }
        
    }
}