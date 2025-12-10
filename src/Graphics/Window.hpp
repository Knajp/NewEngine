#include<iostream>
#include <GLFW/glfw3.h>

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

		private:
			GLFWwindow* pWindow;

		};
	}
}