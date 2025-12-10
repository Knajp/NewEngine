#include "Graphics/Window.hpp"

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

			void init();
			void run();
			void terminate();
		};
	}
}