#include "SceneManager.hpp"

void ke::SceneManager::drawDemo(VkCommandBuffer buffer)
{
    VkBuffer buffers[] = {vertexBuffer.buffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(buffer, 0, 1, buffers, offsets);
    vkCmdDraw(buffer, 3, 1, 0, 0);
}

void ke::SceneManager::init(glm::ivec2 pos, glm::ivec2 extent, int windowHeight)
{
    Graphics::Renderer::getInstance().createVertexBuffer(vertices, vertexBuffer.buffer, vertexBuffer.bufferMemory);

    mSceneViewport.height = extent.y;
    mSceneViewport.width = extent.x;
    mSceneViewport.x = pos.x;
    mSceneViewport.y = windowHeight - (pos.y + extent.y);
    mSceneViewport.minDepth = 0.0f;
    mSceneViewport.maxDepth = 1.0f;

    mSceneScissor.extent = {static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y)};
    mSceneScissor.offset = {pos.x, windowHeight - (pos.y + extent.y)};
}

const VkViewport &ke::SceneManager::getViewport() const
{
    return mSceneViewport;
}

const VkRect2D &ke::SceneManager::getScissor() const
{
    return mSceneScissor;
}
