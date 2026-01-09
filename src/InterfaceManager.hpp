#include "./Utility/XMLparser.hpp"
#include <filesystem>
#include <vector>
namespace ke
{
    namespace gui
    {

        
        class Component
        {
        public:
            Component() = default;
            Component(std::string filepath);

        private:
            std::vector<GUIObject> mFrames;
        };

        class UImanager
        {
        public:
            static UImanager& getInstance() {
                static UImanager instance;
                return instance;
            }
            
            void loadComponents();
        private:
            UImanager() = default;

            std::vector<Component> mComponents;
        };
    }
}