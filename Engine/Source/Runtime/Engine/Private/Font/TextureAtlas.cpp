#include <glad/glad.h>
#include <algorithm>
#include "Font/TextureAtlas.h"

namespace tomato {
    TextureAtlas::TextureAtlas(int w, int h) : width_(w), height_(h) {
        glGenTextures(1, &textureID_);
        glBindTexture(GL_TEXTURE_2D, textureID_);

        // Allocate empty space first time
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    TextureAtlas::~TextureAtlas() {
        if (textureID_ != 0)
            glDeleteTextures(1, &textureID_);
    }

    bool TextureAtlas::Allocate(int w, int h, int& outX, int& outY)
    {
        // Add 1px padding to prevent texture bleeding
        int paddedW = w + 1;
        int paddedH = h + 1;

        // Check if the glyph fits in the current row
        if (currentX_ + paddedW > width_) // go next line
        {
            currentX_ = 0;
            currentY_ += maxRowHeight_;
            maxRowHeight_ = 0;
        }

        // Check if we ran out of vertical space in the atlas
        if (currentY_ + paddedH > height_) // full
        {
            isEmpty_ = false;
            return false;
        }

        outX = currentX_;
        outY = currentY_;

        // Advance the X position and update the row's maximum height
        currentX_ += paddedW;
        maxRowHeight_ = std::max(maxRowHeight_, paddedH);

        return true;
    }

    void TextureAtlas::Upload(int x, int y, int w, int h, unsigned char* data)
    {
        // Set pixel alignment to 1 byte (Required for grayscale font bitmaps)
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        Bind();
        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, w, h, GL_RED, GL_UNSIGNED_BYTE, data);
    }

    /**
     * [Usage Scenario]
     * 1. Bind your font shader: shader.Use();
     * 2. Bind this atlas: atlas.Bind(0);
     * 3. Set shader uniform: shader.SetInt("textTexture", 0);
     * 4. Perform draw calls: glDrawElements(...);
     *
     * @param unit The texture unit to bind to (default is 0).
     */
    void TextureAtlas::Bind(unsigned int unit) const
    {
        /**
        * Binds the atlas to a specific texture slot.
        * * Note: Texture units are shared globally.
        * Always call Bind() right before redering text to ensure the correct texture is used.
        */
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, textureID_);
    }

    void TextureAtlas::Clear()
    {
        currentX_ = currentY_ = maxRowHeight_ = 0;
    }
}