#include <string>
#include <pugixml/pugixml.hpp>
#include <iostream>
#include <variant>
#include <type_traits>
#include "Logger.hpp"

namespace ke
{
    namespace gui
    {
        class Element
        {
        public:
            Element() = default;
            Element(int _x, int _y, int _w, int _h, std::string _color)
                : x(_x), y(_y), w(_w), h(_h), color(_color) {}
            int x,y;
            int w,h;
            std::string color;

        };

        class Frame : public Element
        {
        public:
            Frame() = default;
            Frame(int _x, int _y, int _w, int _h, std::string _color)
                : Element(_x,_y,_w,_h,_color){}

        };
    };
    using GUIObject = std::variant<ke::gui::Element, ke::gui::Frame>;
    namespace util
    {
        
        enum XMLVALTYPE 
        {
            INTEGER, PERCENT, STRING, BOOL, MAX_ENUM
        };
        
        struct Value
        {
            XMLVALTYPE type;
            std::variant<int, std::string, bool> data;

            template<typename T>
            void set(T val);
            bool percentify();
        };

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

        template <typename T>
        inline void Value::set(T val)
        {
            if constexpr(std::is_same_v<T, int>)
            {
                data = val;
                type = XMLVALTYPE::INTEGER;
            }
            else if constexpr(std::is_same_v<T, std::string>)
            {
                data = val;
                type = XMLVALTYPE::STRING;
            }
            else if constexpr(std::is_same_v<T,bool>)
            {
                data = val;
                type = XMLVALTYPE::BOOL;
            }
            
        }
    }
}