#ifndef MANGO_FONT_H
#define MANGO_FONT_H

#include <unordered_map>
#include <glm/vec2.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Font/AtlasManager.h"

namespace tomato {
    struct Glyph {
        glm::ivec2 size;
        glm::ivec2 bearing;
        unsigned int advance;

        glm::vec2 uvMin;
        glm::vec2 uvMax;

        int atlasIndex;
    };

    /**
     * @class Font
     * @brief Manages FreeType face loading and glyph generation.
     * * Note:
     * - When rendering text, you must use a specialized Font Shader.
     * - A Projection Matrix (e.g., Orthographic) is required to calculate
     * the correct text position on the screen.
     */
    class Font {
    public:
        constexpr static const char* PrimitiveName = "SpoqaHanSansNeo-Medium";
        constexpr static const char* defaultPath = "Assets/Fonts/SpoqaHanSansNeo-Medium.ttf";

    private:
        Font(const char* path = defaultPath);

    public:
        ~Font();

        static void Initialize();
        static void Cleanup();
        static void Create(const char* path = defaultPath);

        //void SetFontSize(int size);

        const Glyph& GetGlyph(char32_t codepoint);

    private:
        static FT_Library library_;
        static AtlasManager atlasManager_;

        const Glyph& LoadGlyph(char32_t codepoint);

        FT_Face face_;
        uint32_t baseSize_{ 48 };

        std::unordered_map<char32_t, Glyph> glyphs_;
    };
}

#endif //MANGO_FONT_H