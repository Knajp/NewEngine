#include "XMLparser.hpp"

void ke::util::XML::parseFile(std::string filepath, std::vector<GUIObject>& elements)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    mLogger.info(result.description());

    pugi::xml_node root = doc.child("KEUIcomponent");
    
    for(pugi::xml_node frame : root.children("Frame"))
    {  
        float x = frame.attribute("x").as_float();
        float y = frame.attribute("y").as_float();
        float w = frame.attribute("w").as_float();
        float h = frame.attribute("h").as_float();

        const char* hexColor = frame.attribute("color").as_string();
        int r, g, b;
        std::sscanf(hexColor, "#%02x%02x%02x", &r, &g, &b);

        float rf = r / 255.0f;
        float gf = g / 255.0f;
        float bf = b / 255.0f;

        elements.push_back(gui::Frame(x,y,w,h, {rf,gf,bf}));
    }
}
