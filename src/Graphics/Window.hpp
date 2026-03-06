#include<iostream>
#include <GLFW/glfw3.h>
#include <functional>

#include "../Utility/Logger.hpp"
#include "../Events/event_pch.hpp"

namespace ke
{
	namespace Graphics
	{
		class Window
		{
		public:
			Window() = default;
			Window(uint16_t width, uint16_t height, const char* title);
			~Window();

			static void initGLFW();
			static void exitGLFW();

			GLFWwindow* getWindowHandle() const;

			static void pollEvents();

			bool shouldClose() const;
			void quit();

			void setApplicationEventCallback(void(*func_ptr)(Events::Event& e));

			float getAspectRatio() const;
			void calculateAspectRatio();

			void event(Events::Event&);
			bool isKeyPressed(int key) const;
		private:
			GLFWwindow* pWindow;

			float aspectRatio;
			std::function<void(Events::Event&)> mEventCallback;
		};
	}
}