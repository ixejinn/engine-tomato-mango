#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "Resource/Render/Texture.h"
#include "Resource/AssetRegistry.h"
#include "Utils/Logger.h"

namespace tomato {
    Texture::Texture() : format_(ConvertFormatGL(Format::RGBA8))
    {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);

        // Create 1x1 solid white texture for default shading
        constexpr GLubyte white[4] = {255, 255, 255, 255};
        glTextureStorage2D(textureID_, 1, format_.internalFormat, 1, 1);
        glTextureSubImage2D(textureID_, 0, 0, 0, 1, 1, format_.format, format_.type, white);
    }

    Texture::Texture(const char* filename, Format format) : format_(ConvertFormatGL(format))
    {
        stbi_set_flip_vertically_on_load(true);

        // Create 2D texture
        glCreateTextures(GL_TEXTURE_2D, 1, &textureID_);

        // Set wrapping options
        glTextureParameteri(textureID_, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(textureID_, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Set filtering options
        glTextureParameteri(textureID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(textureID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int actualCh;
        unsigned char* image = stbi_load(filename, &width_, &height_, &actualCh, format_.channels);
        if (image)
        {
            if (format_.channels != 0 && actualCh > format_.channels)
                TMT_WARN << "Data loss occurs: " << filename;

            // Allocate immutable storage for the texture
            auto levels = static_cast<GLsizei>(std::floor(std::log2(std::max(width_, height_))) + 1);    // for mipmaps
            glTextureStorage2D(textureID_, levels, format_.internalFormat, width_, height_);

            // Get current alignment to restore it later
            GLint prevAlign = 0;
            glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlign);
            // Set to 1 byte alignment to handle any image format
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            // Upload pixel data to allocated storage
            glTextureSubImage2D(textureID_, 0, 0, 0, width_, height_, format_.format, format_.type, image);

            // Restore previous alignment
            glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlign);

            // Generate mipmaps for texture
            glGenerateTextureMipmap(textureID_);
        }
        else
            TMT_ERR << "Failed to load texture: " << filename;

        stbi_image_free(image);
    }

    Texture::Texture(const std::filesystem::path& path, Format format) : Texture(path.string().c_str(), format) {}

    Texture::Texture(const std::vector<const char*>& filenames, Format format) : format_(ConvertFormatGL(format))
    {
        if (filenames.size() < 6)
        {
            TMT_ERR << "Invalid cubemap texture vector size: " << filenames.size();
            return;
        }

        stbi_set_flip_vertically_on_load(false);

        glCreateTextures(GL_TEXTURE_CUBE_MAP, 1, &textureID_);

        // Set wrapping options
        glTextureParameteri(textureID_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureID_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureID_, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        // Set filtering options
        glTextureParameteri(textureID_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(textureID_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload textures for 6 sides
        int actualCh;
        unsigned char* images[6];
        for (int i = 0; i < 6; ++i)
        {
            images[i] = stbi_load(filenames[i], &width_, &height_, &actualCh, format_.channels);

            if (images[i] && format_.channels != 0 && actualCh > format_.channels)
                TMT_WARN << "Data loss occurs: " << filenames[i];
        }

        // Allocate immutable storage for the texture (6개 면을 위한 메모리가 한 번에 할당)
        glTextureStorage2D(textureID_, 1, format_.internalFormat, width_, height_);

        // Get current alignment to restore it later
        GLint prevAlign = 0;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &prevAlign);
        // Set to 1 byte alignment to handle any image format
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Copy textures to GPU
        for (int i = 0; i < 6; ++i)
        {
            if (images[i])
                glTextureSubImage3D(textureID_, 0, 0, 0, i, width_, height_, 1, format_.format, format_.type, images[i]);
            else
                TMT_ERR << "Failed to load cubemap texture: " << filenames[i];

            stbi_image_free(images[i]);
        }

        // Restore previous alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, prevAlign);
    }

    Texture::~Texture()
    {
        if (textureID_ != 0)
            glDeleteTextures(1, &textureID_);
    }

    void Texture::Create()
    {
        std::unique_ptr<Texture> whitePtr{new Texture};
        AssetRegistry<Texture>::GetInstance().Register(PrimitiveName, std::move(whitePtr));

        std::vector<const char*> skyboxFilenames(6);
        skyboxFilenames[0] = "Resources/Engine/Textures/Cubemaps/skybox/right.jpg";
        skyboxFilenames[1] = "Resources/Engine/Textures/Cubemaps/skybox/left.jpg";
        skyboxFilenames[2] = "Resources/Engine/Textures/Cubemaps/skybox/top.jpg";
        skyboxFilenames[3] = "Resources/Engine/Textures/Cubemaps/skybox/bottom.jpg";
        skyboxFilenames[4] = "Resources/Engine/Textures/Cubemaps/skybox/front.jpg";
        skyboxFilenames[5] = "Resources/Engine/Textures/Cubemaps/skybox/back.jpg";
        std::unique_ptr<Texture> skyboxPtr{new Texture(skyboxFilenames)};
        AssetRegistry<Texture>::GetInstance().Register("PrimitiveSkybox", std::move(skyboxPtr));
    }

    void Texture::Create(const char* filename, Format format)
    {
        std::unique_ptr<Texture> ptr{new Texture(filename, format)};
        AssetRegistry<Texture>::GetInstance().Register(filename, std::move(ptr));

        TMT_INFO << "Texture Registered " << filename;
    }

    void Texture::Create(const std::filesystem::path& path, Format format)
    {
        if(path.has_extension())
            Create(path.string().c_str(), format);
    }

    void Texture::Bind() const
    {
        // Bind texture to unit 0
        glBindTextureUnit(0, textureID_);
    }

    Texture::GLFormat Texture::ConvertFormatGL(Format format)
    {
        switch (format)
        {
            case Format::RGB8:
                return {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE, STBI_rgb};
            case Format::RGBA8:
            default:
                return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, STBI_rgb_alpha};
            case Format::SRGBA8:
                return {GL_SRGB8_ALPHA8, GL_RGBA, GL_UNSIGNED_BYTE, STBI_rgb_alpha};
            case Format::RGBA16F:
                return {GL_RGBA16F, GL_RGBA, GL_FLOAT, STBI_rgb_alpha};
            case Format::R8:
                return {GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1};
            case Format::Depth24Stencil8:
                return {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0};
        }
    }
}