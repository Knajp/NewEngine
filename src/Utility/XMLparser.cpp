#include "XMLparser.hpp"

void ke::util::XML::parseFile(std::string filepath, std::vector<GUIObject>& elements)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    mLogger.info(result.description());

    pugi::xml_node root = doc.child("KEUIcomponent");
    
    for(pugi::xml_node frame : root.children("Frame"))
    {  
        uint8_t x = frame.attribute("x").as_uint();
        uint8_t y = frame.attribute("y").as_uint();
        uint8_t w = frame.attribute("w").as_uint();
        uint8_t h = frame.attribute("h").as_uint();

        const char* hexColor = frame.attribute("color").as_string();
        int r, g, b;
        std::sscanf(hexColor, "#%02x%02x%02x", &r, &g, &b);


        elements.push_back(gui::Frame(x,y,w,h, {r,g,b}));
    }
}
