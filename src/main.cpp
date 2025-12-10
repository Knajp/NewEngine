#include "Application.hpp"

int main(int argc, char** argv)
{
    ke::Core::Application& APPLICATION = ke::Core::Application::getInstance();
    try
    {
        APPLICATION.Run();
    }
    catch (std::runtime_error& e) { std::cerr << e.what() << std::endl; }

}