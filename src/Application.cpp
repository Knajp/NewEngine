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
    mLogger.info("Initialized GLFW.");
    mMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* videoMode = glfwGetVideoMode(mMonitor);
    mWindow = std::make_unique<Graphics::Window>(videoMode->width, videoMode->height, "Knajp's Engine");
    mLogger.info("Created window.");

    mLogger.trace("Requesting renderer init.");
    mRenderer.init(mWindow->getWindowHandle());
    mLogger.trace("Finished initializing renderer.");

    mLogger.trace("Requesting UI manager load");
    mUIManager.loadComponents();
    mLogger.info("Finished loading UI manager.");

    mLogger.info("Finished application initialization.");
}

void ke::Core::Application::run()
{
    mLogger.trace("Proceeding to main loop.");

    while (!mWindow->shouldClose())
    {
        mWindow->calculateAspectRatio();
        mRenderer.readyCanvas(mWindow->getWindowHandle());

        mRenderer.updateDemoUniforms(mWindow->getAspectRatio());
        mRenderer.drawDemo();

        mRenderer.finishDraw(mWindow->getWindowHandle());
        Graphics::Window::pollEvents();
    }
    
    mLogger.info("Exit main loop.");
}

void ke::Core::Application::terminate()
{
    mLogger.trace("Proceeding to termination.");

    mLogger.trace("Requesting renderer termination.");
    mRenderer.terminate();
    mLogger.trace("Finished terminating renderer.");

    Graphics::Window::exitGLFW();
    mLogger.info("Exit GLFW.");
    
    mLogger.info("Goodbye.");
}
