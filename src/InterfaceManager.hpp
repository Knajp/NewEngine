#include "./Utility/XMLparser.hpp"
#include "./Utility/RenderUtil.hpp"
#include "./Utility/structs.hpp"
#include "./Graphics/Renderer.hpp"
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
            //~Component();
            
            void Draw(VkCommandBuffer commandBuffer);
        private:
            std::vector<GUIObject> mFrames;
            util::Buffer mVertexBuffer;
            util::Buffer mIndexBuffer;

            std::vector<util::str::Vertex2P3C> mVertices;
            std::vector<uint16_t> mIndices;
        };

        class UImanager
        {
        public:
            static UImanager& getInstance() {
                static UImanager instance;
                return instance;
            }
            
            void loadComponents();
            void drawComponents(VkCommandBuffer commandBuffer);
        private:
            UImanager() = default;

            std::vector<Component> mComponents;
        };
    }
}