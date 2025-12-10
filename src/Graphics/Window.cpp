#include "Window.hpp"

ke::Graphics::Window::Window(uint16_t width, uint16_t height, const char* title)
{
	pWindow = glfwCreateWindow(width, height, title, nullptr, nullptr);
}

ke::Graphics::Window::~Window()
{
	glfwDestroyWindow(pWindow);
}

void ke::Graphics::Window::initGLFW()
{
	glfwInit();
}

void ke::Graphics::Window::exitGLFW()
{
	glfwTerminate();
}

GLFWwindow* ke::Graphics::Window::getWindowHandle() const
{
	return pWindow;
}

void ke::Graphics::Window::pollEvents()
{
	glfwPollEvents();
}

bool ke::Graphics::Window::shouldClose() const
{
	return glfwWindowShouldClose(pWindow);
}
