
#include "Object.hpp"
namespace ke::nodes
{
    class Circle : public Node2D
    {
    public:
        Circle(uint8_t depth, int rad, glm::ivec2 pos)
            : Node2D(depth), radius(rad), position(pos) {}
        
        Circle(uint8_t depth, int rad, int _x, int _y)
            : Node2D(depth), radius(rad), x(_x), y(_y) {}

        int radius;
        union
        {
            glm::ivec2 position;

            struct
            {
                int x;
                int y;
            };
        };
        
    };
}