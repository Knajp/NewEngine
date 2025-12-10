#include "Application.hpp"

ke::Core::Application& ke::Core::Application::getInstance()
{
    static ke::Core::Application instance;
    return instance;
}

void ke::Core::Application::Run()
{
    init();
    run();
    terminate();
}

void ke::Core::Application::init()
{
    Graphics::Window::initGLFW();
    mWindow = std::make_unique<Graphics::Window>(800, 800, "Hello, World!");
}

void ke::Core::Application::run()
{
    while (!mWindow->shouldClose())
    {
        Graphics::Window::pollEvents();
    }
}

void ke::Core::Application::terminate()
{
    Graphics::Window::exitGLFW();
}
