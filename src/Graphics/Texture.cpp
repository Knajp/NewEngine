#include "Texture.hpp"
#include <filesystem>

void ke::Graphics::Texture::TextureManager::init()
{
    const std::filesystem::path targetPath{"./src/Textures"};

    try
    {
        for(const auto& direntry : std::filesystem::directory_iterator(targetPath))
        {
            createTexture(direntry.path().filename().stem().string(), direntry.path());
        }
    }catch(std::filesystem::filesystem_error& err) {std::cout << err.what() << std::endl;}
}

void ke::Graphics::Texture::TextureManager::createTexture(const std::string &name, const std::string &filepath)
{
    size_t textureIndex = mTextureList.size();

    mTextureList.emplace_back(filepath);
    
    ke::Graphics::Renderer& renderer = ke::Graphics::Renderer::getInstance();

    if(renderer.addTextureToDescriptor(mTextureList[textureIndex].getImage()) != textureIndex)
        std::cout << "Texture index mismatch!";

    std::filesystem::path path(filepath);
    std::string filename = path.stem().string();

    mIndexMap[filename] = textureIndex;
}

uint32_t ke::Graphics::Texture::TextureManager::getTextureIndex(std::string name) const
{
    try
    {
        return mIndexMap.at(name);
    }
    catch(const std::out_of_range& e)
    {
        std::cerr << "Tried to get a non-existant texture's index!" << '\n';
        return 0;
    }
    
    
}

const ke::Graphics::Texture::Texture& ke::Graphics::Texture::TextureManager::getTexture(uint32_t index) const
{
    return mTextureList[index];
}

void ke::Graphics::Texture::TextureManager::terminate()
{
    mTextureList.clear();
}

ke::Graphics::Texture::Texture::Texture(const std::string &filepath)
{
    ke::Graphics::Renderer& renderer = ke::Graphics::Renderer::getInstance();

    renderer.createTextureImage(filepath, mImage);
    renderer.createTextureImageView(mImage);

    mImage.setDevice(renderer.getDevice());
}

const ke::util::Image& ke::Graphics::Texture::Texture::getImage() const
{
    return mImage;
}

VkImageView ke::Graphics::Texture::Texture::getImageView() const
{
    return mImage.imageView;
}
