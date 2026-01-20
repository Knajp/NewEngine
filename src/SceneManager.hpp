#pragma once
#include "Utility/structs.hpp"
#include "Utility/RenderUtil.hpp"
#include "Graphics/Renderer.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace ke
{
    class SceneManager
    {
    public:
        static SceneManager& getInstance()
        {
            static SceneManager instance;
            return instance;
        }

        void drawDemo(VkCommandBuffer buffer);
        void init(glm::ivec2 pos, glm::ivec2 extent, int windowHeight);

        const VkViewport& getViewport() const;
        const VkRect2D& getScissor() const;
    private:

        SceneManager() = default;

        VkViewport mSceneViewport;
        VkRect2D mSceneScissor;

        util::Buffer vertexBuffer;


        std::vector<util::str::Vertex2P3C> vertices =
        {
            {{0.0f, 0.5f}, {1.0f, 0.0f, 0.0f}},
            {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}
        };
    };
}