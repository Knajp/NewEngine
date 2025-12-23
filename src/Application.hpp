#include "Graphics/Window.hpp"
#include "Graphics/Renderer.hpp"
#include <memory>

namespace ke
{
	namespace Core
	{
		class Application
		{
		public:
			static Application& getInstance();

			void Run();
		private:
			Application() = default;

			std::unique_ptr<Graphics::Window> mWindow;
			Graphics::Renderer& mRenderer = Graphics::Renderer::getInstance();

			util::Logger mLogger = util::Logger("Main Application Logger");

			void init();
			void run();
			void terminate();
		};
	}
}