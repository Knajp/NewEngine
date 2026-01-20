#include <string>
#include <pugixml/pugixml.hpp>
#include <iostream>
#include <variant>
#include <type_traits>
#include "Logger.hpp"
#include <glm/glm.hpp>

namespace ke
{
    namespace gui
    {
        class Element
        {
        public:
            Element() = default;
            Element(float _x, float _y, float _w, float _h, glm::vec3 _color)
                : x(_x), y(_y), w(_w), h(_h), color(_color) {}
            
            float x, y;
            float w,h;

            glm::vec3 color;
        };


        class Frame : public Element
        {
        public:
            Frame() = default;
            Frame(float _x, float _y, float _w, float _h, glm::vec3 _color)
                : Element(_x,_y,_w,_h,_color){}

        };
    };
    using GUIObject = std::variant<ke::gui::Element, ke::gui::Frame>;
    namespace util
    {

        class XML
        {
        public:
            static XML& getInstance()
            {
                static XML instance;
                return instance;
            }

            void parseFile(std::string filepath, std::vector<GUIObject>& elements);
            void parseSceneFile(std::string filepath, glm::ivec2& offset, glm::ivec2& extent, int wx, int wy);
        private:
            XML() = default;

            util::Logger mLogger = util::Logger("XML parser logger");
        };
    }
}