#pragma once

#include "Object.hpp"

namespace ke::nodes
{
    class Rect2D : Node2D
    {
    public:
        Rect2D(uint8_t depth, glm::ivec2 pos, glm::ivec2 ext)
            : Node2D(depth), position(pos), extent(ext) {}

        Rect2D(uint8_t depth, int _x, int _y, int _width, int _height)
            : Node2D(depth), x(_x), y(_y), width(_width), height(_height) {}
        
        union
        {
            glm::ivec2 position;
            struct
            {
                int x;
                int y;
            };
        };

        union
        {
            glm::ivec2 extent;
            struct
            {
                int width;
                int height;
            };
        };
    };
}

