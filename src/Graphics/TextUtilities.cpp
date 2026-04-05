#define STB_RECT_PACK_IMPLEMENTATION
#include "TextUtilities.hpp"

#include "Renderer.hpp"
void ke::Graphics::Text::TextUtils::init()
{
    ft = msdfgen::initializeFreetype();

    const std::filesystem::path targetPath("./src/Fonts");

    try
    {
        for(auto const& direntry : std::filesystem::directory_iterator(targetPath))
        {
            if(!std::filesystem::is_regular_file(direntry.path())) continue;

            mFonts.emplace(direntry.path().filename().stem(), std::make_unique<Font>(direntry.path(), ft));
        }
    }
    catch(const std::filesystem::filesystem_error& e)
    {
        std::cerr << e.what() << '\n';
    }
    

}

void ke::Graphics::Text::TextUtils::terminate()
{
    mFonts.clear();
}

ke::Graphics::Text::GlyphInfo ke::Graphics::Text::TextUtils::rasterizeGlyph(msdfgen::FontHandle *font, uint32_t codepoint)
{
    msdfgen::Shape shape;
    double advance;
    
    msdfgen::GlyphIndex glyphIndex;
    msdfgen::getGlyphIndex(glyphIndex, font, codepoint);
    msdfgen::loadGlyph(shape, font, glyphIndex, &advance);

    shape.normalize();
    msdfgen::edgeColoringSimple(shape, 3.0);

    msdfgen::Shape::Bounds bounds = shape.getBounds();
    double l = bounds.l, b = bounds.b, r = bounds.r, t = bounds.t;

    msdfgen::FontMetrics metrics;
    msdfgen::getFontMetrics(metrics, font);
    double scale = (GLYPH_SIZE - 4) / metrics.emSize;

    int width = (int)std::ceil((r - l) * scale) +4;
    int height = (int)std::ceil((t - b) * scale) + 4;
    width = std::max(width, 1);
    height = std::max(height, 1);

    msdfgen::Vector2 translate(-l + 2.0 / scale, -b + 2.0 / scale);
    msdfgen::Bitmap<float, 3> bitmap(width, height);

    msdfgen::generateMSDF(bitmap, shape, msdfgen::Projection(scale, translate), 64.0);

    GlyphInfo info;
    info.width = width;
    info.height = height;
    info.advance = (float)(advance * scale);
    info.bearingX = (int)(bounds.l * scale) - 2;
    info.bearingY = (int)(bounds.t * scale) + 2;
    info.internalCodepoint = codepoint;

    if(codepoint == 'L' || codepoint == 'o')
        printf("%c bounds: l=%f b=%f r=%f t=%f scale=%f\n",codepoint, l, b, r, t, scale);

    info.pixels.resize(info.width * info.height * 4); // RGBA across every pixel

    for(int y = 0; y < info.height; y++)
        for(int x = 0; x < info.width; x++)
        {
            const float* px = bitmap(x, y);

            int i = (y * info.width + x) * 4;
            info.pixels[i + 0] = px[0];
            info.pixels[i + 1] = px[1];
            info.pixels[i + 2] = px[2];
            info.pixels[i + 3] = 255;
        }

    return info;
}

ke::Graphics::Text::Font& ke::Graphics::Text::TextUtils::getFont(const std::string &fontname)
{
    return *mFonts[fontname];
}

ke::Graphics::Text::Font::Font(const std::string &filepath, msdfgen::FreetypeHandle* lib)
{
    mFontHandle = msdfgen::loadFont(lib, filepath.c_str());

    rasterizeGlyphs(32, 128);

    
}

ke::Graphics::Text::Font::~Font()
{
    mImage.destroy();
}

void ke::Graphics::Text::Font::rasterizeGlyphs(int min, int max)
{
    TextUtils& textutils = TextUtils::getInstance();
    for(uint32_t cp = min; cp < max; cp++)
    {
        mGlyphs[cp] = textutils.rasterizeGlyph(mFontHandle, cp);

    }

    stbrp_context packCtx;
    stbrp_node nodes[ATLAS_SIZE];
    stbrp_init_target(&packCtx, ATLAS_SIZE, ATLAS_SIZE, nodes, ATLAS_SIZE);

    std::vector<stbrp_rect> rects;
    for(auto& [codepoint, glyph] : mGlyphs)
    {
        rects.push_back({(int)codepoint, (int)glyph.width, (int)glyph.height});
    }

    stbrp_pack_rects(&packCtx, rects.data(), rects.size());

    for(auto& rect : rects)
    {
        auto& glyph = mGlyphs[rect.id];

        if(rect.id == 'H')
            std::cout << "H params, rectX: " << rect.x << ", rectY: " << rect.y << ", Internal Codepoint: " << (char)glyph.internalCodepoint << "\n";

        glyph.atlasX = rect.x;
        glyph.atlasY = rect.y;
        glyph.u0 = rect.x / (float)ATLAS_SIZE;
        glyph.v0 = rect.y / (float)ATLAS_SIZE;
        glyph.u1 = (rect.x + rect.w) / (float)ATLAS_SIZE;
        glyph.v1 = (rect.y + rect.h) / (float)ATLAS_SIZE;
    }

    Renderer& rend = Renderer::getInstance();
    mImage.setDevice(rend.getDevice());
    rend.createFontImage(mGlyphs, ATLAS_SIZE, mImage.image, mImage.imageMemory);
    rend.createFontImageView(mImage);
    mDescriptorIndex = rend.addFontToDescriptor(mImage);
}

ke::Graphics::Text::GlyphInfo &ke::Graphics::Text::Font::getGlyphInfo(uint32_t codepoint)
{
    if (codepoint == 'H')
        printf("getGlyphInfo FOR Hx u0=%f atlasX=%u\n", mGlyphs[codepoint].u0, mGlyphs[codepoint].atlasX);
    return mGlyphs.at(codepoint);
}

uint32_t ke::Graphics::Text::Font::getDescriptorIndex() const
{
    return mDescriptorIndex;
}

ke::Graphics::Text::TextInstance::TextInstance(const std::string &text, const std::string &fontname, int x, int y, glm::vec4 color)
{
    TextUtils& textutils = TextUtils::getInstance();
    Font& font = textutils.getFont(fontname);

    int cursorX = x;
    for(char ch : text)
    {
        GlyphInfo& ginfo = font.getGlyphInfo((uint32_t)ch);

        if(ch == 'L' || ch == 'o')
            std::cout << "Bearing Y for " << ch << " is: " << ginfo.bearingY << "\n";

        mInstances.push_back(GlyphInstance{
            .position = { (float)(cursorX + ginfo.bearingX), (float)(y + ginfo.bearingY - ginfo.height) },
            .size = {ginfo.width, ginfo.height},
            .uv = {ginfo.u0, ginfo.v0, ginfo.u1, ginfo.v1},
            .color = color,
        });
        cursorX += (int)ginfo.advance;
    }

    Renderer& rend = Renderer::getInstance();
    mInstanceBuffer.setDevice(rend.getDevice());
    rend.createGlyphInstanceBuffer(mInstances, mInstanceBuffer.buffer, mInstanceBuffer.bufferMemory);
    mFontIndex = font.getDescriptorIndex();
}

void ke::Graphics::Text::TextInstance::Draw() const
{
    static Renderer& rend = Renderer::getInstance();
    
    rend.pickFontIndex(mFontIndex);
    rend.drawText(mInstanceBuffer, static_cast<uint32_t>(mInstances.size()));
}
