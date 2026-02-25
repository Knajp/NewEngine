#pragma once
#include "Utility/structs.hpp"
#include "Utility/RenderUtil.hpp"
#include "Graphics/Renderer.hpp"
#include <vector>
#include <vulkan/vulkan.h>

namespace ke
{
    namespace util
    {
    struct Mesh
    {
        util::Buffer indexBuffer;
        util::Buffer vertexBuffer;
        
        std::vector<ke::util::str::Vertex2P3C2T> mVertices;
        std::vector<uint16_t> mIndices;

        Mesh() = default;
        Mesh(const std::vector<util::str::Vertex2P3C2T>& vertices, const std::vector<uint16_t>& indices)
        {
            VkDeviceSize verticesSize = sizeof(vertices[0]) * vertices.size();
            VkDeviceSize indexSize = sizeof(indices[0]) * indices.size();

            ke::Graphics::Renderer& rend = ke::Graphics::Renderer::getInstance();

            indexBuffer.setDevice(rend.getDevice());
            vertexBuffer.setDevice(rend.getDevice());

            rend.createVertexBuffer(vertices, vertexBuffer.buffer, vertexBuffer.bufferMemory);
            rend.createIndexBuffer(indices, indexBuffer.buffer, indexBuffer.bufferMemory);

            mVertices = std::move(vertices);
            mIndices = std::move(indices);
        }
            
        Mesh(const Mesh& other) = delete;
        Mesh& operator=(const Mesh& other) = delete;

        Mesh(Mesh&& other) noexcept
            : indexBuffer(std::move(other.indexBuffer)),
              vertexBuffer(std::move(other.vertexBuffer)),
              mVertices(std::move(other.mVertices)),
              mIndices(std::move(other.mIndices))
        {
            other.mIndices.clear();
            other.mVertices.clear();
            other.indexBuffer.destroy();
            other.vertexBuffer.destroy();
        }
        Mesh& operator=(Mesh&& other) noexcept
        {
            if(this == &other) return *this;
            
            vertexBuffer = std::move(other.vertexBuffer);
            indexBuffer = std::move(other.indexBuffer);

            mVertices = std::move(other.mVertices);
            mIndices = std::move(other.mIndices);

            other.mIndices.clear();
            other.mVertices.clear();
            other.vertexBuffer.destroy();
            other.indexBuffer.destroy();

            return *this;
        }

    };
    }  
    class SceneObject
    {
    public:
        SceneObject() = default;

        void Draw() const;
        void setTexture(std::string textureName);
        void loadMesh(util::Mesh& mesh);

        SceneObject(SceneObject&& other) noexcept
        : mMesh(std::move(other.mMesh)),
          mTextureIndex(other.mTextureIndex)
        {
        }
        ~SceneObject() = default;
    private:
        util::Mesh mMesh;

        uint32_t mTextureIndex = -1;
    };
    class SceneManager
    {
    public:
        static SceneManager& getInstance()
        {
            static SceneManager instance;
            return instance;
        }


        void init(glm::ivec2 pos, glm::ivec2 extent, int windowHeight);
        void drawScene() const;

        float getSceneAspectRatio() const;

        void terminate();

        const VkViewport& getViewport() const;
        const VkRect2D& getScissor() const;

        void addObjectToScene(std::unique_ptr<SceneObject> object);
    private:

        SceneManager() = default;

        VkViewport mSceneViewport;
        VkRect2D mSceneScissor;

        std::vector<std::unique_ptr<SceneObject>> mSceneObjects;
    };
}