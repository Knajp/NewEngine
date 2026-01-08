#include <pugixml/pugixml.hpp>

namespace ke
{
    namespace gui
    {
        class Component
        {
            
        };

        class UImanager
        {
        public:
            static UImanager& getInstance() {
                static UImanager instance;
                return instance;
            }
            
        private:
            UImanager();
        };
    }
}