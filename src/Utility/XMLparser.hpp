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
            Element(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h, glm::vec3 _color)
                : x(_x), y(_y), w(_w), h(_h), color(_color) {}
            
            uint8_t x, y;
            uint8_t w,h;

            glm::vec3 color;
        };


        class Frame : public Element
        {
        public:
            Frame() = default;
            Frame(uint8_t _x, uint8_t _y, uint8_t _w, uint8_t _h, glm::vec3 _color)
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
        private:
            XML() = default;

            util::Logger mLogger = util::Logger("XML parser logger");
        };
    }
}