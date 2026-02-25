#pragma once

#include "../Utility/RenderUtil.hpp"
#include "Renderer.hpp"

namespace ke
{
    namespace Graphics
    {
        namespace Texture
        {
            class Texture
            {
            public:
                Texture() = default;
                Texture(const std::string& filepath);

                const util::Image& getImage() const;
                VkImageView getImageView() const;

                ~Texture() = default;

                Texture(Texture&&) noexcept = default;
                Texture& operator=(Texture&&) noexcept = default;

                Texture(const Texture&) = delete;
                Texture& operator=(const Texture&) = delete;
            private:
                util::Image mImage;
            };

            class TextureManager
            {
            public:
                static TextureManager& getInstance()
                {
                    static TextureManager instance;
                    return instance;
                }

                void init();


                void createTexture(const std::string& name, const std::string& filepath);
                uint32_t getTextureIndex(std::string name) const;
                const Texture& getTexture(uint32_t index) const;

                void terminate();

            private:
                TextureManager() = default;

                std::vector<Texture> mTextureList;
                std::unordered_map<std::string, uint32_t> mIndexMap;
                
            };
        }
    }
}