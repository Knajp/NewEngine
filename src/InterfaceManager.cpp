#include "InterfaceManager.hpp"

ke::gui::Component::Component(std::string filepath)
    : mIndexBuffer(Graphics::Renderer::getInstance().getDevice()), mVertexBuffer(Graphics::Renderer::getInstance().getDevice())
{
    static util::XML parser = util::XML::getInstance();

    parser.parseFile(filepath, mFrames);
    
    int indexAdvance = 0;
    for(ke::GUIObject obj : mFrames)
    {
        gui::Frame frame = std::get<gui::Frame>(obj);

        float x = frame.x / 100;
        x *= 2.0f;
        x -= 1.0f;

        float y = frame.y / 100;
        y *= 2.0f;
        y -= 1.0f;

        float w = frame.w / 100;
        w *= 2.0f;

        float h = frame.h / 100;
        h *= 2.0f;

        mVertices.push_back({{x, y + h}, {frame.color.r, frame.color.g, frame.color.b}});
        mVertices.push_back({{x, y}, {frame.color.r, frame.color.g, frame.color.b}});
        mVertices.push_back({{x + w, y}, {frame.color.r, frame.color.g, frame.color.b}});
        mVertices.push_back({{x + w, y + h}, {frame.color.r, frame.color.g, frame.color.b}});

        mIndices.push_back(0 + indexAdvance);
        mIndices.push_back(1 + indexAdvance);
        mIndices.push_back(2 + indexAdvance);
        mIndices.push_back(0 + indexAdvance);
        mIndices.push_back(2 + indexAdvance);
        mIndices.push_back(3 + indexAdvance);

        indexAdvance += 4;
    }

    VkDeviceSize verticesSize = sizeof(mVertices[0]) * mVertices.size();
    VkDeviceSize indicesSize = sizeof(mIndices[0]) * mIndices.size();

    Graphics::Renderer& rend = Graphics::Renderer::getInstance();

    rend.createVertexBuffer(mVertices, mVertexBuffer.buffer, mVertexBuffer.bufferMemory);
    rend.createIndexBuffer(mIndices, mIndexBuffer.buffer, mIndexBuffer.bufferMemory);
}


ke::gui::Component::~Component()
{
}

ke::gui::Component::Component(Component&& other) noexcept
    : mFrames(std::move(other.mFrames)),
      mIndexBuffer(std::move(other.mIndexBuffer)),
      mVertexBuffer(std::move(other.mVertexBuffer)),
      mIndices(std::move(other.mIndices)),
      mVertices(std::move(other.mVertices))
{
}

ke::gui::Component& ke::gui::Component::operator=(Component&& other) noexcept
{
    if (this != &other)
    {
        mFrames = std::move(other.mFrames);
        mIndexBuffer = std::move(other.mIndexBuffer);
        mVertexBuffer = std::move(other.mVertexBuffer);
        mIndices = std::move(other.mIndices);
        mVertices = std::move(other.mVertices);
    }
    return *this;
}
void ke::gui::Component::Draw(VkCommandBuffer commandBuffer)
{
    VkBuffer vertexBuffers[] = {mVertexBuffer.buffer};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffer, mIndexBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(mIndices.size()), 1, 0, 0, 0);

}

void ke::gui::UImanager::loadComponents()
{
    const std::filesystem::path targetPath{"./src/UI/"};

    try
    {
        for(auto const& direntry : std::filesystem::directory_iterator{targetPath})
        {
            if(std::filesystem::is_regular_file(direntry.path()))
            {
                mComponents.emplace_back(direntry.path().string());
            }
                
        }
    }catch(std::filesystem::filesystem_error const& err)
        {std::cout << "Error while reading directory: " << err.what() << std::endl;}
    
}

void ke::gui::UImanager::drawComponents(VkCommandBuffer commandBuffer)
{
    for(auto& comp : mComponents)
    {
        comp.Draw(commandBuffer);
    }
}

void ke::gui::UImanager::unloadComponents()
{
    vkDeviceWaitIdle(Graphics::Renderer::getInstance().getDevice());
    mComponents.clear();
}
