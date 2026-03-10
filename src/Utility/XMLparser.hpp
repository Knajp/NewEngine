#include <string>
#include <pugixml/pugixml.hpp>
#include <iostream>
#include <variant>
#include <type_traits>
#include "Logger.hpp"
#include <glm/glm.hpp>
#include <functional>

namespace ke
{
    namespace gui
    {
        enum class UIType
        {
            TypeFrame, TypeButton
        };
        #define GUI_TYPE(type) static UIType getStaticType() {return UIType::type;} \
            UIType getType() const override {return getStaticType();}

        class Element
        {
        public:
            Element() = default;
            Element(float _x, float _y, float _w, float _h, glm::vec3 _color)
                : x(_x), y(_y), w(_w), h(_h), color(_color) {}
            
            virtual ~Element() = default;

            float x, y;
            float w,h;

            glm::vec3 color;

            virtual UIType getType() const = 0;
        };


        class Frame : public Element
        {
        public:
            Frame() = default;
            Frame(float _x, float _y, float _w, float _h, glm::vec3 _color)
                : Element(_x,_y,_w,_h,_color){}

            GUI_TYPE(TypeFrame)
        };

        class Button : public Element
        {
        public:
            Button() = default;
            Button(float _x, float _y, float _w, float _h, glm::vec3 _color, std::string id)
                : Element(_x, _y, _w, _h, _color), buttonID(id){}

            std::function<void()> onClick;

            std::string buttonID;
            GUI_TYPE(TypeButton)
        };
    };
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

            void parseFile(std::string filepath, std::vector<std::unique_ptr<gui::Element>>& elements, std::vector<gui::Button>& buttons);
            void parseSceneFile(std::string filepath, glm::ivec2& offset, glm::ivec2& extent, int wx, int wy);
        private:
            XML() = default;

            util::Logger mLogger = util::Logger("XML parser logger");
        };
    }
}