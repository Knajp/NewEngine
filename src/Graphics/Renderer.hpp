#pragma once

#define GLFW_INCLUDE_VULKAN
#include <glfw3.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "Utility/Logger.hpp"

namespace ke
{
    namespace Graphics
    {
        class Renderer
        {
        public:
            static Renderer& getInstance();
            
            void init();
            void terminate();
        private:
            Renderer() = default;

            void createVulkanInstance();

            
        private:
            util::Logger mLogger("Render Logger", spdlog::level::debug);

            VkInstance mInstance;
        };
    }
}