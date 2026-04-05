#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H
#include <msdfgen/msdfgen.h>
#include <msdfgen/msdfgen-ext.h>
#include <iostream>
#include <filesystem>
#include <unordered_map>
#include <stb/stb_rect_pack.h>
#include "../Utility/RenderUtil.hpp"

namespace ke
{
    namespace Graphics
    {
        namespace Text
        {
            class TextUtils;

            struct GlyphInfo
            {
                uint32_t atlasX, atlasY;
                uint32_t width, height;
                int bearingX, bearingY;
                float advance;
                float u0, v0, u1, v1;
                uint16_t internalCodepoint;
                std::vector<float> pixels;
            };


            struct GlyphInstance
            {
                glm::vec2 position;
                glm::vec2 size;
                glm::vec4 uv;
                glm::vec4 color;

                static std::array<VkVertexInputAttributeDescription, 4> getInputAttributeDescriptions()
                {
                    std::array<VkVertexInputAttributeDescription, 4> descs;
                    
                    descs[0].binding = 0;
                    descs[0].location = 0;
                    descs[0].format = VK_FORMAT_R32G32_SFLOAT;
                    descs[0].offset = offsetof(GlyphInstance, position);

                    descs[1].binding = 0;
                    descs[1].location = 1;
                    descs[1].format = VK_FORMAT_R32G32_SFLOAT;
                    descs[1].offset = offsetof(GlyphInstance, size);

                    descs[2].binding = 0;
                    descs[2].location = 2;
                    descs[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    descs[2].offset = offsetof(GlyphInstance, uv);

                    descs[3].binding = 0;
                    descs[3].location = 3;
                    descs[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
                    descs[3].offset = offsetof(GlyphInstance, color);
                    
                    return descs;
                }

                static VkVertexInputBindingDescription getInputBindingDescription()
                {
                    VkVertexInputBindingDescription desc{};
                    desc.binding = 0;
                    desc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
                    desc.stride = sizeof(GlyphInstance);

                    return desc;
                }
            };

            class Font
            {
                friend class TextUtils;
            public:
                Font() = default;
                Font(const std::string& filepath, msdfgen::FreetypeHandle* lib);
                ~Font();
                
                void rasterizeGlyphs(int min, int max);
                GlyphInfo& getGlyphInfo(uint32_t codepoint);
                uint32_t getDescriptorIndex() const;
            private:
                msdfgen::FontHandle* mFontHandle = nullptr;
                std::unordered_map<uint32_t, GlyphInfo> mGlyphs;
                static const unsigned int ATLAS_SIZE = 2048;

                util::Image mImage;
                uint32_t mDescriptorIndex;
            };

            

            class TextUtils
            {
            public:
                static TextUtils& getInstance()
                {
                    static TextUtils instance;
                    return instance;
                }
                
                void init();
                void terminate();
                
                GlyphInfo rasterizeGlyph(msdfgen::FontHandle* font, uint32_t codepoint);

                ke::Graphics::Text::Font& getFont(const std::string& fontname);
            private:
                TextUtils() = default;


                msdfgen::FreetypeHandle* ft = nullptr;

                std::unordered_map<std::string, std::unique_ptr<Font>> mFonts;
                static const unsigned int GLYPH_SIZE = 32;

            };

            class TextInstance
            {
            public:
                TextInstance(const std::string& text, const std::string& font, int x, int y, glm::vec4 color);
               
                void Draw() const;

            private:
                std::vector<GlyphInstance> mInstances;
                util::Buffer mInstanceBuffer;
                uint32_t mFontIndex;
            };
            
        }
    }
}