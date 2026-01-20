#include "./Utility/XMLparser.hpp"
#include "./Utility/RenderUtil.hpp"
#include "./Utility/structs.hpp"
#include "./Graphics/Renderer.hpp"
#include <GLFW/glfw3.h>
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
            ~Component();

            Component(Component&& other) noexcept;
            ke::gui::Component& operator=(Component&& other) noexcept;
            
            void Draw(VkCommandBuffer commandBuffer);
        private:
            std::vector<GUIObject> mFrames;
            util::Buffer mVertexBuffer;
            util::Buffer mIndexBuffer;

            std::vector<util::str::Vertex2P3C> mVertices;
            std::vector<uint16_t> mIndices;
        };

        class SceneComponent
        {
        public:
            SceneComponent() = default;
            SceneComponent(std::string filepath, GLFWwindow* window);
            ~SceneComponent() = default;

            SceneComponent(SceneComponent&& other) noexcept;
            ke::gui::SceneComponent& operator=(SceneComponent&& other) noexcept;

            SceneComponent(const SceneComponent& other) = delete;
            ke::gui::SceneComponent& operator=(const SceneComponent& other) = delete;
            
            glm::ivec2 pos;
            glm::ivec2 extent; 
        };

        class UImanager
        {
        public:
            static UImanager& getInstance() {
                static UImanager instance;
                return instance;
            }
            
            void loadComponents(GLFWwindow* window);
            void drawComponents(VkCommandBuffer commandBuffer);
            void unloadComponents();

            glm::ivec2 getSceneComponentPosition() const;
            glm::ivec2 getSceneComponentExtent() const;
        private:
            UImanager() = default;

            std::vector<Component> mComponents;
            SceneComponent mSceneComponent;
        };
    }
}