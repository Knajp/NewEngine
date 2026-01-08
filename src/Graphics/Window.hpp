#include<iostream>
#include <GLFW/glfw3.h>
#include "../Utility/Logger.hpp"

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

			float getAspectRatio() const;
			void calculateAspectRatio();
		private:
			GLFWwindow* pWindow;

			float aspectRatio;
		};
	}
}