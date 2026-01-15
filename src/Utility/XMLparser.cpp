#include "XMLparser.hpp"
#include "RenderUtil.hpp"

void ke::util::XML::parseFile(std::string filepath, std::vector<GUIObject>& elements)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    mLogger.info(result.description());

    pugi::xml_node root = doc.child("KEUIcomponent");
    
    float rootx = root.attribute("x").as_float() / 100.0f;
    float rooty = root.attribute("y").as_float() / 100.0f;
    float rootw = root.attribute("w").as_float() / 100.0f;
    float rooth = root.attribute("h").as_float() / 100.0f;

    for(pugi::xml_node frame : root.children("Frame"))
    {  
        float x = frame.attribute("x").as_float() / 100.0f;
        float y = frame.attribute("y").as_float() / 100.0f;
        float w = frame.attribute("w").as_float() / 100.0f;
        float h = frame.attribute("h").as_float() / 100.0f;

        x = rootx + rootw * x;
        y = rooty + rooth * y;
        w *= rootw;
        h *= rooth;

        const char* hexColor = frame.attribute("color").as_string();
        int r, g, b;
        std::sscanf(hexColor, "#%02x%02x%02x", &r, &g, &b);

        float rf = util::srgbToLinear(r / 255.0f);
        float gf = util::srgbToLinear(g / 255.0f);
        float bf = util::srgbToLinear(b / 255.0f);

        elements.push_back(gui::Frame(x,y,w,h, {rf,gf,bf}));
    }
}
