#include "InterfaceManager.hpp"

ke::gui::Component::Component(std::string filepath)
{
    static util::XML parser = util::XML::getInstance();

    parser.parseFile(filepath, mFrames);
    
    
    gui::Frame frame = std::get<gui::Frame>(mFrames[0]);

    std::cout << "First element x: " << frame.x;
    std::cout << "\nFirst element y:" << frame.y;
    std::cout << "\nFirst element w:" << frame.w;
    std::cout << "\nFirst element h" << frame.h;
    
}

void ke::gui::UImanager::loadComponents()
{
    const std::filesystem::path targetPath{"./src/UI/"};

    try
    {
        for(auto const& direntry : std::filesystem::directory_iterator{targetPath})
        {
            if(std::filesystem::is_regular_file(direntry.path()))
                mComponents.push_back(gui::Component(direntry.path().string()));
        }
    }catch(std::filesystem::filesystem_error const& err)
        {std::cout << "Error while reading directory: " << err.what() << std::endl;}
    
}
