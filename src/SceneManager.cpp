#include "SceneManager.hpp"
#include "./Graphics/Texture.hpp"

void ke::SceneManager::init(glm::ivec2 pos, glm::ivec2 extent, int windowHeight)
{

    mSceneViewport.height = extent.y;
    mSceneViewport.width = extent.x;
    mSceneViewport.x = pos.x;
    mSceneViewport.y = windowHeight - (pos.y + extent.y);
    mSceneViewport.minDepth = 0.0f;
    mSceneViewport.maxDepth = 1.0f;

    mSceneScissor.extent = {static_cast<uint32_t>(extent.x), static_cast<uint32_t>(extent.y)};
    mSceneScissor.offset = {pos.x, windowHeight - (pos.y + extent.y)};
}

void ke::SceneManager::drawScene() const
{
    for(const auto& object : mSceneObjects)
    {
        object->Draw();
    }
}

float ke::SceneManager::getSceneAspectRatio() const
{
    return mSceneViewport.width / mSceneViewport.height;
}

void ke::SceneManager::terminate()
{
   mSceneObjects.clear();
}

const VkViewport &ke::SceneManager::getViewport() const
{
    return mSceneViewport;
}

const VkRect2D &ke::SceneManager::getScissor() const
{
    return mSceneScissor;
}

void ke::SceneManager::addObjectToScene(std::unique_ptr<SceneObject> object)
{
    mSceneObjects.push_back(std::move(object));
}

void ke::SceneObject::Draw() const
{
    ke::Graphics::Renderer& renderer = ke::Graphics::Renderer::getInstance();

    renderer.pickTextureIndex(mTextureIndex);
    
    renderer.drawBuffersIndexed(mMesh.vertexBuffer, mMesh.indexBuffer, mMesh.mIndices.size());
}

void ke::SceneObject::setTexture(std::string textureName)
{
    ke::Graphics::Texture::TextureManager& textureManager = ke::Graphics::Texture::TextureManager::getInstance();

    mTextureIndex = textureManager.getTextureIndex(textureName);
}

void ke::SceneObject::loadMesh(util::Mesh &mesh)
{
    mMesh = std::move(mesh);
}

