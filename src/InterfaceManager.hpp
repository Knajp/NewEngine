#include "./Utility/XMLparser.hpp"
#include "./Utility/RenderUtil.hpp"
#include "./Utility/structs.hpp"
#include "./Graphics/Renderer.hpp"
#include <GLFW/glfw3.h>
#include <filesystem>
#include <vector>
#include <memory>

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

            bool pollButtonClick(int mouseX, int mouseY, int windowX, int windowY);

            Component(Component&& other) noexcept;
            ke::gui::Component& operator=(Component&& other) noexcept;
            
            void Draw(VkCommandBuffer commandBuffer);
        private:
            std::vector<std::unique_ptr<gui::Element>> mFrames;
            std::vector<gui::Button> mButtons;

            util::Buffer mVertexBuffer;
            util::Buffer mIndexBuffer;

            std::vector<util::str::Vertex2P3C2T> mVertices;
            std::vector<uint16_t> mIndices;

            static std::unordered_map<std::string, std::function<void()>> mHandlers;
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

            void recreateSceneComponent(GLFWwindow* window);

            glm::ivec2 getSceneComponentPosition() const;
            glm::ivec2 getSceneComponentExtent() const;

            void processMouseClick(int mouseX, int mouseY, int windowX, int windowY);
        private:
            UImanager() = default;

            std::vector<Component> mComponents;
            SceneComponent mSceneComponent;

            std::string sceneComponentFilepath;
        };
    }
}