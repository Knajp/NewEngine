#include <optional>
#include <fstream>

namespace ke
{
    namespace util
    {
        struct QueueFamilyIndices
        {
            std::optional<uint32_t> graphicsFamily;
            std::optional<uint32_t> presentFamily;
            std::optional<uint32_t> transferFamily;

            bool isComplete()
            {
                return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
            }
        };

        struct SwapchainSupportDetails
        {
            VkSurfaceCapabilitiesKHR surfaceCapabilities;
            std::vector<VkSurfaceFormatKHR> surfaceFormats;
            std::vector<VkPresentModeKHR> presentModes;
        };

        static std::vector<char> readFile(const std::string& filename)
        {
            std::ifstream file(filename, std::ios::ate | std::ios::binary);

            if(!file.is_open())
                throw std::runtime_error("Failed to open file for read!");

            size_t fileSize = (size_t) file.tellg();
            std::vector<char> buffer(fileSize);

            file.seekg(0);
            file.read(buffer.data(), fileSize);

            file.close();

            return buffer;
        }
    }
}