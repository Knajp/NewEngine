#include "XMLparser.hpp"



void ke::util::XML::parseFile(std::string filepath, std::vector<GUIObject>& elements)
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file(filepath.c_str());

    mLogger.info(result.description());

    pugi::xml_node root = doc.child("KEUIcomponent");
    
    for(pugi::xml_node frame : root.children("Frame"))
    {  
        Value x;
        x.set(frame.attribute("x").as_string());
        if(!x.percentify())
            x.set(frame.attribute("x").as_int());

        Value y;
        y.set(frame.attribute("y").as_string());
        if(!y.percentify())
            y.set(frame.attribute("y").as_int());

        Value w;
        w.set(frame.attribute("w").as_string());
        if(!w.percentify())
            w.set(frame.attribute("w").as_int());

        Value h;
        h.set(frame.attribute("h").as_string());
        if(!h.percentify())
            h.set(frame.attribute("h").as_int());

        Value color;
        color.set(frame.attribute("color").as_string());

        elements.push_back(gui::Frame(std::get<int>(x.data), std::get<int>(y.data), std::get<int>(w.data), std::get<int>(h.data), std::get<std::string>(color.data)));
    }
}

bool ke::util::Value::percentify()
{
    if(auto* s = std::get_if<std::string>(&data))
        {
            if(auto pos = s->find('%'); pos != std::string::npos)
            {
                s->erase(pos);
                int value = std::stoi(*s);

                set(value);
                type = PERCENT;

                return true;
            }
        }
    return false;
}
