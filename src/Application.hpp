#include "Graphics/Window.hpp"
#include "Graphics/Renderer.hpp"
#include "InterfaceManager.hpp"
#include "SceneManager.hpp"
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
			gui::UImanager& mUIManager = gui::UImanager::getInstance();
			SceneManager& mSceneManager = SceneManager::getInstance();
			
			util::Logger mLogger = util::Logger("Main Application Logger");

			GLFWmonitor* mMonitor = nullptr;

			void init();
			void run();
			void terminate();
		};
	}
}