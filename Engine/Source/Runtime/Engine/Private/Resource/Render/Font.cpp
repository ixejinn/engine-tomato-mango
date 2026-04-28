#include <string>
#include "Resource/Render/Font.h"
#include "Resource/AssetRegistry.h"
#include "Utils/Logger.h"

namespace tomato {
    FT_Library Font::library_ = nullptr;
    AtlasManager Font::atlasManager_;

    Font::Font(const char *path) {
        if (FT_New_Face(library_, path, 0, &face_))
            TMT_ERR << "FreeType: Failed to load font " << path;
        else
            FT_Set_Pixel_Sizes(face_, 0, baseSize_);
    }

    Font::~Font() {
        TMT_DEBUG << "~Font()";

        if (face_)
            FT_Done_Face(face_);
    }

    void Font::Initialize() {
        if (FT_Init_FreeType(&library_))
            TMT_ERR << "FreeType: Failed to initialize FreeType library";
    }

    void Font::Cleanup() {
        if (library_) {
            FT_Done_FreeType(library_);
            library_ = nullptr;
        }
    }

    void Font::Create(const char *path) {
        std::unique_ptr<Font> ptr{new Font(path)};

        std::string fullPath(path);

        // Find the last slash and the last dot
        size_t lastSlash = fullPath.find_last_of("/\\");
        size_t lastDot = fullPath.find_last_of(".");

        // Extract the file name without extension
        std::string assetKey;
        if (path == defaultPath)
            assetKey = PrimitiveName;
        else
        {
            size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
            size_t count = (lastDot == std::string::npos || lastDot < start)
                ? std::string::npos : lastDot - start;

            assetKey = fullPath.substr(start, count);
        }

        // Register using the parsed key
        AssetRegistry<Font>::GetInstance().Register(assetKey.c_str(), std::move(ptr));

        TMT_INFO << "Font Registered with key: " << assetKey;
    }

    const Glyph& Font::GetGlyph(char32_t codepoint)
    {
        auto it = glyphs_.find(codepoint);

        //If found, return the cached glyph
        if (it != glyphs_.end())
            return it->second;

        //If not found, load, cache, and return the reference
        return LoadGlyph(codepoint);
    }

    const Glyph& Font::LoadGlyph(char32_t codepoint)
    {
        if (FT_Load_Char(face_, codepoint, FT_LOAD_RENDER))
        {
            TMT_ERR << "ERROR::FreeType: Failed to load Glyph for: " << (uint32_t)codepoint;
            throw std::runtime_error("Failed to load Glyph");
        }

        FT_GlyphSlot slot = face_->glyph;
        int width = slot->bitmap.width;
        int rows = slot->bitmap.rows;

        auto res = atlasManager_.RequestAllocate(width, rows);

        TextureAtlas* atlas = atlasManager_.GetAtlas(res.atlasIndex);
        atlas->Upload(res.x, res.y, width, rows, slot->bitmap.buffer);

        // Calcuate UV coordinates (Normalized 0.0 to 1.0)
        // Divide pixel coordinates by atlas size to get normalized values.
        float atlasW = static_cast<float>(atlas->GetWidth());
        float atlasH = static_cast<float>(atlas->GetHeight());

        float uvX = static_cast<float>(res.x) / atlasW;
        float uvY = static_cast<float>(res.y) / atlasH;
        float uvW = static_cast<float>(width) / atlasW;
        float uvH = static_cast<float>(rows) / atlasH;

        Glyph glyph = {
            glm::ivec2(width, rows),
            glm::ivec2(slot->bitmap_left, slot->bitmap_top),
            static_cast<unsigned int>(slot->advance.x >> 6), // Divide by 64 to convert 26.6 fixed-point to pixels (Shift right by 6)

            glm::vec2(uvX, uvY), // UV Start point (Top-Left)
            glm::vec2(uvX + uvW, uvY + uvH), // UV End point (Bottom-Right)

            res.atlasIndex
        };

        return (glyphs_[codepoint] = glyph);
    }
}
